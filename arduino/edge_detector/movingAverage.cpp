#include "movingAverage.h"
#include "configuration.h"

uint16_t MOV_Values[1U << CFG_MOVING_AVERAGE_SHIFTS];
uint32_t MOV_Sum = 0;

void MOV_Push(uint16_t x)
{
	MOV_Sum += x;
	MOV_Sum -= MOV_Values[0];
	for (uint16_t i = 0; i < (1U << CFG_MOVING_AVERAGE_SHIFTS)-1U; i++)
	{
		MOV_Values[i] = MOV_Values[i+1];
	}
	MOV_Values[(1U << CFG_MOVING_AVERAGE_SHIFTS)-1U] = x;
}

uint16_t MOV_Average()
{
	return MOV_Sum >> CFG_MOVING_AVERAGE_SHIFTS;
}