#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdint.h>

/* Edge detection settings */
#define DET_BOX_FILTER_LENGTH (3U)
#define DET_GAUSSIAN_ITERATIONS (3U)

/* Edge detection methods using integer operations */
extern uint32_t DET_NanometersBetweenEdges(int16_t x[]);
extern uint16_t DET_PixelsBetweenEdges(int16_t x[]);

/* Edge detection using floating point operations with the hardware FPU */
extern uint32_t DET_NanometersBetweenEdgesFloat(float x[]);
extern uint16_t DET_PixelsBetweenEdgesFloat(float x[]);

#endif