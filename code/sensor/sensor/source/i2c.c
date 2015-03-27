#include "i2c.h"
#include "usart.h"
#include "i2c-commands.h"

__IO uint32_t I2C_Timeout = I2C_LONG_TIMEOUT; 
volatile __IO uint8_t I2C_RX_Packets[I2C_RX_PACKET_BUFFER_LENGTH], I2C_RX_Index;
volatile __IO uint8_t I2C_TX_Packets[I2C_TX_PACKET_BUFFER_LENGTH], I2C_TX_Index;
volatile __IO uint8_t I2C_IgnoreStop;

void I2C_PrintInterruptStatus()
{
	uprintf("I2C_IT_ERRI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_ERRI));
	uprintf("I2C_IT_TCI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_TCI));
	uprintf("I2C_IT_STOPI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_STOPI));
	uprintf("I2C_IT_NACKI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_NACKI));
	uprintf("I2C_IT_ADDRI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_ADDRI));
	uprintf("I2C_IT_RXI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_RXI));
	uprintf("I2C_IT_TXI %i\r\n", I2C_GetITStatus(I2C, I2C_IT_TXI));
}

void I2C_ProcessCommand(__IO uint8_t* receivedBytes, __IO uint8_t numberOfBytes)
{
	uint8_t i;
	uprintf("Command received!\r\n");
	for (i = 0; i < numberOfBytes; i++)
	{
		uprintf("Received byte: %i\r\n", receivedBytes[i]);
	}
	COM_ExecuteCommand(receivedBytes[0], &receivedBytes[1], numberOfBytes-1);
}

void I2C1_EV_IRQHandler(void)
{
	/* Address matched */
	if (I2C_GetITStatus(I2C, I2C_IT_ADDRI))
	{
		uprintf("ADDRI\r\n");
		I2C_ClearITPendingBit(I2C, I2C_IT_ADDRI);
	}

	/* Received byte */
	if (I2C_GetITStatus(I2C, I2C_IT_RXI))
	{
		I2C_RX_Packets[I2C_RX_Index] = I2C_ReceiveData(I2C);
		uprintf("RXI: %i\r\n", I2C_RX_Packets[I2C_RX_Index]);
		if (I2C_RX_Index < I2C_RX_PACKET_BUFFER_LENGTH-1) 
		{
			I2C_RX_Index++;
		}
		I2C_ClearITPendingBit(I2C, I2C_IT_RXI);
	}

	/* Stop condition detected */
	if (I2C_GetITStatus(I2C, I2C_IT_STOPI))
	{
		uprintf("STOPI\r\n");
		I2C_ClearITPendingBit(I2C, I2C_IT_STOPI);
		I2C_ProcessCommand(I2C_RX_Packets, I2C_RX_Index+1);
		I2C_RX_Index = 0;
	}

	///* Transmit interrupt status */
	//if (I2C_GetITStatus(I2C, I2C_IT_TXI))
	//{
	//	if (I2C_TX_Index > 0)
	//	{
	//		I2C_SendData(I2C, I2C_TX_Packets[I2C_TX_Index]);
	//		I2C_TX_Index--;
	//		uprintf("Transmitted packet\r\n");
	//		I2C_ClearITPendingBit(I2C, I2C_IT_TXI);
	//	}
	//}
	//I2C_PrintInterruptStatus();
}

uint32_t I2C_TIMEOUT_UserCallback()
{
}

void I2C_CheckReceive()
{
	if (I2C_GetFlagStatus(I2C, I2C_FLAG_ADDR))
	{
		uprintf("\r\nAddress matched!\r\n");
		I2C_ClearFlag(I2C, I2C_FLAG_ADDR);
	}

	/* Receive byte */
	if (I2C_GetFlagStatus(I2C, I2C_FLAG_RXNE))
	{
		uprintf("Packet received!\r\n");

		I2C_RX_Packets[I2C_RX_Index] = I2C_ReceiveData(I2C);
		if (I2C_RX_Index < I2C_RX_PACKET_BUFFER_LENGTH-1) 
		{
			I2C_RX_Index++;
		}
		I2C_ClearFlag(I2C, I2C_FLAG_RXNE);
	}

	/* Process command */
	if (I2C_GetFlagStatus(I2C, I2C_FLAG_STOPF))
	{
		uprintf("Stop! received!\r\n");
		I2C_ClearFlag(I2C, I2C_FLAG_STOPF);
		I2C_ProcessCommand(I2C_RX_Packets, I2C_RX_Index+1);
		I2C_RX_Index = 0;
	}
}

void I2C_InterruptConfig()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the I2C event priority */
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C_INTERRUPT;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the interrupt */
	//uint32_t interruptFlags = I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI | I2C_IT_STOPI | I2C_IT_TCI | I2C_IT_ERRI;
	uint32_t interruptFlags = I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_STOPI;
	I2C_ITConfig(I2C, interruptFlags, ENABLE);
}

void I2C_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	/* Reset TX, RX, stop */
	I2C_RX_Index = 0;
	I2C_TX_Index = 0;
	I2C_IgnoreStop = 0;

	/* Enable the I2C periph */
	RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);

	/* Enable SCK and SDA GPIO clocks */
	RCC_AHBPeriphClockCmd(I2C_SCK_GPIO_CLK | I2C_SDA_GPIO_CLK , ENABLE);

	/* Enable INT1 GPIO clock */
	RCC_AHBPeriphClockCmd(I2C_INT1_GPIO_CLK, ENABLE);

	/* Enable INT2 GPIO clock */
	RCC_AHBPeriphClockCmd(I2C_INT2_GPIO_CLK, ENABLE);

	/* Enable DRDY clock */
	RCC_AHBPeriphClockCmd(DRDY_GPIO_CLK, ENABLE);

	GPIO_PinAFConfig(I2C_SCK_GPIO_PORT, I2C_SCK_SOURCE, I2C_SCK_AF);
	GPIO_PinAFConfig(I2C_SDA_GPIO_PORT, I2C_SDA_SOURCE, I2C_SDA_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* I2C SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = I2C_SCK_PIN;
	GPIO_Init(I2C_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* I2C SDA pin configuration */
	GPIO_InitStructure.GPIO_Pin =  I2C_SDA_PIN;
	GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

	/* Mems DRDY */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* Mems DRDY pin configuration */
	GPIO_InitStructure.GPIO_Pin = DRDY_PIN;
	GPIO_Init(DRDY_GPIO_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to Mems DRDY Pin */
	SYSCFG_EXTILineConfig(DRDY_EXTI_PORT_SOURCE, DRDY_EXTI_PIN_SOURCE);

	EXTI_InitStructure.EXTI_Line = DRDY_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	EXTI_Init(&EXTI_InitStructure);

	/* I2C configuration -------------------------------------------------------*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = I2C_OWN_ADDRESS;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing = 0x00902025;

	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C, &I2C_InitStructure);

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C, ENABLE);

	/* Configure GPIO PINs to detect Interrupts */
	GPIO_InitStructure.GPIO_Pin = I2C_INT1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(I2C_INT1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = I2C_INT2_PIN;
	GPIO_Init(I2C_INT2_GPIO_PORT, &GPIO_InitStructure);

	I2C_InterruptConfig();
}  

uint16_t I2C_Write(uint8_t deviceAddressess, uint8_t registerAddress, uint8_t* dataPointer)
{
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_BUSY) != RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C, deviceAddressess, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;  
	while(I2C_GetFlagStatus(I2C, I2C_ISR_TXIS) == RESET)   
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	/* Send Register address */
	I2C_SendData(I2C, (uint8_t) registerAddress);

	/* Wait until TCR flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_TCR) == RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C, deviceAddressess, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);

	/* Wait until TXIS flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_TXIS) == RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}  

	/* Write data to TXDR */
	I2C_SendData(I2C, *dataPointer);

	/* Wait until STOPF flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_STOPF) == RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}   

	/* Clear STOPF flag */
	I2C_ClearFlag(I2C, I2C_ICR_STOPCF);

	return I2C_OK;
}

uint16_t I2C_Read(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* dataPointer, uint16_t bytesToRead)
{    
	/* Test on BUSY Flag */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_BUSY) != RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C, deviceAddress, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_TXIS) == RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	if(bytesToRead>1) registerAddress |= 0x80;

	/* Send Register address */
	I2C_SendData(I2C, (uint8_t)registerAddress);

	/* Wait until TC flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_TC) == RESET)
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}  

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C, deviceAddress, bytesToRead, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	/* Wait until all data are received */
	while (bytesToRead)
	{   
		/* Wait until RXNE flag is set */
		I2C_Timeout = I2C_LONG_TIMEOUT;
		while(I2C_GetFlagStatus(I2C, I2C_ISR_RXNE) == RESET)    
		{
			if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
		}

		/* Read data from RXDR */
		*dataPointer = I2C_ReceiveData(I2C);
		/* Point to the next location where the byte read will be saved */
		dataPointer++;

		/* Decrement the read bytes counter */
		bytesToRead--;
	} 

	/* Wait until STOPF flag is set */
	I2C_Timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_ISR_STOPF) == RESET)   
	{
		if((I2C_Timeout--) == 0) return I2C_TIMEOUT_UserCallback();
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(I2C, I2C_ICR_STOPCF);

	/* If all operations OK */
	return I2C_OK;  
}  