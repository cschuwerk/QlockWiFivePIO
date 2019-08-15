/******************************************************************************
 LedDriver_LPD8806RGBW.h
 ******************************************************************************/

#ifndef LEDDRIVER_LPD8806RGBW_H
#define LEDDRIVER_LPD8806RGBW_H

#include <LPD8806RGBW.h>
#include "LedDriverBase.h"
#include "Configuration.h"

class LedDriver_LPD8806RGBW: public LedDriverBase {

public:
	LedDriver_LPD8806RGBW(eLayout layout);
	String getSignature();
	void clear();
	void show();
	void setPixel(uint8_t num, uint32_t color);

private:
	LPD8806RGBW *strip;
};

#endif
