#ifndef I2C_FILAMENT_H
#define I2C_FILAMENT_H
#ifdef I2C_FILAMENT_SENSOR

#include "Configuration.h"

/* Used for cross sectional area ratio comparisons */
const uint32_t FIL_ExpectedDiameterSquared_UM = (I2C_FILAMENT_SENSOR_EXPECTED_DIAMETER_UM * I2C_FILAMENT_SENSOR_EXPECTED_DIAMETER_UM);

/* Stores a buffer of flow compensation values to account for distance between sensor and nozzle */
volatile int8_t FIL_FlowCompensationBuffer[I2C_FILAMENT_SENSOR_TO_NOZZLE_MM+1];

/* Request a filament diameter measurement from the I2C filament sensor in micrometers */
extern uint16_t FIL_RequestDiameter_UM(void);
/* Reset the values in the area ratio buffer to 0 */
extern void FIL_ResetAreaRatioBuffer(void);
/* Determine the percentage flow adjustment that should be made for a given filament diameter (in microns) */
extern int8_t FIL_DetermineFlowCompensation(uint16_t filamentDiameter_UM);

/* Returns the current flow compensation percentage value and increments the buffer */
extern int8_t FIL_PopFlowCompensation(void);

/* Pushes a 
extern void FIL_PushFlowCompensation(int8_t flowPercentage);


#endif /* I2C_FILAMENT_SENSOR */
#endif /* I2C_FILAMENT_H */
