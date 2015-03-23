#include "i2c-commands.h"

/* Set the number of averaging samples */
void COM_SetAveraging(uint16_t samples)
{
	if (samples > 0)
	{
		MAIN_AveragingIterations = samples;
	}
}

/* Change the enabled state of the sensor */
void COM_SetSensorEnabledState(uint8_t state)
{
	/* !! forces value to 0 or 1 by double inverting */
	MAIN_SetSensorState((FunctionalState)!!state);
}

void COM_PrintI2CFlags()
{
	uprintf("Flag: I2C_FLAG_TXE:%i\r\n",     I2C_GetFlagStatus(I2C, I2C_FLAG_TXE));
	uprintf("Flag: I2C_FLAG_TXIS:%i\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_TXIS));
	uprintf("Flag: I2C_FLAG_RXNE:%i\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_RXNE));
	uprintf("Flag: I2C_FLAG_ADDR:%i\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_ADDR));
	uprintf("Flag: I2C_FLAG_NACKF:%i\r\n",   I2C_GetFlagStatus(I2C, I2C_FLAG_NACKF));
	uprintf("Flag: I2C_FLAG_STOPF:%i\r\n",   I2C_GetFlagStatus(I2C, I2C_FLAG_STOPF));
	uprintf("Flag: I2C_FLAG_TC:%i\r\n",      I2C_GetFlagStatus(I2C, I2C_FLAG_TC));
	uprintf("Flag: I2C_FLAG_TCR:%i\r\n",     I2C_GetFlagStatus(I2C, I2C_FLAG_TCR));
	uprintf("Flag: I2C_FLAG_BERR:%i\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_BERR));
	uprintf("Flag: I2C_FLAG_ARLO:%i\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_ARLO));
	uprintf("Flag: I2C_FLAG_OVR:%i\r\n",     I2C_GetFlagStatus(I2C, I2C_FLAG_OVR));
	uprintf("Flag: I2C_FLAG_PECERR:%i\r\n",  I2C_GetFlagStatus(I2C, I2C_FLAG_PECERR));
	uprintf("Flag: I2C_FLAG_TIMEOUT:%i\r\n", I2C_GetFlagStatus(I2C, I2C_FLAG_TIMEOUT));
	uprintf("Flag: I2C_FLAG_ALERT:%i\r\n",   I2C_GetFlagStatus(I2C, I2C_FLAG_ALERT));
	uprintf("Flag: I2C_FLAG_BUSY:%i\r\n\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));
}

/* Send the Master the latest filament diameter measurement */
void COM_SendMeasurement()
{
	uint16_t diameter = (uint16_t)(MAIN_FilamentDiameter_MM * 1000.0);

	/* Clear the ADDR flag */
	uint16_t timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_ADDR) == RESET)
	{
		if((timeout--) == 0) return;
	}
	I2C_ClearFlag(I2C, I2C_FLAG_ADDR);
	COM_PrintI2CFlags();

	I2C_SendData(I2C, (uint8_t)((diameter & 0xFF00) >> 8));
	COM_PrintI2CFlags();
	timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_TXE) == RESET)
	{
		if((timeout--) == 0) return;
	}
	I2C_SendData(I2C, (uint8_t)(diameter & 0x00FF));

	/* Wait for not busy */
	timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_STOPF) == RESET)
	{
		if((timeout--) == 0) return;
	}
	I2C_ClearFlag(I2C, I2C_FLAG_STOPF);

	COM_PrintI2CFlags();

	uprintf("Sent data!\r\n");

}

void COM_ExecuteCommand(uint8_t command, uint8_t* data, uint8_t numberOfBytes)
{
	switch(command)
	{
	case COM_ENABLE_SENSOR:
		uprintf("Enabling sensor!\r\n");
		COM_SetSensorEnabledState(ENABLE);
		break;
	case COM_DISABLE_SENSOR:
		uprintf("Disabling sensor!\r\n");
		COM_SetSensorEnabledState(DISABLE);
		break;
	case COM_REQUEST_MEASUREMENT:
		uprintf("Measurement requested!\r\n");
		COM_SendMeasurement();
		break;
	default: 
		break;
	}
}