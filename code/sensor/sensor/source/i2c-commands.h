#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include "main.h"

#define COM_ENABLE_SENSOR                   (0x01U)
#define COM_DISABLE_SENSOR		            (0x02U)
#define COM_REQUEST_MEASUREMENT             (0x03U)
#define COM_AVERAGING_SAMPLES               (0x04U)

extern void COM_ExecuteCommand(__IO uint8_t command, __IO uint8_t* data, uint8_t numberOfBytes);

#endif