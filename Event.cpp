/**
 * Event.cpp
 * Klasse für ein jährliches Event
 *
 * @mc       Arduino/UNO
 * @autor    Manuel Bracher / manuel.bracher@gmail.com
 * @version  1.0
 * @created  02.01.15
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 */

#include "Event.h"

// #define DEBUG
#include "Debug.h"
#include "Settings.h"

extern Settings settings;

void Event::show(event_t event) {
	DEBUG_PRINT(F("Ticker String: "));DEBUG_PRINTLN(event.txt);
	if (strlen(event.txt) != 0)
		Effects::showTickerString(event.txt, TICKER_SPEED, (eColor) settings.getColorNum());
	if (event.effect < Effects::BITMAP_MIN) {
		switch (event.effect) {
		case Effects::NO_EFFECT:
			break;
		case Effects::EFFECT_FIREWORK:
			Effects::showFireWork(5, event.color);
			Effects::showFireWork(2, event.color);
			Effects::showFireWork(8, event.color);
			break;
		case Effects::EFFECT_HEART:
			Effects::showHeart(event.color);
			break;
		case Effects::EFFECT_CANDLE:
			Effects::showCandle(event.color);
			break;

		default:
			;
		}
	}
	if ((event.effect >= Effects::BITMAP_MIN) && (event.effect < Effects::ANI_BITMAP_MIN)) {
		Effects::showBitmap(event.effect, 225, event.color);
	}
	if (event.effect >= Effects::ANI_BITMAP_MIN) {
		Effects::showAnimatedBitmap(event.effect, event.color);
	}
}

