#include "tsl1401cl.h"
#include "edge-detection.h"
#include <stdint.h>

/* Performs a box filter on the given array
 *
 * This is a simple and fast box filter (also called box blur) that runs in linear O(n) time
 * Successive iterations of this filter can approximate Gaussian blur
 */
void DET_BoxFilterFloat(float x[])
{
	uint16_t i;
	float movingSum = 0.0f;
	for (i = 0; i < DET_BOX_FILTER_LENGTH; i++)
	{
		movingSum += x[i];
	}
	for (i = 0; i < TSL_PIXEL_COUNT - DET_BOX_FILTER_LENGTH; i++)
	{
		x[i] = movingSum / ((float)DET_BOX_FILTER_LENGTH);
		movingSum -= x[i];
		movingSum += x[i + DET_BOX_FILTER_LENGTH];
	}
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
void DET_ApproximateGaussianConvolutionFloat(float x[])
{
	uint16_t i;
	for (i = 0; i < DET_GAUSSIAN_ITERATIONS; i++)
	{
		DET_BoxFilterFloat(x);
	}
}

/* Approximates a derivative using finite differences
 * 
 * Calculates f[x]-f[x+1]. To optimize for speed, this does not divide by 2
 * 
 */
void DET_FastFiniteDifferencesFloat(float x[])
{
	uint16_t i;
	for (i = 0; i < TSL_PIXEL_COUNT - 1; i++)
	{
		x[i] = x[i]-x[i+1];
	}
	x[TSL_PIXEL_COUNT-1] = x[TSL_PIXEL_COUNT-2]; // Extend the array to handle the edge case
}

/* Finds the maximum of the given array
 * 
 * The dereferecned index pointer is assigned to the index that contains the maximum value
 * The dereferenced value pointer is assigned to the maximum value
 */
void DET_MaxFloat(float x[], uint16_t *index, float *value)
{
	uint16_t i;
	*value = 0.0f;
	*index = 0;
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		if (x[i] > *value)
		{
			*value = x[i];
			*index = i;
		}
	}
}

/* Finds the maximum of the given array
 * 
 * The dereferecned index pointer is assigned to the index that contains the minimum value
 * The dereferenced value pointer is assigned to the minimum value
 */
void DET_MinFloat(float x[], uint16_t *index, float *value)
{
	uint16_t i;
	*value = 0.0f;
	*index = 0;
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		if (x[i] < *value)
		{
			*value = x[i];
			*index = i;
		}
	}
}

/* Detects the edges and returns the distance in nanometers between them
 * 
 * Edge detection is achieved through the following steps:
 * |> Convolve the array with a Gaussian derivative kernel to reduce noise
 * |> Take the derivative using finite differences
 * |> Locate the edges by extracting the minima and maxima of the derivative
 * |> Calculate number of pixels between edges multiplied by the distance between each pixel
 */
uint32_t DET_NanometersBetweenEdgesFloat(float x[])
{
	/* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
	DET_ApproximateGaussianConvolutionFloat(x);

	/* Compute the derivative to expose the extrema */
	DET_FastFiniteDifferencesFloat(x);

	/* The two edges are the extreme of the function */
	uint16_t edgeIndex0 = 0, edgeIndex1 = 0;
	float edgeValue0 = 0, edgeValue1 = 0;
	DET_MinFloat(x, &edgeIndex0, &edgeValue0); /* Get the first edge */
	DET_MaxFloat(x, &edgeIndex1, &edgeValue1); /* Get the second edge */
		
	/* Compute the distance width */
	uint16_t pixelDistance      = (edgeIndex0 > edgeIndex1) ? (edgeIndex0-edgeIndex1):(edgeIndex1-edgeIndex0);
	uint32_t nanometerDistance  = (uint32_t)pixelDistance * TSL_PIXEL_SPACING_NM;

	/* Optional debugging output */
	return nanometerDistance;
}

uint16_t DET_PixelsBetweenEdgesFloat(float x[])
{
	/* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
	DET_ApproximateGaussianConvolutionFloat(x);

	/* Compute the derivative to expose the extrema */
	DET_FastFiniteDifferencesFloat(x);

	/* The two edges are the extreme of the function */
	uint16_t edgeIndex0 = 0, edgeIndex1 = 0;
	float edgeValue0 = 0, edgeValue1 = 0;
	DET_MinFloat(x, &edgeIndex0, &edgeValue0); /* Get the first edge */
	DET_MaxFloat(x, &edgeIndex1, &edgeValue1); /* Get the second edge */
		
	/* Compute the distance width */
	uint16_t pixelDistance = (edgeIndex0 > edgeIndex1) ? (edgeIndex0-edgeIndex1):(edgeIndex1-edgeIndex0);

	return pixelDistance;
}
