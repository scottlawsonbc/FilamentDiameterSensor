#include "i2c_filament_sensor.h"

const uint16_t FIL_VolumetricMultiplierShifts = 14;
volatile uint16_t FIL_VolumetricMultiplier;
volatile float FIL_LastExtruderPosition_MM;
float FIL_MeasuredFilamentDiameter_MM;

/* Initializes the Wire library for I2C communication */
void FIL_Init()
{
    FIL_VolumetricMultiplier = 1 << FIL_VolumetricMultiplierShifts;
    FIL_MeasuredFilamentDiameter_MM = 0.0f;
    FIL_LastExtruderPosition_MM = 0.0f;
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

/* Requests a new volumetric multiplier via I2C                              */
/* This should be called whenever the extruder position is increased by 1 mm */
void FIL_UpdateVolumetricMultiplierI2C(void)
{
    /* Request a filament diameter measurement from the STM32F3 */
    Wire.beginTransmission(FIL_SENSOR_I2C_ADDRESS);       /* Transmit start condition */
    Wire.write(FIL_COMMAND_UPDATE_VOLUMETRIC_MULTIPLIER); /* Transmit command byte    */
    Wire.endTransmission();                               /* Transmit stop condition  */

    /* Request two bytes of data from STM32F3 */
    /* This is the new volumetric multiplier  */
    Wire.requestFrom(FIL_SENSOR_I2C_ADDRESS, 2);
    uint16_t multiplier = 0;
    multiplier |= ((uint16_t)Wire.read()) << 8; /* Upper bits are transmitted first    */
    multiplier |= ((uint16_t)Wire.read()) << 0; /* Lower bits are transmitted second   */
    FIL_VolumetricMultiplier = multiplier;
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