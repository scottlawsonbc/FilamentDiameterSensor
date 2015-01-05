/*
 * Module prefix: MOV
 *
 * Provides a running average filter to reduce sensor noise
 */
#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <Arduino.h>

void MOV_Push(uint16_t x);
uint16_t MOV_Average();

#endif