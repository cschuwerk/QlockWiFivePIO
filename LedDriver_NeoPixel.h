/******************************************************************************
 LedDriver_NeoPixel.h
 ******************************************************************************/

#ifndef LEDDRIVER_NEOPIXEL_H
#define LEDDRIVER_NEOPIXEL_H

#include <Adafruit_NeoPixel.h>
#include "LedDriverBase.h"
#include "Configuration.h"

class LedDriver_NeoPixel: public LedDriverBase {

public:
	LedDriver_NeoPixel(eLayout layout);
	String getSignature();
	void clear();
	void show();
	void setPixel(uint8_t num, uint32_t color);

private:
	Adafruit_NeoPixel *strip;
};

#endif
