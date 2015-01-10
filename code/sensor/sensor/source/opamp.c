#include "stm32f30x_adc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_opamp.h"
#include "stm32f30x_rcc.h"
#include "opamp.h"

/* GPIO configuration */
void PGA_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Configure PA.05 (DAC_OUT2) as analog */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* OPAMP configuration */
void PGA_OPAMPConfig(void)
{
	OPAMP_InitTypeDef OPAMP_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* PA5 is used as OPAMP1 non inverting input: it is already configured in
	analog mode in DAC_Config() */

	/* GPIOA Peripheral clock is already enabled in DAC_Config() */

	/* Configure PA2 analog mode: It is used as OPAMP1 output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* OPAMP Peripheral clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* OPAMP1 config */
	OPAMP_InitStructure.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO2;
	OPAMP_InitStructure.OPAMP_InvertingInput =  OPAMP_InvertingInput_IO1;
	OPAMP_Init(OPAMP_Selection_OPAMP1, &OPAMP_InitStructure);

	/* Configure OPAMP1 in PGA mode with gain set to 2 */
//	OPAMP_PGAConfig(OPAMP_Selection_OPAMP1, OPAMP_OPAMP_PGAGain_2, OPAMP_PGAConnect_No);
	
	/* Enable OPAMP1 */
	OPAMP_Cmd(OPAMP_Selection_OPAMP1, ENABLE);
}

void PGA_Init()
{
	PGA_GPIOConfig();
	PGA_OPAMPConfig();
}