/******************************************************************************
 LedDriver_LPD8806RGBW.cpp
 ******************************************************************************/

#include "LedDriver_LPD8806RGBW.h"

LedDriver_LPD8806RGBW::LedDriver_LPD8806RGBW(eLayout layout) : LedDriverBase(layout) {
	m_numLeds = getLedsCount(layout);
	strip = new LPD8806RGBW(m_numLeds, PIN_LEDS_DATA, PIN_LEDS_CLOCK);
	strip->begin();
}

String LedDriver_LPD8806RGBW::getSignature() {
	return "LPD8806RGBW";
}

void LedDriver_LPD8806RGBW::clear() {
	for (uint8_t i = 0; i < m_numLeds; i++)
		strip->setPixelColor(i, 0);
}

void LedDriver_LPD8806RGBW::show() {
	strip->show();
}

void LedDriver_LPD8806RGBW::setPixel(uint8_t num, uint32_t color) {
	strip->setPixelColor(num, color);
}
