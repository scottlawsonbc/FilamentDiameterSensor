#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdint.h>
#include <stdbool.h>
#include "tsl1401cl.h"

/* Edge detection settings */
#define DET_BOX_FILTER_LENGTH (2U)
#define DET_GAUSSIAN_ITERATIONS (1U)

/* Heuristic parameters for determining edge validity */
#define DET_MIN_VALID_PIXEL_WIDTH ((uint16_t)(1.5 * (1000.0*1000.0) / TSL_PIXEL_SPACING_NM)) 
#define DET_MAX_VALID_PIXEL_WIDTH ((uint16_t)(7.5 * (1000.0*1000.0) / TSL_PIXEL_SPACING_NM))

typedef struct
{
	/* Distance between edges */
	float Width;
	/* Leftmost edge index */
	float E0;
	/* Rightmost edge index */
	float E1;
	/* Indicates whether valid filamnet was detected */
	bool IsValid;	
} EdgeData;

/* Edge detection methods using integer operations */
extern EdgeData DET_MicronsBetweenEdges(int32_t x[]);

#endif