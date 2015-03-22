#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_adc.h>
#include "stm32f30x_dac.h"
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
#include "i2c.h"

#define USART1_BAUDRATE (115200U)
char buffer[60];
int32_t xPixels[TSL_PIXEL_COUNT];
int32_t yPixels[TSL_PIXEL_COUNT];

int main()
{
	//USART1_Init(USART1_BAUDRATE);
	Delay_Init();
	TSL_Init();
	LED_Init();
	DAC_SingleValue_Setup();
	//I2C_Config();

	/* Turn on lasers */
	LED_Write(0, Bit_SET);
	LED_Write(1, Bit_SET);

	const int iterations = 6000;
	while(1)
	{
		int i;
		EdgeData x_edge_sum = {0,0,0,1};
		EdgeData y_edge_sum = {0,0,0,1};
		for (i = 1; i < iterations+1; i++)
		{
			TSL_MeasurePixels(xPixels, yPixels);
			EdgeData edge_x = DET_MicronsBetweenEdges(xPixels);
			EdgeData edge_y = DET_MicronsBetweenEdges(yPixels);

			if (edge_x.IsValid && edge_y.IsValid)
			{
				/* Add the x-result to the sum */
				x_edge_sum.E0    += edge_x.E0;
				x_edge_sum.E1    += edge_x.E1;
				x_edge_sum.Width += edge_x.Width;

				/* Add the y-result to the sum */
				y_edge_sum.E0    += edge_y.E0;
				y_edge_sum.E1    += edge_y.E1;
				y_edge_sum.Width += edge_y.Width;
			}
			else
			{
				i -= 1; /* Reject the measurement */
			}
			LED_Write(2, (BitAction)(edge_x.IsValid));
			LED_Write(3, (BitAction)(edge_y.IsValid));
		}

		x_edge_sum.E0    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)iterations;
		x_edge_sum.E1    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)iterations;
		x_edge_sum.Width /= (float)iterations;
		y_edge_sum.E0    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)iterations;
		y_edge_sum.E1    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)iterations;
		y_edge_sum.Width /= (float)iterations;

		float diameter = GEO_Filament_Diameter_MM(x_edge_sum, y_edge_sum);
		DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)((diameter*1000.0)*(4096.0/3000.0)));
		//uprintf("Diameter: %f\r\n", diameter);
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