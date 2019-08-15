/******************************************************************************
 LedDriver_FastLED.h
 ******************************************************************************/

#ifndef LEDDRIVER_FASTLED_H
#define LEDDRIVER_FASTLED_H

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "LedDriverBase.h"
#include "Configuration.h"

class LedDriver_FastLED: public LedDriverBase {

public:
	LedDriver_FastLED(eLayout layout);
	String getSignature();
	void clear();
	void show();
	void setPixel(uint8_t num, uint32_t color);

private:
	CRGB* leds;
};

#endif
