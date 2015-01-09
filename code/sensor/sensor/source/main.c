#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_adc.h>
#include <stdio.h>
#include <math.h>
#include "dac.h"
#include "usart.h"
#include "delay.h"
#include "tsl1401cl.h"
#include "edge-detection.h"
#include "led-bar.h"

#define USART1_BAUDRATE (115200U)

int32_t intPixels[TSL_PIXEL_COUNT];

int main()
{
	USART1_Init(USART1_BAUDRATE);
	Delay_Init();
	TSL_Init();
	LED_Init();
	DAC_Setup();

	LED_Write(2, Bit_SET);

	while(1)
	{
		TSL_MeasurePixels(intPixels);
		DET_MicronsBetweenEdges(intPixels);		
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