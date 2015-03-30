#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <math.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_adc.h>
#include "stm32f30x_dac.h"
#include "dac.h"
#include "usart.h"
#include "delay.h"
#include "tsl1401cl.h"
#include "edge-detection.h"
#include "led-bar.h"
#include "opamp.h"
#include "geometry.h"
#include "i2c.h"
#include "buffer.h"

#define USART1_BAUDRATE (115200U)
#define MAIN_DEFAULT_AVERAGING (100U)

/* IIR filter reduces sudden large changes in measured filament diameter */
#define MAIN_IIR_FILTER_SAMPLES (40U)
#define MAIN_IIR_FILTER_INPUT_WEIGHT (1U)

/* The number of measurements to average before computing the diameter */
extern uint16_t MAIN_AveragingIterations;

/* The functional state of the sensor */
extern FunctionalState MAIN_SensorState;

/* The most recent valid filament diameter measurement */
extern float MAIN_FilamentDiameter_MM;

/* The expected filament diameter */
extern float MAIN_NominalFilamentDiameter_MM;

/* The filament length between the sensor and extruder (equal to buffer size) */
extern const uint16_t MAIN_FilamentBufferDistance_MM;

/* Turn the sensor on or off */
extern void MAIN_SetSensorState(FunctionalState state);

#endif