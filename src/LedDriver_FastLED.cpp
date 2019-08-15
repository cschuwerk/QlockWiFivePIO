/******************************************************************************
 LedDriver_FastLED.cpp
 ******************************************************************************/

#include "LedDriver_FastLED.h"

LedDriver_FastLED::LedDriver_FastLED(eLayout layout) : LedDriverBase(layout){
	m_numLeds = getLedsCount(layout);
	leds = new CRGB[m_numLeds];
#ifdef LED_DRIVER_FAST_APA102
	FastLED.addLeds<APA102, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_APA104
	FastLED.addLeds<APA104, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_APA106
	FastLED.addLeds<APA106, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_DOTSTAR
	FastLED.addLeds<DOTSTAR, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_DMXSIMPLE
	FastLED.addLeds<DMXSIMPLE, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_GW6205
	FastLED.addLeds<GW6205, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_GW6205_400
	FastLED.addLeds<GW6205_400, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_LPD1886
	FastLED.addLeds<LPD1886, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_LPD1886_8BIT
	FastLED.addLeds<LPD1886_8BIT, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_LPD8806
	FastLED.addLeds<LPD8806, PIN_LEDS_DATA, PIN_LEDS_CLOCK, BRG>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_NEOPIXEL
	FastLED.addLeds<NEOPIXEL, PIN_LEDS_DATA>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_P9813
	FastLED.addLeds<P9813, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_PL9823
	FastLED.addLeds<PL9823, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_SK6812
	FastLED.addLeds<SK6812, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_SK6822
	FastLED.addLeds<SK6822, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_SK9822
	FastLED.addLeds<SK9822, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_SM16716
	FastLED.addLeds<SM16716, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_TM1803
	FastLED.addLeds<TM1803, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_TM1804
	FastLED.addLeds<TM1804, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_TM1809
	FastLED.addLeds<TM1809, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_TM1812
	FastLED.addLeds<TM1812, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_TM1829
	FastLED.addLeds<TM1829, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_UCS1903
	FastLED.addLeds<UCS1903, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_UCS1903B
	FastLED.addLeds<UCS1903B, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_UCS1904
	FastLED.addLeds<UCS1904, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_UCS2903
	FastLED.addLeds<UCS2903, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2801
	FastLED.addLeds<WS2801, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2803
	FastLED.addLeds<WS2803, PIN_LEDS_DATA, PIN_LEDS_CLOCK, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2811
	FastLED.addLeds<WS2811, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2811_400
	FastLED.addLeds<WS2811_400, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2812
	FastLED.addLeds<WS2812, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2812B
	FastLED.addLeds<WS2812B, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2813
	FastLED.addLeds<WS2813, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
#ifdef LED_DRIVER_FAST_WS2852
	FastLED.addLeds<WS2852, PIN_LEDS_DATA, RGB>(leds, m_numLeds);
#endif
}

String LedDriver_FastLED::getSignature() {
	return "FastLED";
}

void LedDriver_FastLED::clear() {
	FastLED.clear();
}

void LedDriver_FastLED::show() {
	FastLED.show();
}

void LedDriver_FastLED::setPixel(uint8_t num, uint32_t color) {
	leds[num] = color;
}
