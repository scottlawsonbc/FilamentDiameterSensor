#include "i2c_filament_sensor.h"

/* Initializes the Wire library for I2C communication */
void FIL_Init()
{
    Wire.begin();
}

/* Instructs the filament sensor to turn on and start taking measurements */
void FIL_EnableFilamentSensorI2C(void)
{
    Wire.beginTransmission(FIL_SENSOR_I2C_ADDRESS);
    Wire.write(FIL_COMMAND_ENABLE_SENSOR);
    Wire.endTransmission();
}

/* Instructs the filament sensor to turn off and stop taking measurements */
void FIL_DisableFilamentSensorI2C(void)
{
    Wire.beginTransmission(FIL_SENSOR_I2C_ADDRESS);
    Wire.write(FIL_COMMAND_DISABLE_SENSOR);
    Wire.endTransmission();
}

/* Requests a filament diameter measurement (expressed in microns) from the sensor */
uint16_t FIL_RequestDiameterMeasurementI2C_UM(void)
{
    /* Request a filament diameter measurement from the STM32F3 */
    Wire.beginTransmission(FIL_SENSOR_I2C_ADDRESS);   /* Transmit start condition */
    Wire.write(FIL_COMMAND_REQUEST_MEASUREMENT); /* Transmit command byte    */
    Wire.endTransmission();                      /* Transmit stop condition  */

    /* Request two bytes of data from STM32F3 */
    Wire.requestFrom(FIL_SENSOR_I2C_ADDRESS, 2);
    uint16_t diameter_UM = 0;
    diameter_UM |= ((uint16_t)Wire.read()) << 8; /* Upper bits are transmitted first    */
    diameter_UM |= ((uint16_t)Wire.read()) << 0; /* Lower bits are transmitted second   */
    return diameter_UM;                          /* Return filament diameter in microns */
}