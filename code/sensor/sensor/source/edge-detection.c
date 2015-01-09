#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tsl1401cl.h"
#include "edge-detection.h"
#include "dac.h"

/* Performs a box filter on the given array
*
* This is a simple and fast box filter (also called box blur) that runs in O(n) time
* Successive iterations of this filter can approximate Gaussian blur
*/
void DET_BoxFilter(int32_t x[])
{
	uint32_t i;
	uint32_t movingSum = 0;
	for (i = 0; i < DET_BOX_FILTER_LENGTH; i++)
	{
		movingSum += x[i];
	}
	for (i = 0; i < TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i++)
	{
		x[i] = movingSum / DET_BOX_FILTER_LENGTH;
		movingSum -= x[i];
		movingSum += x[i + DET_BOX_FILTER_LENGTH];
	}
	/* Edge case handling */
	for (i = TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i < TSL_PIXEL_COUNT; i++)
	{
		x[i] = x[TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH];
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
	}
}

/* Approximates a derivative using finite differences
*  Calculates f[x]-f[x+1]. To optimize for speed, this does not divide by 2
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

/* Finds the indices of the maximum and minimum values in the given array */
void DET_MinMaxIndex(const int32_t x[], uint16_t *minIndex, uint16_t *maxIndex)
{
	uint32_t i;
	int32_t min = 0, max = 0;
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

inline float DET_SubpixelEdge(int32_t x[], uint16_t edgeIndex)
{
	float left, center, right;
	left   = (float)x[edgeIndex-1]; /* Pixel to the left of the edge */
	center = (float)x[edgeIndex];   /* Edge pixel */
	right  = (float)x[edgeIndex+1]; /* Pixel to the right of the edge */
	/* Quadratic interpolation of neighboring pixels */
	return (left-right)/(2.0f*(left+right-2.0f*center))+((float)edgeIndex); 
}

float DET_MicronsBetweenEdges(int32_t x[])
{
	/* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
	DET_ApproximateGaussianConvolution(x);

	/* Compute the derivative to expose the extrema */
	DET_FastFiniteDifferences(x);

	/* The two edges are the extrema of the function */
	uint16_t minEdge, maxEdge;
	DET_MinMaxIndex(x, &minEdge, &maxEdge);
	float subpixelMinEdge = DET_SubpixelEdge(x, minEdge);
	float subpixelMaxEdge = DET_SubpixelEdge(x, maxEdge);
	float nanometers = (subpixelMinEdge - subpixelMaxEdge)*(float)TSL_PIXEL_SPACING_NM/1000.0f;
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
