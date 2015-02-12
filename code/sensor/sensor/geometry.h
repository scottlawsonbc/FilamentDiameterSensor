#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>

/* Geometry constants */
#define GEO_NM_PER_MM (1000000U)
#define GEO_A_LED_X_MM 150.0
#define GEO_A_LED_Y_MM 30.0
#define GEO_B_LED_X_MM 30.0
#define GEO_B_LED_Y_MM 150.0

/* Filament diamter calculations for 2-axis sensor */
extern float GEO_filament_diameter(int32_t shad_ind_a[], int32_t shad_ind_b[])

#endif