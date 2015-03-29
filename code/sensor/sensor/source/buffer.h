#ifndef BUFF_H
#define BUFF_H

#include "main.h"
#include "circular-buffer.h"

#define BUFF_FLOAT_TO_INT_MULTIPLIER (1U << 10U)

/* Initializes the ring buffer for volumetric flow ratios */
extern uint16_t BUFF_InitializeBuffer(void);

/* Returns a volumetric flow multiplier from the ring buffer, adds new value to buffer */
extern uint16_t BUFF_UpdateBufferMultiplier(void);

#endif