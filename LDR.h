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

#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Configuration.h"

class LDR {
public:
	LDR(byte pin, boolean isInverted, uint8_t constrainMin, uint8_t constrainMax, boolean autoScale = true, uint16_t ldrHysteresis = 50, uint16_t minLdr = 0, uint16_t maxLdr = 1023);

	byte value();

private:
	byte _pin;
	boolean _isInverted;
	boolean _autoScale;
	uint16_t _lastValue;
	byte _outputValue;
	uint16_t _min;
	uint16_t _max;
	uint16_t _ldrMin;
	uint16_t _ldrMax;
	uint16_t _ldrHysteresis;
	uint8_t _constrainMin;
	uint8_t _constrainMax;
};

#endif
