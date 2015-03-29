#include "usart.h"
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_usart.h>

/* Transmits the given text */
void USART1_Send(char *data)
{
	while (*data != '\0')
	{
		if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != RESET)
		{
			USART_SendData(USART1, *data); /* Transmit character */
			data++;						   /* Increment to next character */
		}
	}
}

/* Transmits given text followed by a new line terminator */
void USART1_SendLine(char *data)
{
	USART1_Send(data);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\r'); /* Transmit return character */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\n'); /* Transmit newline character */
}	

void USART1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); /* USART1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);    /* GPIOA clock enable */
}

void USART1_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIO Configuration */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_7);  // USART1_TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);  // USART1_RX
}

void USART1_Init(uint32_t baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	USART1_RCC_Configuration();
	USART1_GPIO_Configuration();

	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

void uprintf(const char *msg, ... )
{
	#if USART1_DEBUG_MESSAGES_ON

	va_list argList;
	char buffer[150];
	va_start(argList, msg);
	vsprintf(buffer, msg, argList);
	va_end(argList);
	USART1_Send(buffer);

	#endif
}

void cprintf(const char *msg, ... )
{
	va_list argList;
	char buffer[150];
	va_start(argList, msg);
	vsprintf(buffer, msg, argList);
	va_end(argList);
	USART1_Send(buffer);
}