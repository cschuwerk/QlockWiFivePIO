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

#ifndef TRANSITION_SLIDE_H
#define TRANSITION_SLIDE_H

#include "Arduino.h"
#include "TransitionBase.h"

class TransitionSlide: public TransitionBase {
public:
	TransitionSlide(LedDriverBase* ledDriver);
	virtual ~TransitionSlide() {};

	uint8_t nextStep();
	void begin(uint16_t matrixOld[10], uint16_t matrixNew[10]);

private:
	void resetBuffers();
	void shiftColumnDown(uint16_t matrixOld[10], byte column);
	void shiftColumnUp(uint16_t matrixOut[10], uint16_t matrixNew[10], byte column);
	uint16_t m_matrixOld[10];
	uint16_t m_matrixNew[10];
};

#endif //TRANSITION_SLIDE_H
