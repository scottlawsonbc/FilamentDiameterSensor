/*
 * Module prefix: TSL
 * Methods for interfacing with the TSL1401CL linear photodiode array.
 */
#ifndef TSL1401CL_H
#define TSL1401CL_H

#include <Arduino.h>

void TSL_MeasurePixels(int16_t x[]);
void TSL_Init();

#endif