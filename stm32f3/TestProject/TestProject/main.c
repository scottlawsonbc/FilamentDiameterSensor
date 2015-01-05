#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_adc.h>
#include <stdio.h>
#include "usart.h"
#include "delay.h"
#include "tsl1401cl.h"
#include "edge-detection.h"
#include <math.h>

#define USART1_BAUDRATE (115200U)

int16_t pixels[TSL_PIXEL_COUNT];
float pixelsFloat[TSL_PIXEL_COUNT];
char text[60];

#define MOVING_AVG_SIZE (64U)
uint32_t avg[MOVING_AVG_SIZE];
uint32_t sum = 0;

float avgFloat[MOVING_AVG_SIZE];
float sumFloat = 0;

void Push(uint32_t x)
{
	uint16_t i;
	sum -= avg[0];
	sum += x;
	for (i = 0; i < MOVING_AVG_SIZE-1; i++)
	{
		avg[i] = avg[i+1];
	}
	avg[MOVING_AVG_SIZE-1] = x;
}

void PushFloat(float x)
{
	uint16_t i;
	sumFloat -= avgFloat[0];
	sumFloat += x;
	for (i = 0; i < MOVING_AVG_SIZE-1; i++)
	{
		avgFloat[i] = avgFloat[i+1];
	}
	avgFloat[MOVING_AVG_SIZE-1] = x;
}

uint32_t Pop(void)
{
	return sum / MOVING_AVG_SIZE;
}

float PopFloat(void)
{
	return sumFloat / (float)MOVING_AVG_SIZE;
}

void measure(void)
{
	/* Take a bunch of measurements */
	uint16_t i;
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_SET);
	for (i = 0; i < MOVING_AVG_SIZE; i++)
	{
		TSL_MeasurePixels(pixels);
		Push(DET_NanometersBetweenEdges(pixels));
	}
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_RESET);
	for (i = 0; i < MOVING_AVG_SIZE; i++)
	{
		TSL_MeasurePixels(pixels);
		Push(DET_NanometersBetweenEdges(pixels));
	}


	///* Send the averaged distance */
	//float distance = ((float)Pop()) / 1000.0;
	//distance /= 1000.0;
	//sprintf(text, "Distance: %f mm", distance);
	//USART1_SendLine(text);	
}

void measureFloat(void)
{
	/* Take a bunch of measurements */
	uint16_t i;
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_SET);
	for (i = 0; i < MOVING_AVG_SIZE; i++)
	{
		TSL_MeasurePixelsFloat(pixelsFloat);
		Push(DET_NanometersBetweenEdgesFloat(pixelsFloat));
	}
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_RESET);
	for (i = 0; i < MOVING_AVG_SIZE; i++)
	{
		TSL_MeasurePixelsFloat(pixelsFloat);
		Push(DET_NanometersBetweenEdgesFloat(pixelsFloat));
	}

	///* Send the averaged distance */
	//float distance = ((float)Pop()) / 1000.0;
	//distance /= 1000.0;
	//sprintf(text, "Distance: %f mm", distance);5
	//USART1_SendLine(text);	
}

int main()
{
	Delay_Init();
	USART1_Init(USART1_BAUDRATE);
	TSL_Init();

	while(1)
	{
		measure();
	}
}
//****************************************************************************

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif