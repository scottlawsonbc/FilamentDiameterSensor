/*
 * Module prefix: CFG
 *
 * The configuration module provides define macros for settings and various compile-time parameters used by the other modules
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

/* Interpolation settings >>> */
#define CFG_MOVING_AVERAGE_SHIFTS    (7U)  // Base 2 logarithm of the window size used for the averaging filter
/* <<< */

/* Linear sensor array settings >>> */
#define CFG_PIXEL_COUNT            (128U)  // Number of pixels in the sensor array
#define CFG_PIXEL_SPACING_NM     (63500U)  // Center-center spacing (in nanometers) between pixels in the sensor array
#define CFG_LOGIC_SETUP_CLOCKS      (18U)  // Number of setup logic clock pulses
#define CFG_ANALOG_INPUT_PIN         (0U)  // Analog pin connected to the analog output of the sensor array
#define CFG_SERIAL_PIN              (11U)  // Digital pin that delivers the serial SI pulse to the sensor
#define CFG_CLOCK_PIN               (10U)  // Clock pulse pin connected to CLK pin of the sensor
#define CFG_STABILIZATION_TIME_US   (5U)  // Amount of time to allow for the pixel data to stabilize
/* <<< */

/* Edge detection settings >>> */
#define CFG_BOX_FILTER_SHIFTS        (2U)  // Base 2 logarithm of the window size used for the box filter
#define CFG_GAUSSIAN_ITERATIONS      (3U)  // 3 iterations ≈ 3% error
/* <<< */

/* Debugging settings >>> */
#define SERIAL_DEBUGGING             (0U) // Enable or disable serial debugging. Serial debugging is very slow.
#if SERIAL_DEBUGGING
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