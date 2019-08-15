/**
 * LDR
 * Klasse fuer den Zugriff auf einen lichtabhaengigen Widerstand.
 *
 * @mc       Arduino/ESP8266
 * @autor    Manuel Bracher
 * @version  1.0
 * @created  27.07.2017
 *
 */

#include "LDR.h"

//#define DEBUG
#include "Debug.h"

/**
 * Initialisierung mit dem Pin, an dem der LDR haengt.
 * Die Maximalwerte vom LDR koennen automatisch
 * eingemessen werden (LDR_AUTOSCALE).
 * Ansonsten muss man diese Werte im #define-DEBUG-Mode
 * ausmessen und eintragen.
 */
LDR::LDR(byte pin, boolean isInverted, uint8_t constrainMin, uint8_t constrainMax, boolean autoScale, uint16_t ldrHysterese, uint16_t minLdr, uint16_t maxLdr) {
	_pin = pin;
	_isInverted = isInverted;
	_autoScale = autoScale;
	_ldrMin = minLdr;
	_ldrMax = maxLdr;
	_constrainMin = constrainMin;
	_constrainMax = constrainMax;
	_ldrHysteresis = ldrHysterese;
	_lastValue = 1;
	_outputValue = 0;
	if (_autoScale) {
		_min = 1023;
		_max = 0;
	} else {
		_min = _ldrMin;
		_max = _ldrMax;
	}
}

/**
 * Welchen Wert hat der LDR? In Prozent...
 */
byte LDR::value() {
	int rawVal, val;
	if (!_isInverted) {
		rawVal = analogRead(_pin);
	} else {
		rawVal = (1023 - analogRead(_pin));
	}

	if ((rawVal != _lastValue)
			&& ((rawVal == 0) || (rawVal == 1023)
					|| (rawVal > (_lastValue + _ldrHysteresis) || (rawVal < _lastValue - _ldrHysteresis)))) {
		val = rawVal;
		_lastValue = val;
		if (_autoScale) {
			if (val < _min) {
				_min = val;
			}
			if (val > _max) {
				_max = val;
			}
		} else {
			val = constrain(val, _min, _max);
		}
		uint8_t mapVal = 50;
		if (_min != _max) {
			mapVal = map(val, _min, _max, 0, 255);
		}
		mapVal = constrain(mapVal, _constrainMin, _constrainMax);
		DEBUG_PRINT(F("rawVal: ")); DEBUG_PRINT(rawVal); DEBUG_PRINT(F(" val: ")); DEBUG_PRINT(val); DEBUG_PRINT(F(" _min: ")); DEBUG_PRINT(_min); DEBUG_PRINT(F(" _max: ")); DEBUG_PRINT(_max); DEBUG_PRINT(F(" mapValue: ")); DEBUG_PRINTLN(mapVal);
		_outputValue = mapVal;
	}
	return _outputValue;
}
