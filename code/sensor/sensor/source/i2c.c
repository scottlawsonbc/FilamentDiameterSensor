#include "i2c.h"

__IO uint32_t  I2C_Timeout = I2C_LONG_TIMEOUT; 

uint32_t I2C_TIMEOUT_UserCallback()
{

}

void I2C_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

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
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
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
  
  if(bytesToRead>1)
	  registerAddress |= 0x80;

  
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

void I2C_TestWrite(uint8_t reg)
{
	uint8_t ctrl1 = 0x00;
	//I2C_Write(0x32, reg, &ctrl1); 
	I2C_Write(0x02, reg, &ctrl1); 
}

uint8_t I2C_TestRead()
{
	uint8_t tmpreg;
	I2C_Read(0x02, 0x02, &tmpreg, 1);
	//I2C_Read(0x3C, 0x09, &tmpreg, 1);
	return tmpreg;
}

///* Configures the GPIO pins used for I2C communication */
//void I2C_GPIO_Config()
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	/* Enable the GPIOB peripheral clock */
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//
//	/* Configure the SDA and SCL pins */
//	GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN | I2C_SCL_PIN;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	/* Configure SCL and SDA as alternate function    */
//	/* GPIO_AF_4 allows I2C1 to control the GPIO pins */
//	GPIO_PinAFConfig(GPIOB, I2C_SCL_PIN_SOURCE, GPIO_AF_4);
//	GPIO_PinAFConfig(GPIOB, I2C_SDA_PIN_SOURCE, GPIO_AF_4);
//}


//void I2C_Config()
//{
//	I2C_InitTypeDef I2C_InitStructure;
//	I2C_GPIO_Config();
//	//I2C_Interrupt_Config();
//	
//	/* Enable the I2C1 peripheral clock */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//	/* Configure the I2C clock source. Derived from HSI */
//	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
//
//	/* Deinitialize any existing I2C1 configuration */
//	I2C_DeInit(I2C1);
//	I2C_Cmd(I2C1, ENABLE);
//
//	/* Configure I2C1 */
//	I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_Timing              = 0xB0420F13; /* 100KHz */
//	I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Disable;
//	I2C_InitStructure.I2C_DigitalFilter       = 0x00;
//	I2C_Init(I2C1, &I2C_InitStructure);
//}
