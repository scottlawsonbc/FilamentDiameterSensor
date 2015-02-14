#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdint.h>

typedef struct
{
	/* Distance between edges */
	float Width;
	/* Leftmost edge index */
	float E0;
	/* Rightmost edge index */
	float E1;
} EdgeData;

/* Edge detection settings */
#define DET_BOX_FILTER_LENGTH (3U)
#define DET_GAUSSIAN_ITERATIONS (1U)

/* Edge detection methods using integer operations */
extern EdgeData DET_MicronsBetweenEdges(int32_t x[]);

#endif