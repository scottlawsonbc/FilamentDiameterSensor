#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "tsl1401cl.h"
#include "edge-detection.h"
#include "dac.h"

/* Performs a forward-looking box filter on the given array
*
* This is a simple and fast box filter (also called box blur) that runs in O(n) time
* Successive iterations of this filter can approximate Gaussian blur
* The division of each box filter sum is neglected for efficiency
*/
void DET_BoxFilter(int32_t x[])
{
	uint32_t i;
	uint32_t movingSum = 0;
	uint32_t oldValue;
	const uint32_t divisor = DET_BOX_FILTER_LENGTH;
	//const uint32_t divisor = 1;


	/* Preparing the moving sum */
	for (i = 0; i < DET_BOX_FILTER_LENGTH; i++)
	{
		movingSum += x[i];
	}

	for (i = 0; i < TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i++)
	{
		oldValue = x[i];
		x[i] = movingSum / divisor;
		movingSum -= oldValue;
		movingSum += x[i + DET_BOX_FILTER_LENGTH];
	}

	/* Edge case handling */
	for (i = TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i < TSL_PIXEL_COUNT; i++)
	{
		x[i] = movingSum / divisor;
	}
}

/* Performs a centered box filter on the given array
 *
 * This filter averages over a box +N and -N pixels from the current pixel (where N is
 * DET_BOX_FILTER_LENGTH). A circular array is used to store the averages. This filter
 * runs in O(n) time. The division of each shifting sum is neglected for efficiency.
 */
void DET_BoxFilterCentered(int32_t x[])
{
	uint32_t i;
	uint32_t movingSum = 0;
	uint32_t storedAverages[DET_BOX_FILTER_LENGTH + 1];
	const uint32_t divisor = 2*DET_BOX_FILTER_LENGTH + 1;
	//const uint32_t divisor = 1;

	/* Preparing the moving average sum */
	for (i = 0; i < ((2*DET_BOX_FILTER_LENGTH) + 1); i++)
	{
		movingSum += x[i];
	}

	/* Rolling average */
	for (i = DET_BOX_FILTER_LENGTH; i < 2*DET_BOX_FILTER_LENGTH + 1; i++)
	{
		storedAverages[i % (DET_BOX_FILTER_LENGTH + 1)] = movingSum / divisor;
		movingSum += x[i + DET_BOX_FILTER_LENGTH + 1];
		movingSum -= x[i - DET_BOX_FILTER_LENGTH];
	}
	for (i = 2*DET_BOX_FILTER_LENGTH + 1; i < TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i++)
	{
		x[i - DET_BOX_FILTER_LENGTH - 1] = storedAverages[i % (DET_BOX_FILTER_LENGTH + 1)];
		storedAverages[i % (DET_BOX_FILTER_LENGTH + 1)] = movingSum / divisor;
		movingSum += x[i + DET_BOX_FILTER_LENGTH + 1];
		movingSum -= x[i - DET_BOX_FILTER_LENGTH];
	}
	for (i = TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i < TSL_PIXEL_COUNT+1; i++)
	{
		x[i - DET_BOX_FILTER_LENGTH - 1] = storedAverages[i % (DET_BOX_FILTER_LENGTH + 1)];
	}

	/* Edge case handling */
	for (i = 0; i < DET_BOX_FILTER_LENGTH; i++)
	{
		x[i] = x[DET_BOX_FILTER_LENGTH];
		x[TSL_PIXEL_COUNT - 1 - i] = x[TSL_PIXEL_COUNT - 1 - DET_BOX_FILTER_LENGTH];
	}
}

/* Approximates a convolution with a Gaussian kernel derivative
* 
* Approximation is achieved by successive iterations of a box filter.
* 3 iterations approximates a true Gaussian convolution with roughly 3% error
*/
void DET_ApproximateGaussianConvolution(int32_t x[])
{
	uint32_t i;
	for (i = 0; i < DET_GAUSSIAN_ITERATIONS; i++)
	{
		DET_BoxFilter(x);
	//	DET_BoxFilterCentered(x);
	}
}

/* Approximates a derivative using finite differences
*  Calculates f[x+1]-f[x]. To optimize for speed, this does not divide by h (2)
*/
void DET_FastFiniteDifferences(int32_t x[])
{
	uint32_t i;
	for (i = 0; i < TSL_PIXEL_COUNT - 1; i++)
	{
		x[i] = x[i+1]-x[i];
	}
	/* Edge case handling */
	x[TSL_PIXEL_COUNT-1] = x[TSL_PIXEL_COUNT-2];
}

