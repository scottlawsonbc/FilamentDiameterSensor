#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "edge-detection.h"
#include <stdint.h>

#define GEO_PRESET_80_MM_SENSOR 1	/* 80mm sensor (first 2-axis prototype) */
#define GEO_PRESET_130_MM_SENSOR 2	/* 130mm sensor (printer-adapted 2-axis prototype) */
#define GEO_PRESET_CUSTOM_SENSOR 0	/* Custom sensor geometry (define below) */

/* Define sensor type for preset geometry constants */
#define GEO_CONSTANTS_PRESET GEO_PRESET_130_MM_SENSOR
/* Geometry constants */
#if GEO_CONSTANTS_PRESET == GEO_PRESET_80_MM_SENSOR
		#define GEO_A_LED_X_MM 86.5f
		#define GEO_A_LED_Y_MM 17.75f
		#define GEO_B_LED_X_MM 17.75f
		#define GEO_B_LED_Y_MM 86.5f
		#define GEO_A_SENSOR_OFFSET_MM 13.f
		#define GEO_B_SENSOR_OFFSET_MM 22.5f
#else
#if GEO_CONSTANTS_PRESET == GEO_PRESET_130_MM_SENSOR
		#define GEO_A_LED_X_MM 130.f
		#define GEO_A_LED_Y_MM 17.75f
		#define GEO_B_LED_X_MM 17.75f
		#define GEO_B_LED_Y_MM 130.f
		#define GEO_A_SENSOR_OFFSET_MM 13.f
		#define GEO_B_SENSOR_OFFSET_MM 22.5f
#else // All other values default to custom geometry
		#define GEO_A_LED_X_MM 80.0f
		#define GEO_A_LED_Y_MM 15.0f
		#define GEO_B_LED_X_MM 15.0f
		#define GEO_B_LED_Y_MM 80.0f
		#define GEO_A_SENSOR_OFFSET_MM 13.f
		#define GEO_B_SENSOR_OFFSET_MM 22.5f
#endif
#endif

/* Filament diameter calculations for 2-axis sensor */
extern float GEO_Filament_Diameter_MM(EdgeData edge_position_a, EdgeData edge_position_b);

#endif