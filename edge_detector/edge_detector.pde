#include "detection.h"
#include "tsl1401cl.h"
#include "configuration.h"
#include "movingAverage.h"

int16_t x[CFG_PIXEL_COUNT];

void setup()
{
	//Serial.begin(9600);
	TSL_Init();
	pinMode(9, OUTPUT);
}

void loop()
{
	TSL_MeasurePixels(x);
	MOV_Push(DET_PixelsBetweenEdges(x));
	analogWrite(9, MOV_Average());
	//Serial.println(MOV_Average());
}

