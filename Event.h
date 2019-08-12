/**
 * Event.h
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

#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include "Effects.h"
#include "Configuration.h"
#include "Colors.h"

#define TICKER_SPEED  6
#define LEN_EVT_STR 30

typedef struct event_s {
	time_t time;
	char txt[LEN_EVT_STR];
	Effects::eEffects effect;
	eColor color;bool enabled;
} event_t;

const char sEvtRep[][4] PROGMEM = { { "5" },   // 00
		{ "15" }, // 01
		{ "30" }, // 02
		{ "60" }, // 03
		};

enum eEvtRep : uint8_t {
	EVT_REP_5,
	EVT_REP_15,
	EVT_REP_30,
	EVT_REP_60,
	EVT_REP_COUNT
};

class Event {
public:
	Event() {};

	static void show(event_t event);
};

#endif //EVENT_H
