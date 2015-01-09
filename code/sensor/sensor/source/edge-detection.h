#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdint.h>

/* Edge detection settings */
#define DET_BOX_FILTER_LENGTH (3U)
#define DET_GAUSSIAN_ITERATIONS (1U)

/* Edge detection methods using integer operations */
extern float DET_MicronsBetweenEdges(int32_t x[]);

#endif