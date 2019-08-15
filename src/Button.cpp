/**
 * Button
 * Kleine Klasse zum Entprellen der Tasten.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.7
 * @created  18.2.2011
 * @updated  16.2.2015
 *
 * Versionshistorie:
 * V 1.1:  - Kompatibilitaet zu Arduino-IDE 1.0 hergestellt.
 * V 1.2:  - Optimierung hinsichtlich Speicherbedarf.
 * V 1.3:  - Verbessertes Debugging.
 * V 1.4:  - Doppel-Tasten-Abfrage ermoeglicht.
 * V 1.5:  - Ueberlauf in millis() beruecksichtigt.
 * V 1.6:  - Schalten gegen LOW ermoeglicht.
 * V 1.7:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 */
#include "Button.h"

// #define DEBUG
#include "Debug.h"

#include "Configuration.h"

/**
 * Initialisierung mit dem Pin, an dem der Taster haengt.
 *
 * @param  pin: der Pin, an dem der Taster haengt
 *         pressedAgainst: wogegen schaltet der Taster? (HIGH/LOW)
 */
Button::Button(byte pin, byte pressedAgainst, uint16_t debounceTime) {
	_pin1 = pin;
	_lastPressTime = 0;
	_doubleMode = false;
	_pressedAgainst1 = pressedAgainst;
	_debounceTime = debounceTime;
	if (_pressedAgainst1 == HIGH) {
		if(_pin1 == D0){
			pinMode(_pin1, INPUT_PULLDOWN_16);
		} else {
			pinMode(_pin1, INPUT);
		}
	} else {
		pinMode(_pin1, INPUT_PULLUP);
	}
}

/**
 * Initialisierung mit den zwei Pins, an denen die Taster haengen, die man gleichzeitig abfragen moechte.
 *
 * @param  pin1, pin2: die Pins, an dem der virtuelle Taster haengt
 *         pressedAgainst: wogegen schalten die Taster? (HIGH/LOW)
 */
Button::Button(byte pin1, byte pin2, byte pressedAgainst1, byte pressedAgainst2, uint16_t debounceTime) {
	_pin1 = pin1;
	_pin2 = pin2;
	_lastPressTime = 0;
	_doubleMode = true;
	_pressedAgainst1 = pressedAgainst1;
	_pressedAgainst2 = pressedAgainst2;
	_debounceTime = debounceTime;
	if (_pressedAgainst1 == HIGH) {
		if(_pin1 == D0){
			pinMode(_pin1, INPUT_PULLDOWN_16);
		} else {
			pinMode(_pin1, INPUT);
		}
	} else {
		pinMode(_pin1, INPUT_PULLUP);
	}
	if (_pressedAgainst2 == HIGH) {
		if (_pin2 == D0) {
			pinMode(_pin2, INPUT_PULLDOWN_16);
		} else {
			pinMode(_pin2, INPUT);
		}
	} else {
		pinMode(_pin2, INPUT_PULLUP);
	}
}

/**
 * Wurde der Taster gedrueckt?
 */
boolean Button::pressed() {
	boolean _retVal = false;

	if (!_doubleMode) {
		if ((digitalRead(_pin1) == _pressedAgainst1) && (millis() - _lastPressTime > _debounceTime)) {
			_lastPressTime = millis();
			_retVal = true;
		}
	} else {
		if ((digitalRead(_pin1) == _pressedAgainst1) && (digitalRead(_pin2) == _pressedAgainst2)
				&& (millis() - _lastPressTime > _debounceTime)) {
			_lastPressTime = millis();
			_retVal = true;
		}
	}

	return _retVal;
}

/**
 * Ist der Taster aktuell gedrueckt?
 */
boolean Button::pressedRaw() {
	boolean _retVal = false;

	if (!_doubleMode) {
		if (digitalRead(_pin1) == _pressedAgainst1) {
			_retVal = true;
		}
	} else {
		if ((digitalRead(_pin1) == _pressedAgainst1) && (digitalRead(_pin2) == _pressedAgainst2)) {
			_retVal = true;
		}
	}

	return _retVal;
}
