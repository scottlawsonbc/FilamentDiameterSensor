#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* Comment out line below to disable debug messages on USART1 */
#define USART1_DEBUG_MESSAGES_ON (1U)

extern void USART1_Init(uint32_t baudRate);
extern void USART1_Send(char *data);
extern void USART1_SendLine(char *data);

extern void uprintf(const char *msg, ... );

#endif