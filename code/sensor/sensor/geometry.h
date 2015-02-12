#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>

/* Geometry constants */
#define GEO_A_LED_X_MM 150.0
#define GEO_A_LED_Y_MM 30.0
#define GEO_B_LED_X_MM 30.0
#define GEO_B_LED_Y_MM 150.0
#define GEO_A_SENSOR_OFFSET_MM 10.0
#define GEO_B_SENSOR_OFFSET_MM 10.0

/* Filament diamter calculations for 2-axis sensor */
extern float GEO_filament_diameter(float edge_position_a[], float edge_position_b[]);

#endif