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
#include "opamp.h"
#include "geometry.h"

#define USART1_BAUDRATE (115200U)
char buffer[60];
int32_t xPixels[TSL_PIXEL_COUNT];
int32_t yPixels[TSL_PIXEL_COUNT];

int main()
{
	USART1_Init(USART1_BAUDRATE);
	Delay_Init();
	TSL_Init();
	LED_Init();
	
	// Turn on the sensor LEDs
	LED_Write(0, Bit_SET);
	LED_Write(1, Bit_SET);


	while(1)
	{
		int i;
	double sum = 0.0;

		for (i = 0; i < 10; i++)
		{
			TSL_MeasurePixels(xPixels, yPixels);
			EdgeData resultX = DET_MicronsBetweenEdges(xPixels);
			EdgeData resultY = DET_MicronsBetweenEdges(yPixels);

			resultX.E0 *= 62.5f * 1000.0f;
			resultX.E1 *= 62.5f * 1000.0f;
			resultX.Width *= 1000.0f;

			resultY.E0 *= 62.5f * 1000.0f;
			resultY.E1 *= 62.5f * 1000.0f;
			resultY.Width *= 1000.0f;

			float total = GEO_Filament_Diameter_MM(resultX, resultY);
			sum += total;
		}
		sprintf(buffer, "Diameter: %f", ((float)sum)*100.0f);
		USART1_SendLine(buffer);
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