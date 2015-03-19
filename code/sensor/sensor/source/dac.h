#ifndef DAC_OUTPUT_H
#define DAC_OUTPUT_H

#include "stm32f30x_gpio.h"
#include "tsl1401cl.h"

/* Toggle DAC debugging (affects performance) */
#define DAC_DEBUG_OUTPUT (1U)

/* Base address for direct memory access */
#define DAC_DHR12RD_Address 0x40007420

/* DAC channels connected to GPIOA */
#define DAC_OUTPUT_PIN_0 (GPIO_Pin_4)
#define DAC_OUTPUT_PIN_1 (GPIO_Pin_5)

extern void DAC_SetCH1(uint32_t x[]);
extern void DAC_SetCH2(uint32_t x[]);
extern void DAC_Setup();

#endif