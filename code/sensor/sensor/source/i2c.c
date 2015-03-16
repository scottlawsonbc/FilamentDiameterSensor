#include "i2c.h"

/* Configures the GPIO pins used for I2C communication */
void I2C_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the GPIOB peripheral clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* Configure the SDA and SCL pins */
	GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN | I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure SCL and SDA as alternate function    */
	/* GPIO_AF_4 allows I2C1 to control the GPIO pins */
	GPIO_PinAFConfig(GPIOB, I2C_SCL_PIN_SOURCE, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOB, I2C_SDA_PIN_SOURCE, GPIO_AF_4);
}

/* Configures the I2C interrupt in the nested vector interrupt controller (NVIC) */
void I2C_Interrupt_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the I2C event priority */
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void I2C_Config()
{
	I2C_InitTypeDef I2C_InitStructure;
	I2C_GPIO_Config();
	//I2C_Interrupt_Config();
	
	/* Enable the I2C1 peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* Configure the I2C clock source. Derived from HSI */
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	/* Deinitialize any existing I2C1 configuration */
	I2C_DeInit(I2C1);
	I2C_Cmd(I2C1, ENABLE);

	/* Configure I2C1 */
	I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
	I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
	I2C_InitStructure.I2C_Timing              = 0xB0420F13; /* 100KHz */
	I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Disable;
	I2C_InitStructure.I2C_DigitalFilter       = 0x00;
	I2C_Init(I2C1, &I2C_InitStructure);
}

uint8_t I2C_Write(uint8_t address, uint8_t reg, uint8_t data)
{
	/* Set R/W bit to 0 */
	I2C_GenerateSTART(I2C1, ENABLE);
	DelayMs(2);

	I2C_SendData(I2C1, reg);
	DelayMs(2);

	I2C_GenerateSTOP(I2C1, ENABLE);
	return 0;
}

//
//void SCL(BitAction state)
//{
//	GPIO_WriteBit(GPIOB, I2C_SCL_PIN, state);
//	DelayUs(10);
//}
//
//void SDA(BitAction state)
//{
//	GPIO_WriteBit(GPIOB, I2C_SDA_PIN, state);
//	DelayUs(10);
//}
//
//uint8_t SCL_Read()
//{
//	return GPIO_ReadInputDataBit(GPIOB, I2C_SCL_PIN);
//}
//
//uint8_t SDA_Read()
//{
//	return GPIO_ReadInputDataBit(GPIOB, I2C_SDA_PIN);
//}
//
//void I2C_BitBang_Start()
//{
//	SDA(Bit_SET);
//	SCL(Bit_SET);
//	SDA(Bit_RESET);
//	SDA(Bit_RESET);
//}
//
//void I2C_BitBang_Stop()
//{
//	SDA(Bit_RESET);
//	SCL(Bit_SET);
//	SDA(Bit_SET);
//}
//
//void I2C_BitBang_TX(uint8_t data)
//{
//	int8_t x;
//	for(x = 8; x; x--)
//	{
//		if (data & 0x80)
//		{
//			SDA(Bit_SET);
//		}
//		else
//		{
//			SDA(Bit_SET);
//		}
//		SCL(Bit_SET);
//		data <<= 1;
//		SCL(Bit_RESET);
//	}
//	SDA(Bit_SET);
//	SCL(Bit_SET);
//	DelayUs(10);
//	SCL(Bit_RESET);
//}
//
//int8_t I2C_BitBang_RX(uint8_t ack)
//{
//	int8_t x, data=0;
//	SDA(Bit_SET);
//	for (x = 0; x < 8; x++)
//	{
//		data <<= 1;
//		do
//		{
//			SCL(Bit_SET);
//		} 
//		while(SCL_Read() == 0);
//		DelayUs(10);
//		
//		data |= SDA_Read();
//		SCL(Bit_SET);
//		DelayUs(10);
//	}
//	return data;
//}
//
//void I2C_BitBang_Write(uint8_t address, uint8_t register_address, uint8_t data)
//{
//	I2C_BitBang_Start();
//	I2C_BitBang_TX(address);
//	I2C_BitBang_TX(register_address);
//	I2C_BitBang_TX(data);
//	I2C_BitBang_Stop();
//}