#include "buffer.h"

/* Calculates a new volumetric multiplier based on the current diameter measurement */
inline uint16_t BUFF_GetNewMultiplier(void)
{
	float nominal = MAIN_NominalFilamentDiameter_MM;
	float current = MAIN_FilamentDiameter_MM;
	float ratio = nominal*nominal/(current*current);
	return (uint16_t)(ratio * BUFF_FLOAT_TO_INT_MULTIPLIER);
}

/* Initializes the volumetric ratio buffer */
/* Return value == 0 -> SUCCESS */
/* Return value != 0 -> ERROR   */
uint16_t BUFF_InitializeBuffer(void)
{
	uint16_t i;

	RING_Init();
	if( RING_SetLength(MAIN_FilamentBufferDistance_MM) )
	{
		return 9999; /* ERROR: buffer length bounds */
	}

	for(i = 0; i < MAIN_FilamentBufferDistance_MM; i++)
	{
		if( RING_Push(BUFF_FLOAT_TO_INT_MULTIPLIER) )
		{	
			return 8888; /* ERROR */
		}
	}

	return 0;
}

/* Returns the requested (old) multiplier, pushes the current multiplier to the buffer */
uint16_t BUFF_UpdateBufferMultiplier(void)
{
	uint16_t outputMultiplier;

	if( RING_Pop(&outputMultiplier) )
	{
		return 0xFFFF; /* ERROR: buffer empty */
	}

	RING_Push( BUFF_GetNewMultiplier() );

	return outputMultiplier;
//	return BUFF_FLOAT_TO_INT_MULTIPLIER;
}