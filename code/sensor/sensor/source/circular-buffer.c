#include "circular-buffer.h"


uint16_t RING_Buffer[RING_MAX_ELEMENTS];
uint16_t RING_In;       /* Index of ring input  */
uint16_t RING_Out;      /* Index of ring output */
uint16_t RING_Elements; /* Specific number of values to store in circular buffer */

/* Initializes the ring buffer */
void RING_Init(void)
{
	RING_In = 0;
	RING_Out = 0;
	RING_Elements = RING_DEFAULT_ELEMENTS;
}

/* Sets the number of elements in the ring buffer. */
/* This also erases all data in the buffer */
/* Valid range: 0 < length < (RING_MAX_ELEMENTS-1) */
/* Return value == 0 -> SUCCESS */
/* Return value != 0 -> ERROR BOUNDS */
uint16_t RING_SetLength(uint16_t length)
{
	if ((length == 0) || (length >= RING_MAX_ELEMENTS-1))
	{
		return 0xFFFF; /* ERROR BOUNDS */
	}
	RING_Init();
	RING_Elements = length;
	return 0;
}

/* Pushes a new value onto the ring buffer */
/* Return value == 0 -> SUCCESS */
/* Return value != 0 -> ERROR FULL */
uint16_t RING_Push(uint16_t data)
{
	if (RING_In == (( RING_Out - 1 + RING_Elements) % RING_MAX_ELEMENTS))
	{
		return 0xFFFF; /* ERROR FULL */
	}
	RING_Buffer[RING_In] = data;
	RING_In = (RING_In + 1) % RING_Elements;
	return 0;
}

/* Pops the oldest value from the ring buffer */
/* Return value == 0 -> SUCCESS */
/* Return value != 0 -> ERROR EMPTY */
uint16_t RING_Pop(uint16_t* data)
{
	if (RING_In == RING_Out)
	{
		return 0xFFFF; /* ERROR EMPTY */
	}
	else
	{
		*data = RING_Buffer[RING_Out];
		RING_Out = (RING_Out + 1) % RING_Elements;
		return 0;
	}
}