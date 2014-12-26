#include "detection.h"
#include "configuration.h"

/* Performs a box filter on the given array
 *
 * This is a simple and fast box filter (also called box blur) that runs in linear O(n) time
 * Successive iterations of this filter can approximate Gaussian blur
 */
void DET_BoxFilter(int16_t x[])
{
    uint16_t i;
    uint16_t movingSum = 0;
    const uint8_t boxFilterWindow = 1 << CFG_BOX_FILTER_SHIFTS;
    for (i = 0; i < boxFilterWindow; i++)
    {
        movingSum += x[i];
    }
    for (i = 0; i < CFG_PIXEL_COUNT - boxFilterWindow; i++)
    {
        x[i] = movingSum >> CFG_BOX_FILTER_SHIFTS;
        movingSum -= x[i];
        movingSum += x[i + boxFilterWindow];
    }
    for (i = CFG_PIXEL_COUNT - boxFilterWindow; i < CFG_PIXEL_COUNT; i++)
    {
        x[i] = x[CFG_PIXEL_COUNT - boxFilterWindow];
    }
}
 
/* Approximates a convolution with a Gaussian kernel derivative
 * 
 * Approximation is achieved by successive iterations of a box filter.
 * 3 iterations approximates a true Gaussian convolution with roughly 3% error
 */
void DET_ApproximateGaussianConvolution(int16_t x[])
{
	for (uint8_t i = 0; i < CFG_GAUSSIAN_ITERATIONS; i++)
	{
		DET_BoxFilter(x);
	}
}

/* Approximates a derivative using finite differences
 * 
 * Calculates f[x]-f[x+1]. To optimize for speed, this does not divide by 2
 * 
 */
void DET_FastFiniteDifferences(int16_t x[])
{
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT - 1; i++)
    {
    	x[i] = x[i]-x[i+1];
    }
    x[CFG_PIXEL_COUNT-1] = x[CFG_PIXEL_COUNT-2]; // Extend the array to handle the edge case
}

/* Finds the maximum of the given array
 * 
 * The dereferecned index pointer is assigned to the index that contains the maximum value
 * The dereferenced value pointer is assigned to the maximum value
 */
void DET_Max(int16_t x[], uint16_t *index, int16_t *value)
{
	*value = 0;
	*index = 0;
	for (uint16_t i = 0; i < CFG_PIXEL_COUNT; i++)
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
void DET_Min(int16_t x[], uint16_t *index, int16_t *value)
{
    *value = 0;
    *index = 0;
    for (uint16_t i = 0; i < CFG_PIXEL_COUNT; i++)
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
uint32_t DET_NanometersBetweenEdges(int16_t x[])
{
    /* Approximate a convolution with a gaussian kernel derivative to reduce noise */  
    DET_ApproximateGaussianConvolution(x);
    PRINT("Gaussian\r\n"); PRINT_ARRAY(x);

    /* Compute the derivative to expose the extrema */
    DET_FastFiniteDifferences(x);
    PRINT("Derivative\r\n"); PRINT_ARRAY(x);

    /* The two edges are the extreme of the function */
    uint16_t edgeIndex0 = 0, edgeIndex1 = 0;
    int16_t  edgeValue0 = 0, edgeValue1 = 0;
    DET_Min(x, &edgeIndex0, &edgeValue0); /* Get the first edge */
    DET_Max(x, &edgeIndex1, &edgeValue1); /* Get the second edge */
        
    /* Compute the distance width */
    uint16_t pixelDistance      = (edgeIndex0 > edgeIndex1) ? (edgeIndex0-edgeIndex1):(edgeIndex1-edgeIndex0);
    uint32_t nanometerDistance  = (uint32_t)pixelDistance * CFG_PIXEL_SPACING_NM;

    /* Optional debugging output */
    PRINT_VAL("Edge 0 index: ", edgeIndex0);
    PRINT_VAL("Edge 1 index: ", edgeIndex1);
    PRINT_VAL("Edge 0 value: ", edgeValue0);
    PRINT_VAL("Edge 1 value: ", edgeValue1);
    PRINT_VAL("Distance: ", nanometerDistance);

    return nanometerDistance;
}