/* Approximates a derivative using 1st order centered finite differences
*  Calculates f[x+1]-f[x-1]. To optimize for speed, this does not divide by 2*h (4)
*/
void DET_FastCenteredFiniteDifferences(int32_t x[])
{
	uint32_t i;
	uint32_t storage[2];

	storage[1] = x[2]-x[0];
	storage[0] = x[3]-x[1];
	for (i = 3; i < TSL_PIXEL_COUNT - 1; i++)
	{
		x[i - 2] = storage[i % 2];
		storage[i % 2] = x[i+1]-x[i-1];
	}
	x[TSL_PIXEL_COUNT - 3] = storage[(TSL_PIXEL_COUNT - 3) % 2];
	x[TSL_PIXEL_COUNT - 2] = storage[(TSL_PIXEL_COUNT - 2) % 2];

	/* Edge case handling */
	x[0] = x[1];
	x[TSL_PIXEL_COUNT-1] = x[TSL_PIXEL_COUNT-2];
}

/* Finds the indices of the maximum and minimum values in the given array */
void DET_MinMaxIndex(const int32_t x[], uint16_t *minIndex, uint16_t *maxIndex)
{
	uint32_t i;
	int32_t min = x[0], max = x[0];
	*minIndex = 0;
	*maxIndex = 0;
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		if (x[i] > max)
		{
			*maxIndex = i;
			max = x[i];
		}
		else if (x[i] < min)
		{
			*minIndex = i;
			min = x[i];
		}
	}
}

/* Performs quadratic interpolation on the given edge pixel and returns a floating point
 * estimate of subpixel edge position 
 */
inline float DET_SubpixelEdge(int32_t x[], uint16_t edgeIndex)
{
	float left, center, right;
	left   = (float)x[edgeIndex-1]; /* Pixel to the left of the edge */
	center = (float)x[edgeIndex];   /* Edge pixel */
	right  = (float)x[edgeIndex+1]; /* Pixel to the right of the edge */
	/* Quadratic interpolation of neighboring pixels */
	return (left-right)/(2.0f*(left+right-2.0f*center))+((float)edgeIndex); 
}

EdgeData DET_MicronsBetweenEdges(int32_t x[])
{
	/* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
	DET_ApproximateGaussianConvolution(x);

	/* Compute the derivative to expose the extrema */
	//DET_FastFiniteDifferences(x);
	DET_FastCenteredFiniteDifferences(x);

	/* The two edges are the extrema of the function */
	EdgeData edge_data = {0,0,0,true};
	uint16_t minEdge, maxEdge;
	DET_MinMaxIndex(x, &minEdge, &maxEdge);

	// Determine edge validity
	if ((minEdge >= maxEdge) ||
	   (minEdge == 0) || (maxEdge == TSL_PIXEL_COUNT-1) ||
	   ((maxEdge-minEdge)>DET_MAX_VALID_PIXEL_WIDTH) || 
	   ((maxEdge-minEdge)<DET_MIN_VALID_PIXEL_WIDTH))
	{
		edge_data.IsValid = false;
		return edge_data;
	}

	float subpixelMinEdge = DET_SubpixelEdge(x, minEdge);
	float subpixelMaxEdge = DET_SubpixelEdge(x, maxEdge);
	float microns = (subpixelMaxEdge-subpixelMinEdge)*(float)TSL_PIXEL_SPACING_NM/1000.0f;
	edge_data.E0 = subpixelMinEdge;
	edge_data.E1 = subpixelMaxEdge;
	edge_data.Width = microns;
	return edge_data;
}
//
///* Detects the edges and returns the distance in pixels between them
//* 
//* Edge detection is achieved through the following steps:
//* |> Convolve the array with a Gaussian derivative kernel to reduce noise
//* |> Take the derivative using finite differences
//* |> Locate the edges by extracting the minima and maxima of the derivative
//* |> Calculate number of pixels between edges, multiplied by the distance between each pixel
//*/
//uint32_t DET_PixelsBetweenEdges(int32_t x[])
//{	
//	/* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
//	DET_ApproximateGaussianConvolution(x);
//
//	/* Compute the derivative to expose the extrema */
//	DET_FastFiniteDifferences(x);
//
//	/* The two edges are the extrema of the function */
//	uint16_t edgeIndex0, edgeIndex1;
//	DET_MinMaxIndex(x, &edgeIndex0, &edgeIndex1);
//
//	/* Compute the distance width */
//	uint32_t pixelDistance = abs(edgeIndex0-edgeIndex1);
//
//	/* Optional debugging output */
//	return pixelDistance;
//}
//
///* Detects the edges and returns the distance in nanometers between them
//* 
//* Edge detection is achieved through the following steps:
//* |> Convolve the array with a Gaussian derivative kernel to reduce noise
//* |> Take the derivative using finite differences
//* |> Locate the edges by extracting the minima and maxima of the derivative
//* |> Calculate number of pixels between edges multiplied by the distance between each pixel
//*/
//uint32_t DET_NanometersBetweenEdges(int32_t x[])
//{
//	return (uint32_t)DET_PixelsBetweenEdges(x) * TSL_PIXEL_SPACING_NM;
//}
