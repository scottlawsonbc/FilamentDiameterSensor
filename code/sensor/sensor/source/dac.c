#include "tsl1401cl.h"
#include "dac.h"
#include "stm32f30x_dac.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_dma.h"

uint32_t DAC_DataBuffer[TSL_PIXEL_COUNT];

void DAC_SetCH1(uint32_t x[])
{
	uint16_t i;
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		DAC_DataBuffer[i] &= 0x0000FFFF;
		DAC_DataBuffer[i] |= x[i] << 16;
	}
}

void DAC_SetCH2(uint32_t x[])
{
	uint16_t i;
	for (i = 0; i < TSL_PIXEL_COUNT; i++)
	{
		DAC_DataBuffer[i] &= 0xFFFF0000;
		DAC_DataBuffer[i] |= (uint32_t)(uint16_t)x[i];
	}
}

static void DAC_TimerConfig()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* TIM2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = 0xFF;         
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* TIM2 TRGO selection: update event is selected as trigger for DAC */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

static void DAC_DMAConfig()
{
	DMA_InitTypeDef   DMA_InitStructure;

	/* Enable DMA2 clock -------------------------------------------------------*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	DMA_DeInit(DMA2_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)&DAC_DataBuffer;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize         = TSL_PIXEL_COUNT;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	/* Enable DMA2 Channel3 */
	DMA_Cmd(DMA2_Channel3, ENABLE);
}

static void DAC_DACConfig()
{
	DAC_InitTypeDef   DAC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable GPIOA Periph clock --------------------------------------*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure PA.04 (DAC1_OUT1), PA.05 (DAC1_OUT2) as analog */
	GPIO_InitStructure.GPIO_Pin  =  DAC_OUTPUT_PIN_0 | DAC_OUTPUT_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* Fill DAC InitStructure */
	DAC_InitStructure.DAC_Trigger                      = DAC_Trigger_T2_TRGO;
	DAC_InitStructure.DAC_WaveGeneration               = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits2_0;  
	DAC_InitStructure.DAC_OutputBuffer                 = DAC_OutputBuffer_Disable;

	/* DAC channel1 Configuration */
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* DAC channel2 Configuration */
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
	automatically connected to the DAC converter. */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	/* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
	automatically connected to the DAC converter. */
	DAC_Cmd(DAC_Channel_2, ENABLE);

	/* Enable DMA for DAC Channel1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void DAC_Setup()
{
	DAC_DMAConfig();
	DAC_DACConfig();
	DAC_TimerConfig();
}