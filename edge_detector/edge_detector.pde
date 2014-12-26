#include "detection.h"
#include "tsl1401cl.h"
#include "configuration.h"

int16_t x[CFG_PIXEL_COUNT];
uint32_t width[1U << CFG_MOVING_AVERAGE_SHIFTS];
uint32_t widthSum = 0;

void setup()
{
	Serial.begin(9600);
	TSL_Init();
	TSL_MeasurePixels(x);
	uint32_t initialWidth = DET_NanometersBetweenEdges(x);
	widthSum = initialWidth * (1U << CFG_MOVING_AVERAGE_SHIFTS);
	for (uint8_t i = 0; i < (1U << CFG_MOVING_AVERAGE_SHIFTS); i++)
	{
		width[i] = initialWidth;
	}
}

void loop()
{
	widthSum -= width[0];
	for (uint8_t i = 0; i < ((1U << CFG_MOVING_AVERAGE_SHIFTS)-1U); i++)
	{
		width[i] = width[i+1];
	}	
	TSL_MeasurePixels(x);
	width[(1U << CFG_MOVING_AVERAGE_SHIFTS)-1U] = DET_NanometersBetweenEdges(x);
	widthSum += width[(1U << CFG_MOVING_AVERAGE_SHIFTS)-1U];
	uint32_t movingAverageWidth = widthSum >> CFG_MOVING_AVERAGE_SHIFTS;
	Serial.print("Moving average width: ");
	Serial.println(movingAverageWidth);
}

