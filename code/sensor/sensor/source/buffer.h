#ifndef BUFF_H
#define BUFF_H

#include "main.h"
#include "circular-buffer.h"

#define BUFF_FLOAT_TO_INT_MULTIPLIER (16384U)

/* Initializes the ring buffer for volumetric flow ratios */
extern void BUFF_InitializeBuffer(void);

/* Returns a volumetric flow multiplier from the ring buffer, adds new value to buffer */
extern uint16_t BUFF_GetBufferMultiplier(void);

#endif