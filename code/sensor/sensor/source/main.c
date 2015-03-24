#include "main.h"

char buffer[60];
int32_t xPixels[TSL_PIXEL_COUNT];
int32_t yPixels[TSL_PIXEL_COUNT];

uint16_t MAIN_AveragingIterations = MAIN_DEFAULT_AVERAGING;
FunctionalState MAIN_SensorState = DISABLE;
float MAIN_FilamentDiameter_MM = 0;

int main()
{
	USART1_Init(USART1_BAUDRATE);
	Delay_Init();
	TSL_Init();
	LED_Init();
	DAC_SingleValue_Setup();
	I2C_Config();

	MAIN_SetSensorState(ENABLE);

	while(1)
	{
		if (MAIN_SensorState == ENABLE)
		{
			uint16_t i;
			EdgeData x_edge_sum = {0,0,0,1};
			EdgeData y_edge_sum = {0,0,0,1};
			for (i = 1; i < MAIN_AveragingIterations+1; i++)
			{
				/* Measure the pixel array sensors and determine the shadow edges */
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
				/* Display the filament validity detection on the LEDs */
				LED_Write(2, (BitAction)(edge_x.IsValid));
				LED_Write(3, (BitAction)(edge_y.IsValid));

				/* Check the I2C bus to see if a command was recieved */
				I2C_CheckReceive();
			}

			/* Divide the sum by the number of sampling iterations to get the averaged data */
			x_edge_sum.E0    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)MAIN_AveragingIterations;
			x_edge_sum.E1    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)MAIN_AveragingIterations;
			x_edge_sum.Width /= (float)MAIN_AveragingIterations;
			y_edge_sum.E0    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)MAIN_AveragingIterations;
			y_edge_sum.E1    *= (TSL_PIXEL_SPACING_NM / 1000.0f / 1000.0f) / (float)MAIN_AveragingIterations;
			y_edge_sum.Width /= (float)i;

			/* Compute the filament diameter */
			float diameter = GEO_Filament_Diameter_MM(x_edge_sum, y_edge_sum);
			/* Update the most recent filament diameter measurement */			
			MAIN_FilamentDiameter_MM = diameter;
			uprintf("Diameter: %f\r\n", diameter);
			/* Set analog output to converted filament diameter */
			DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)((diameter*1000.0)*(4096.0/3000.0)));
		}
		else
		{
			/* Check the I2C bus to see if data was received */
			I2C_CheckReceive();
		}
	}
}

/* Change the state of the sensor */
void MAIN_SetSensorState(FunctionalState state)
{
	MAIN_SensorState = state;
	LED_Write(0, (BitAction)state);
	LED_Write(1, (BitAction)state);
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