#ifndef I2C_FILAMENT_H
#define I2C_FILAMENT_H

#include "Configuration.h"
#include <Wire.h>

/* Connect SDA to PA7 of the STM32F3 (you MUST use a 5V-3.3V level converter!) */
/* Connect SCK to PA6 of the STM32F3 (you MUST use a 5V-3.3V level converter!) */
#define FIL_SENSOR_I2C_ADDRESS                   (0x11)
#define FIL_COMMAND_ENABLE_SENSOR                (0x01U)
#define FIL_COMMAND_DISABLE_SENSOR               (0x02U)
#define FIL_COMMAND_REQUEST_MEASUREMENT          (0x03U)
#define FIL_COMMAND_AVERAGING_SAMPLES            (0x04U)
#define FIL_COMMAND_UPDATE_VOLUMETRIC_MULTIPLIER (0x05U)

/* Divisor for FIL_VolumetricMultiplier in terms of bitshifts */
extern const uint16_t FIL_VolumetricMultiplierShifts;
/* Determines how much to compensate for fluctuations in filament diameter */
extern volatile uint32_t FIL_VolumetricMultiplier;
/* Stores the most recent filament diameter measurement */
extern float FIL_MeasuredFilamentDiameter_MM;
/* Position that the volumetric multiplier was last updated */
extern volatile float FIL_LastExtruderPosition_MM;

extern void FIL_Init();
extern void FIL_EnableFilamentSensorI2C(void);
extern void FIL_DisableFilamentSensorI2C(void);
extern void FIL_UpdateVolumetricMultiplierI2C(void);
extern uint16_t FIL_RequestDiameterMeasurementI2C_UM(void);
#endif
