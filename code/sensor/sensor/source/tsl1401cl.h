/*
* Module prefix: TSL
* Methods for interfacing with the TSL1401CL linear photodiode array.
*/
#ifndef TSL1401CL_H
#define TSL1401CL_H
#include <stdint.h>
#include <stm32f30x_gpio.h>

/* Sensor parameters */
#define TSL_PIXEL_COUNT             (128U)
#define TSL_PIXEL_SPACING_NM        (63500U)

/* Timing parameters */
#define TSL_CHARGE_TRANSFER_TIME_US (20U)
#define TSL_SENSOR_DELAY_US         (TSL_CHARGE_TRANSFER_TIME_US + 20U)
#define TSL_CLOCK_DELAY_US          (0U)

/* Digital pins connected to GPIOE */
#define TSL_SERIAL_PIN             (GPIO_Pin_11)
#define TSL_CLOCK_PIN              (GPIO_Pin_9)

/* Analog pins connected to GPIOC */
#define TSL_ANALOG_INPUT_PIN       (GPIO_Pin_1)

extern void TSL_Init();
extern void TSL_MeasurePixels(int32_t *pixels);
extern void TSL_MeasurePixelsFloat(float *pixels);


#endif