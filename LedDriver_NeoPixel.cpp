/******************************************************************************
 LedDriver_NeoPixel.cpp
 ******************************************************************************/

#include "LedDriver_NeoPixel.h"

LedDriver_NeoPixel::LedDriver_NeoPixel(eLayout layout) : LedDriverBase(layout) {
	m_numLeds = getLedsCount(layout);
#ifdef LED_DRIVER_NEO_800KHZ_GRB
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_GRB + NEO_KHZ800);
#endif
#ifdef LED_DRIVER_NEO_800KHZ_RGB
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_RGB + NEO_KHZ800);
#endif
#ifdef LED_DRIVER_NEO_800KHZ_RGBW
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_RGBW + NEO_KHZ800);
#endif
#ifdef LED_DRIVER_NEO_800KHZ_GRBW
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_GRBW + NEO_KHZ800);
#endif
#ifdef LED_DRIVER_NEO_400KHZ_GRB
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_GRB + NEO_KHZ400);
#endif
#ifdef LED_DRIVER_NEO_400KHZ_RGB
	strip = new Adafruit_NeoPixel(m_numLeds, PIN_LEDS_DATA, NEO_RGB + NEO_KHZ400);
#endif
	strip->begin();
}

String LedDriver_NeoPixel::getSignature() {
	return "NeoPixel";
}

void LedDriver_NeoPixel::clear() {
	strip->clear();
}

void LedDriver_NeoPixel::show() {
	strip->show();
}

void LedDriver_NeoPixel::setPixel(uint8_t num, uint32_t color) {
	strip->setPixelColor(num, color);
}

