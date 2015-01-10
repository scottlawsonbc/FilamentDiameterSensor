#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_adc.h>
#include "tsl1401cl.h"
#include "delay.h"

/* Quickly toggles the CLK pin high and low */
inline void TSL_ClockPulse(void)
{
	GPIO_WriteBit(GPIOE, TSL_CLOCK_PIN, Bit_SET);   /* Rising edge */
	DelayUs(TSL_CLOCK_DELAY_US);
	GPIO_WriteBit(GPIOE, TSL_CLOCK_PIN, Bit_RESET); /* Falling edge */
}

/* Sends a clock pulse with a high SI bit, then sets SI low */
inline void TSL_StartOutputCycle(void)
{
	/* Rising edge */
	GPIO_WriteBit(GPIOE, TSL_SERIAL_PIN, Bit_SET);
	DelayUs(1); /* Make sure SI pin is high before CLK goes high */
	GPIO_WriteBit(GPIOE, TSL_CLOCK_PIN, Bit_SET);
	DelayUs(TSL_CLOCK_DELAY_US);

	/* Falling edge */
	GPIO_WriteBit(GPIOE, TSL_SERIAL_PIN, Bit_RESET);
	DelayUs(1); /* Make sure SI pin is low before CLK goes low */
	GPIO_WriteBit(GPIOE, TSL_CLOCK_PIN, Bit_RESET);
}

inline uint16_t TSL_AnalogRead(void)
{
	ADC_StartConversion(ADC1);                              /* Trigger an ADC measurement */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);  /* Wait for ADC to be ready */
	return ADC_GetConversionValue(ADC1);                    /* Return the ADC result */
}

void TSL_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = TSL_CLOCK_PIN | TSL_SERIAL_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;  /* Output pin */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  /* Push-pull configuration */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;   /* Pull-up */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Set the serial and clock pins low */
	GPIO_WriteBit(GPIOE, TSL_CLOCK_PIN, Bit_RESET);
	GPIO_WriteBit(GPIOE, TSL_SERIAL_PIN, Bit_RESET);
}

void TSL_ADC_Configuration(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div2);             /* Set ADC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE); /* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); /* GPIOC Periph clock enable */

	GPIO_InitStructure.GPIO_Pin  = TSL_ANALOG_INPUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	ADC_StructInit(&ADC_InitStructure);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
	ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock            = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;             
	ADC_CommonInitStructure.ADC_DMAMode          = ADC_DMAMode_OneShot;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
	ADC_InitStructure.ADC_ContinuousConvMode     = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent  = ADC_ExternalTrigConvEvent_0;         
	ADC_InitStructure.ADC_ExternalTrigEventEdge  = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode            = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode            = ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel        = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY)); /* Wait for ADC to be ready */
}

void TSL_Sensor_Init(void)
{
	uint16_t i;
	/* Clock out indeterminant data on power up */
	for (i = TSL_PIXEL_COUNT+1; i > 0; i--)
	{
		TSL_ClockPulse();
	}
	/* Start an output cycle and discard the first set of data */
	TSL_StartOutputCycle();
	for (i = TSL_PIXEL_COUNT+1; i > 0; i--)
	{
		TSL_ClockPulse();
	}
}

/* Initialize the digital pins, ADC, and TSL1401Cl sensor */
void TSL_Init(void)
{
	TSL_GPIO_Configuration();
	TSL_ADC_Configuration();
	TSL_Sensor_Init();
}

void TSL_MeasurePixels(int32_t *pixels)
{
	uint16_t i, j;
	/* Clear indeterminant data from the sensor */
	TSL_StartOutputCycle();
	for (i = 0; i < TSL_PIXEL_COUNT; i++) 
	{
		TSL_ClockPulse();
	}
	TSL_ClockPulse();
	DelayUs(TSL_SENSOR_DELAY_US);

	/* Assign values to the first measurement */;
	TSL_StartOutputCycle();
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		pixels[i] = TSL_AnalogRead();
		TSL_ClockPulse();
	}

	/* Clock out the high SI bit from the shift register */
	/* This returns the analog output to a known high impedance state */
	TSL_ClockPulse();
}
