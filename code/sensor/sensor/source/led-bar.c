#include "led-bar.h"
#include <stm32f30x_rcc.h>
#include "stm32f30x_gpio.h"

const uint16_t LED_Pins[LED_COUNT] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = LED_0_PIN | LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_4_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;  /* Output pin */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  /* Push-pull configuration */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;   /* Pull-up */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Set the LED pins low */
	GPIO_WriteBit(GPIOD, LED_0_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_1_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_2_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_3_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOD, LED_4_PIN, Bit_RESET);
}

void LED_Write(uint8_t ledIndex, BitAction state)
{
	GPIO_WriteBit(GPIOD, LED_Pins[ledIndex], state);
}