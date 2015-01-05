#ifndef DELAY_H
#define DELAY_H

#include <stm32f30x.h>
#include <stdint.h>

extern void DelayUs(__IO uint32_t microseconds);
extern void DelayMs(__IO uint32_t milliseconds);
extern void Delay_Init(void);

#endif