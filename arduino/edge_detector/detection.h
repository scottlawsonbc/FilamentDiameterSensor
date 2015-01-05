/*
 * Module prefix: DET
 * The detection module provides portable code for 1-dimensional edge detection.
 * Edge detection has O(n) algorithmic complexity, where n is the number of pixels
 */
#ifndef DETECTION_H
#define DETECTION_H

#include <Arduino.h>

uint32_t DET_NanometersBetweenEdges(int16_t x[]);
uint8_t DET_PixelsBetweenEdges(int16_t x[]);
#endif