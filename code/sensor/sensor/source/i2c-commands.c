#include "i2c-commands.h"
#include "core_cmInstr.h"
/* Set the number of averaging samples */
void COM_SetAveraging(uint16_t samples)
{
	if (samples > 0)
	{
		MAIN_AveragingIterations = samples;
		uprintf("Averaging samples set to %i\r\n", samples);
	}
	else
	{
		uprintf("Error: invalid averaging samples: %i\r\n", samples);
	}
}

/* Change the enabled state of the sensor */
void COM_SetSensorEnabledState(uint8_t state)
{
	/* !! forces value to 0 or 1 by double inverting */
	MAIN_SetSensorState((FunctionalState)!!state);
	uprintf("Sensor state set to %i\r\n", state);
}

/* Prints the state of each I2C flag to USART1 to assist in debugging I2C communication */
void COM_PrintI2CFlags()
{
	uprintf("Flag: I2C_FLAG_TXE:%i\r\n",         I2C_GetFlagStatus(I2C, I2C_FLAG_TXE));
	uprintf("Flag: I2C_FLAG_TXIS:%i\r\n",        I2C_GetFlagStatus(I2C, I2C_FLAG_TXIS));
	uprintf("Flag: I2C_FLAG_RXNE:%i\r\n",        I2C_GetFlagStatus(I2C, I2C_FLAG_RXNE));
	uprintf("Flag: I2C_FLAG_ADDR:%i\r\n",        I2C_GetFlagStatus(I2C, I2C_FLAG_ADDR));
	uprintf("Flag: I2C_FLAG_NACKF:%i\r\n",       I2C_GetFlagStatus(I2C, I2C_FLAG_NACKF));
	uprintf("Flag: I2C_FLAG_STOPF:%i\r\n",       I2C_GetFlagStatus(I2C, I2C_FLAG_STOPF));
	uprintf("Flag: I2C_FLAG_TC:%i\r\n",          I2C_GetFlagStatus(I2C, I2C_FLAG_TC));
	uprintf("Flag: I2C_FLAG_TCR:%i\r\n",         I2C_GetFlagStatus(I2C, I2C_FLAG_TCR));
	uprintf("Flag: I2C_FLAG_BERR:%i\r\n",        I2C_GetFlagStatus(I2C, I2C_FLAG_BERR));
	uprintf("Flag: I2C_FLAG_ARLO:%i\r\n",        I2C_GetFlagStatus(I2C, I2C_FLAG_ARLO));
	uprintf("Flag: I2C_FLAG_OVR:%i\r\n",         I2C_GetFlagStatus(I2C, I2C_FLAG_OVR));
	uprintf("Flag: I2C_FLAG_PECERR:%i\r\n",      I2C_GetFlagStatus(I2C, I2C_FLAG_PECERR));
	uprintf("Flag: I2C_FLAG_TIMEOUT:%i\r\n",     I2C_GetFlagStatus(I2C, I2C_FLAG_TIMEOUT));
	uprintf("Flag: I2C_FLAG_ALERT:%i\r\n",       I2C_GetFlagStatus(I2C, I2C_FLAG_ALERT));
	uprintf("Flag: I2C_FLAG_BUSY:%i\r\n\r\n",    I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));
}

#define WAIT_FOR(flag, condition) {uint32_t timeout = I2C_LONG_TIMEOUT; while(I2C_GetFlagStatus(I2C, flag) != condition){ if((timeout--) == 0){uprintf("I2C timeout. Flags:\r\n"); COM_PrintI2CFlags(); return;} }}

void COM_SendMeasurement()
{
	uint16_t diameter = (uint16_t)(MAIN_FilamentDiameter_MM * 1000.0);
	uint32_t timeout = I2C_LONG_TIMEOUT;

	/* Wait for ADDR */
	uprintf("Wait for ADDR\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_ADDR, SET);
	I2C_ClearFlag(I2C, I2C_FLAG_ADDR);

	/* Wait for TXIS */
	uprintf("Wait for TXIS\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_TXIS, SET);
	I2C_ClearFlag(I2C, I2C_FLAG_TXIS);

	/* Send first byte */
	uprintf("Send first byte\r\n");
	//COM_PrintI2CFlags();
	I2C_SendData(I2C, (uint8_t)(diameter >> 8));
	uprintf("Wait for TXE\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_TXE, SET);
	//COM_PrintI2CFlags();

	/* Send second byte */
	uprintf("Send second byte\r\n");
	//COM_PrintI2CFlags();
	I2C_SendData(I2C,(uint8_t)diameter);
	uprintf("Wait for TXE\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_TXE, SET);
	//COM_PrintI2CFlags();

	/* Wait for NACKF */
	uprintf("Wait for NACKF\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_NACKF, SET);
	I2C_ClearFlag(I2C, I2C_FLAG_NACKF);

	/* Wait for STOPF */
	uprintf("Wait for STOPF\r\n");
	//COM_PrintI2CFlags();
	WAIT_FOR(I2C_FLAG_STOPF, SET);
	I2C_ClearFlag(I2C, I2C_FLAG_STOPF);

}

/* Executes the specified I2C command */
void COM_ExecuteCommand(__IO uint8_t command, __IO uint8_t* data, uint8_t numberOfBytes)
{
	switch(command)
	{
	case COM_ENABLE_SENSOR:
		COM_SetSensorEnabledState(ENABLE);
		uprintf("Command: [Enable Sensor] received via I2C\r\n");
		break;
	case COM_DISABLE_SENSOR:
		COM_SetSensorEnabledState(DISABLE);
		uprintf("Command: [Disable Sensor] received via I2C\r\n");
		break;
	case COM_REQUEST_MEASUREMENT:
		COM_SendMeasurement();
		//		COM_SendMeasurementIT();
		uprintf("Command: [Request Measurement] received via I2C\r\n");
		break;
	case COM_AVERAGING_SAMPLES:
		COM_SetAveraging(((uint16_t)data[0] << 8) & (uint16_t)data[1]);
		uprintf("Command: [Averaging Samples] received via I2C\r\n");
		break;
	default:
		uprintf("Unrecognized command: [%x] received via I2C\r\n", command);
		break;
	}
}