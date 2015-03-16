#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_i2c.h"
#include "stm32f30x_misc.h"
#include "delay.h"

#define I2C_OWN_ADDRESS (0x30)

/* All pins connected to GPIOB */
#define I2C_SDA_PIN        (GPIO_Pin_7)
#define I2C_SDA_PIN_SOURCE (GPIO_PinSource7)

#define I2C_SCL_PIN        (GPIO_Pin_6)
#define I2C_SCL_PIN_SOURCE (GPIO_PinSource6)

extern void I2C_Config(void);
extern uint8_t I2C_Write(uint8_t address, uint8_t reg, uint8_t data);
extern void I2C_BitBang_Write(uint8_t address, uint8_t register_address, uint8_t data);

#endif /* I2C_H */

