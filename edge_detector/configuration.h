/*
 * Module prefix: CFG
 *
 * The configuration module provides define macros for settings and various compile-time parameters used by the other modules
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

/* ADC settings >>> */
// Various ADC prescaler values
#define CFG_ADC_PS_16  (1 << ADPS2)
#define CFG_ADC_PS_32  ((1 << ADPS2) | (1 << ADPS0))
#define CFG_ADC_PS_64  ((1 << ADPS2) | (1 << ADPS1))
#define CFG_ADC_PS_128 ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define CFG_ADC_PRESCALER CFG_ADC_PS_128
/* <<< */

/* Interpolation settings >>> */
#define CFG_MOVING_AVERAGE_SHIFTS    (4U)  // Base 2 logarithm of the window size used for the averaging filter
/* <<< */

/* Linear sensor array settings >>> */
#define CFG_PIXEL_COUNT            (128U)  // Number of pixels in the sensor array
#define CFG_PIXEL_SPACING_NM     (63500U)  // Center-center spacing (in nanometers) between pixels in the sensor array
#define CFG_LOGIC_SETUP_CLOCKS      (18U)  // Number of setup logic clock pulses
#define CFG_ANALOG_INPUT_PIN         (0U)  // Analog pin connected to the analog output of the sensor array
#define CFG_SERIAL_PIN              (11U)  // Digital pin that delivers the serial SI pulse to the sensor
#define CFG_CLOCK_PIN               (10U)  // Clock pulse pin connected to CLK pin of the sensor
#define CFG_STABILIZATION_TIME_US   (0U)  // Amount of time to allow for the pixel data to stabilize

#define CFG_EXTERNAL_ADC (1U)
#if CFG_EXTERNAL_ADC
#define CFG_EXTERNAL_ADC_DDR       (DDRD)  // DMA data direction register for external ADC
#define CFG_EXTERNAL_ADC_DR        (PORTD) // DMA data register for external ADC
#define CFG_EXTERNAL_ADC_IDR       (PIND)  // DMA input data register for external ADC

#define CFG_EXTERNAL_ADC_NOPS       (9U)  // Number of NOP instructions before next rising/falling edge
#define CFG_EXTERNAL_ADC_WRITE_PIN  (12U)  // Pin used to signal a conversion on the external ADC  
#endif
/* <<< */

/* Edge detection settings >>> */
#define CFG_BOX_FILTER_SHIFTS        (2U)  // Base 2 logarithm of the window size used for the box filter
#define CFG_GAUSSIAN_ITERATIONS      (3U)  // 3 iterations ≈ 3% error
/* <<< */

/* Debugging settings >>> */
#define CFG_SERIAL_DEBUGGING             (0U) // Enable or disable serial debugging. Serial debugging is very slow.
#if CFG_SERIAL_DEBUGGING
	#define PRINT(x) Serial.print(x)
	#define PRINT_VAL(x, y) Serial.print(x); Serial.println(y)
	#define PRINT_ARRAY(x) for(uint16_t i=0; i<CFG_PIXEL_COUNT; i++){PRINT(x[i]);PRINT(",");}PRINT("\r\n")
#else
	#define PRINT(x)
	#define PRINT_VAL(x, y)
	#define PRINT_ARRAY(x)
#endif
/* <<< */

#endif