#include "delay.h"
#include "stm32f30x_gpio.h"

static __IO uint32_t TimingDelay;

void DelayUs(__IO uint32_t microseconds)
{
	TimingDelay = microseconds;
	while(TimingDelay != 0);
}

void DelayMs(__IO uint32_t milliseconds)
{
	DelayUs(milliseconds * 1000);
}


void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void Delay_Init()
{
	/* Configure SysTick to trigger every microsecond */
	if (SysTick_Config(SystemCoreClock / 1000000))
	{ 
		while (1); /* Capture error */ 
	}
}