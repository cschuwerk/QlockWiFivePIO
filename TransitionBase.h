/**
 * Transition.h
 * Transitionen bei Wechsel Zeitmatrix
 *
 * @mc       Arduino/UNO
 * @autor    Manuel Bracher / manuel.bracher@gmail.com
 * @version  1.0
 * @created  18.08.2017
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 */

#ifndef TRANSITION_BASE_H
#define TRANSITION_BASE_H

#include "Arduino.h"
#include "LedDriverBase.h"

class TransitionBase {
public:
	TransitionBase(LedDriverBase* ledDriver);
	virtual ~TransitionBase() {};

	static void resetTransition();

	virtual uint8_t nextStep() = 0;
	virtual void begin(uint16_t matrixOld[10], uint16_t matrixNew[10]) = 0;

protected:
	static void setTransitionActive(bool isActive);
	static bool isTransitionActive();

	virtual void resetBuffers() = 0;

	void clearBuffer();
	void showOnScreen();

	static bool m_transitionActive;
	static byte _counter;
	static uint16_t _usedColumns;
	static byte _remainingCoulumnCount;
	static boolean _ereasingDone;

	LedDriverBase* m_pLedDriver;
};

#endif //TRANSITION_BASE_H
