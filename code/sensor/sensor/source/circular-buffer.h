#ifndef RING_H
#define RING_H

#include <stdint.h>

/* Maximum possible values in circular buffer */
#define RING_MAX_ELEMENTS (2000U)
#define RING_SIZE (RING_MAX_ELEMENTS+1)

#define RING_DEFAULT_ELEMENTS (100U)

extern void RING_Init(void);
extern uint16_t RING_SetLength(uint16_t length);
extern uint16_t RING_Push(uint16_t data);
extern uint16_t RING_Pop(uint16_t* data);

#endif