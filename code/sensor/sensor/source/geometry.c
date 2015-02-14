#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "edge-detection.h"
#include "geometry.h"

/* Adds offset based on TSL1401 sensor placements in 2-axis sensor
*/
void GEO_Add_Sensor_Offsets(EdgeData* edge_position_a, EdgeData* edge_position_b)
{
	edge_position_a->E0 += GEO_A_SENSOR_OFFSET_MM;
	edge_position_a->E1 += GEO_A_SENSOR_OFFSET_MM;
	edge_position_b->E0 += GEO_B_SENSOR_OFFSET_MM;
	edge_position_b->E1 += GEO_B_SENSOR_OFFSET_MM;
}

/* Returns an averaged diameter based on the two perpedicular widths
*/
inline float GEO_Averaged_Diameter(float diameter_A, float diameter_B)
{
	//return sqrt(diameter_A * diameter_B); /* geometric mean */
	return (diameter_A + diameter_B)/2.0f; /* arithmetic mean */
}

///* Returns the filament diameter
//*/
//float GEO_Filament_Diameter(float edge_position_a[], float edge_position_b[])
//{
//	GEO_Add_Sensor_Offsets(edge_position_a, edge_position_b);
//
//	float x_a1 = edge_position_a[1] - GEO_A_LED_Y_MM;
//	float x_a0 = edge_position_a[0] - GEO_A_LED_Y_MM;
//	float x_b1 = edge_position_b[1] - GEO_B_LED_Y_MM;
//	float x_b0 = edge_position_b[0] - GEO_B_LED_X_MM;
//
//	float X_a = (x_a1 + x_a0 == 0) ? 0 : GEO_A_LED_X_MM * tanf(0.5 * (atan2(x_a1, GEO_A_LED_X_MM) + atan2f(x_a0, GEO_A_LED_X_MM)));
//	float X_b = (x_b1 + x_b0 == 0) ? 0 : GEO_B_LED_Y_MM * tanf(0.5 * (atan2(x_b1, GEO_B_LED_Y_MM) + atan2f(x_b0, GEO_B_LED_Y_MM)));
//
//	float P_a = X_a + GEO_A_LED_Y_MM;
//	float P_b = X_b + GEO_B_LED_X_MM;
//			
//	float fil_x = (GEO_A_LED_X_MM * (GEO_B_LED_X_MM * P_a - P_a * P_b + GEO_B_LED_Y_MM * P_b)) / (GEO_A_LED_X_MM * GEO_B_LED_Y_MM - P_a * P_b - GEO_A_LED_Y_MM * GEO_B_LED_X_MM + GEO_A_LED_Y_MM * P_b + GEO_B_LED_X_MM * P_a);
//	float fil_y = (GEO_B_LED_Y_MM * (GEO_A_LED_X_MM * P_a - P_a * P_b + GEO_A_LED_Y_MM * P_b)) / (GEO_A_LED_X_MM * GEO_B_LED_Y_MM - P_a * P_b - GEO_A_LED_Y_MM * GEO_B_LED_X_MM + GEO_A_LED_Y_MM * P_b + GEO_B_LED_X_MM * P_a);
//
//	float dia_a = 2.0 / 10.0 * sqrtf((fil_y - GEO_A_LED_Y_MM) * (fil_y - GEO_A_LED_Y_MM) + (GEO_A_LED_X_MM - fil_x) * (GEO_A_LED_X_MM - fil_x)) * sinf(0.5 * (atan2f(x_a1, GEO_A_LED_X_MM) - atan2f(x_a0, GEO_A_LED_X_MM)));
//	float dia_b = 2.0 / 10.0 * sqrtf((fil_x - GEO_B_LED_X_MM) * (fil_x - GEO_B_LED_X_MM) + (GEO_B_LED_Y_MM - fil_y) * (GEO_B_LED_Y_MM - fil_y)) * sinf(0.5 * (atan2f(x_b1, GEO_B_LED_Y_MM) - atan2f(x_b0, GEO_B_LED_Y_MM)));
//
//	return GEO_Averaged_Diameter(dia_a, dia_b);
//}

float GEO_Filament_Diameter(EdgeData edge_position_a, EdgeData edge_position_b)
{
	GEO_Add_Sensor_Offsets(&edge_position_a, &edge_position_b);

	float x_a1 = edge_position_a.E1 - GEO_A_LED_Y_MM;
	float x_a0 = edge_position_a.E0 - GEO_A_LED_Y_MM;
	float x_b1 = edge_position_b.E1 - GEO_B_LED_Y_MM;
	float x_b0 = edge_position_b.E0 - GEO_B_LED_X_MM;

	float X_a = (x_a1 + x_a0 == 0) ? 0 : GEO_A_LED_X_MM * tanf(0.5 * (atan2(x_a1, GEO_A_LED_X_MM) + atan2f(x_a0, GEO_A_LED_X_MM)));
	float X_b = (x_b1 + x_b0 == 0) ? 0 : GEO_B_LED_Y_MM * tanf(0.5 * (atan2(x_b1, GEO_B_LED_Y_MM) + atan2f(x_b0, GEO_B_LED_Y_MM)));

	float P_a = X_a + GEO_A_LED_Y_MM;
	float P_b = X_b + GEO_B_LED_X_MM;
			
	float fil_x = (GEO_A_LED_X_MM * (fmaf(GEO_B_LED_X_MM, P_a, fmaf(-P_a, P_b, GEO_B_LED_Y_MM * P_b)))) / (GEO_A_LED_X_MM * GEO_B_LED_Y_MM + fmaf(-P_a, P_b, -GEO_A_LED_Y_MM * GEO_B_LED_X_MM) + fmaf(GEO_A_LED_Y_MM, P_b, GEO_B_LED_X_MM * P_a));
	float fil_y = (GEO_B_LED_Y_MM * (fmaf(GEO_A_LED_X_MM, P_a, fmaf(-P_a, P_b, GEO_A_LED_Y_MM * P_b)))) / (GEO_A_LED_X_MM * GEO_B_LED_Y_MM + fmaf(-P_a, P_b, -GEO_A_LED_Y_MM * GEO_B_LED_X_MM) + fmaf(GEO_A_LED_Y_MM, P_b, GEO_B_LED_X_MM * P_a));

	float dia_a = 2.0f / 10.0f * sqrtf((fil_y - GEO_A_LED_Y_MM) * (fil_y - GEO_A_LED_Y_MM) + (GEO_A_LED_X_MM - fil_x) * (GEO_A_LED_X_MM - fil_x)) * sinf((atan2f(x_a1, GEO_A_LED_X_MM) - atan2f(x_a0, GEO_A_LED_X_MM))/2.0f);
	float dia_b = 2.0f / 10.0f * sqrtf((fil_x - GEO_B_LED_X_MM) * (fil_x - GEO_B_LED_X_MM) + (GEO_B_LED_Y_MM - fil_y) * (GEO_B_LED_Y_MM - fil_y)) * sinf((atan2f(x_b1, GEO_B_LED_Y_MM) - atan2f(x_b0, GEO_B_LED_Y_MM))/2.0f);

	return GEO_Averaged_Diameter(dia_a, dia_b);
}
