#ifndef USART_H
#define USART

#include <stdint.h>

extern void USART1_Init(uint32_t baudRate);
extern void USART1_Send(char *data);
extern void USART1_SendLine(char *data);

#endif