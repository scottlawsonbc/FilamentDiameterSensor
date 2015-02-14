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

/* Sets SI high for the rising edge of a clock pulse, then sets SI low before the falling edge.
 *
 * This initiates an output cycle for the following 128 clock cycles.
 * Photodiode integration for the next cycle begins on the 19th clock pulse folowing
 * the start of an output cycle.
 */
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

inline void TSL_DualAnalogRead(int32_t *ADC1_Value, int32_t *ADC2_Value)
{
	ADC_StartConversion(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); /* Wait for ADC to be ready */
	*ADC1_Value = ADC_GetDualModeConversionValue(ADC1);	   /* Both ADC values stored in single uint32_t */
	*ADC2_Value = (uint32_t)(((uint32_t)*ADC1_Value) & 0x0000FFFF);				   /* ADC2 value stored in lower 16 bits */
	*ADC1_Value = (uint32_t)(((uint32_t)*ADC1_Value) & 0xFFFF0000);				   /* ADC1 value stored in upper 16 bits */
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

void TSL_ADC1_Configuration(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);             /* Set ADC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE); /* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); /* GPIOA Periph clock enable */

	GPIO_InitStructure.GPIO_Pin  = TSL_ADC1_INPUT_PIN; /* Connect the ADC to PINA and PINB */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	ADC_StructInit(&ADC_InitStructure);
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	while(ADC_GetCalibrationStatus(ADC1) != RESET);
	ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_RegSimul;                                                                    
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
	ADC_RegularChannelConfig(ADC1, TSL_ADC1_CHANNEL, 1, ADC_SampleTime_1Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY)); /* Wait for ADC to be ready */
}

void TSL_ADC2_Configuration(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);             /* Set ADC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE); /* Enable ADC1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); /* GPIOA Periph clock enable */

	GPIO_InitStructure.GPIO_Pin  = TSL_ADC2_INPUT_PIN; /* Connect the ADC to PINA and PINB */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	ADC_StructInit(&ADC_InitStructure);
	ADC_VoltageRegulatorCmd(ADC2, ENABLE);
	while(ADC_GetCalibrationStatus(ADC2) != RESET );
	ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_RegSimul;                                                                    
	ADC_CommonInitStructure.ADC_Clock            = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;             
	ADC_CommonInitStructure.ADC_DMAMode          = ADC_DMAMode_OneShot;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC2, &ADC_CommonInitStructure);
	ADC_InitStructure.ADC_ContinuousConvMode     = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent  = ADC_ExternalTrigConvEvent_0;         
	ADC_InitStructure.ADC_ExternalTrigEventEdge  = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode            = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode            = ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel        = 1;
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC2, TSL_ADC2_CHANNEL, 1, ADC_SampleTime_1Cycles5);
	ADC_Cmd(ADC2, ENABLE);

	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY)); /* Wait for ADC to be ready */
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
	TSL_ADC1_Configuration();
	TSL_ADC2_Configuration();
	TSL_Sensor_Init();
}

void TSL_MeasurePixels(int32_t *x_pixels, int32_t *y_pixels)
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
		TSL_DualAnalogRead(&(x_pixels[i]), &(y_pixels[i]));
		//pixels[i] = TSL_AnalogRead();
		TSL_ClockPulse();
	}

	/* Clock out the high SI bit from the shift register */
	/* This returns the analog output to a known high impedance state */
	TSL_ClockPulse();
}
