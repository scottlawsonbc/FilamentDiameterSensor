#ifndef LED_BAR_H
#define LED_BAR_H

#include "stm32f30x_gpio.h"

#define LED_COUNT (5U)

/* Connected to GPIOD */
#define LED_0_PIN GPIO_Pin_9
#define LED_1_PIN GPIO_Pin_8
#define LED_2_PIN GPIO_Pin_11
#define LED_3_PIN GPIO_Pin_13
#define LED_4_PIN GPIO_Pin_15

extern const uint16_t LED_Pins[LED_COUNT];

extern void LED_Init();
extern void LED_Write(uint8_t ledIndex, BitAction state);

#endif