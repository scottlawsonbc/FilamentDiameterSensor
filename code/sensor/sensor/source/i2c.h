#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "stm32f30x_exti.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_i2c.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_syscfg.h"
#include "delay.h"

#define I2C_OWN_ADDRESS (0x22)

#define I2C_LONG_TIMEOUT ((uint32_t)(10 * I2C_FLAG_TIMEOUT))  
#define I2C_OK                       ((uint32_t) 0)
#define I2C_FAIL                     ((uint32_t) 0)

#define I2C_RX_PACKET_BUFFER_LENGTH (10U)
#define I2C_TX_PACKET_BUFFER_LENGTH (10U)

/* Low-level pin definitions */
#define I2C                       I2C1
#define I2C_CLK                   RCC_APB1Periph_I2C1
#define I2C_INTERRUPT             I2C1_EV_IRQn

#define I2C_SCK_PIN               GPIO_Pin_6                  /* PB.06 */
#define I2C_SCK_GPIO_PORT         GPIOB                       /* GPIOB */
#define I2C_SCK_GPIO_CLK          RCC_AHBPeriph_GPIOB
#define I2C_SCK_SOURCE            GPIO_PinSource6
#define I2C_SCK_AF                GPIO_AF_4

#define I2C_SDA_PIN               GPIO_Pin_7                  /* PB.7 */
#define I2C_SDA_GPIO_PORT         GPIOB                       /* GPIOB */
#define I2C_SDA_GPIO_CLK          RCC_AHBPeriph_GPIOB
#define I2C_SDA_SOURCE            GPIO_PinSource7
#define I2C_SDA_AF                GPIO_AF_4

#define DRDY_PIN                  GPIO_Pin_2                  /* PE.02 */
#define DRDY_GPIO_PORT            GPIOE                       /* GPIOE */
#define DRDY_GPIO_CLK             RCC_AHBPeriph_GPIOE
#define DRDY_EXTI_LINE            EXTI_Line2
#define DRDY_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOE
#define DRDY_EXTI_PIN_SOURCE      EXTI_PinSource2
#define DRDY_EXTI_IRQn            EXTI2_TS_IRQn 

#define I2C_INT1_PIN              GPIO_Pin_4                  /* PE.04 */
#define I2C_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define I2C_INT1_GPIO_CLK         RCC_AHBPeriph_GPIOE
#define I2C_INT1_EXTI_LINE        EXTI_Line4
#define I2C_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define I2C_INT1_EXTI_PIN_SOURCE  EXTI_PinSource4
#define I2C_INT1_EXTI_IRQn        EXTI4_IRQn 

#define I2C_INT2_PIN              GPIO_Pin_5                  /* PE.05 */
#define I2C_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define I2C_INT2_GPIO_CLK         RCC_AHBPeriph_GPIOE
#define I2C_INT2_EXTI_LINE        EXTI_Line5
#define I2C_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define I2C_INT2_EXTI_PIN_SOURCE  EXTI_PinSource5
#define I2C_INT2_EXTI_IRQn        EXTI9_5_IRQn

extern void I2C_Config(void);
extern uint16_t I2C_Write(uint8_t deviceAddressess, uint8_t registerAddress, uint8_t* dataPointer);
extern uint16_t I2C_Read(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* dataPointer, uint16_t bytesToRead);
extern void I2C_TestWrite(uint8_t reg);
extern uint8_t I2C_TestRead(void);
extern void I2C_CheckReceive();

extern __IO uint8_t I2C_RX_Packets[I2C_RX_PACKET_BUFFER_LENGTH], I2C_RX_Index;
extern __IO uint8_t I2C_TX_Packets[I2C_TX_PACKET_BUFFER_LENGTH], I2C_TX_Index;
extern __IO uint8_t I2C_IgnoreStop;

#endif /* I2C_H */

