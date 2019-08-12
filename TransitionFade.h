/**
 * TransitionFade.h
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

#ifndef TRANSITION_FADE_H
#define TRANSITION_FADE_H

#include "Arduino.h"
#include "TransitionBase.h"

class TransitionFade: public TransitionBase {
public:
	TransitionFade(LedDriverBase* ledDriver);
	virtual ~TransitionFade() {};

	uint8_t nextStep();
	void begin(uint16_t matrixOld[10], uint16_t matrixNew[10]);

private:
	void resetBuffers();
	uint16_t m_matrixOld[10];
	uint16_t m_matrixNew[10];

	uint16_t m_matrixCommon[10];
	uint16_t m_matrixFadeIn[10];
	uint16_t m_matrixFadeOut[10];

	bool m_marixChanged;
};

#endif //TRANSITION_FADE_H
