#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "edge-detection.h"
#include <stdint.h>

/* Geometry constants */
#define GEO_A_LED_X_MM 86.5f
#define GEO_A_LED_Y_MM 17.75f
#define GEO_B_LED_X_MM 17.75f
#define GEO_B_LED_Y_MM 86.5f
#define GEO_A_SENSOR_OFFSET_MM 13.f
#define GEO_B_SENSOR_OFFSET_MM 22.5f

/* Filament diameter calculations for 2-axis sensor */
extern float GEO_Filament_Diameter_MM(EdgeData edge_position_a, EdgeData edge_position_b);

#endif