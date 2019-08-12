/**
 * TransitionSlide.cpp
 * Transitionen bei Wechsel Zeitmatrix
 *
 * @mc       Arduino/UNO
 * @autor    Manuel Bracher / manuel.bracher@gmail.com
 * @version  1.0
 * @created  19.05.15
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 */

#include "TransitionSlide.h"

TransitionSlide::TransitionSlide(LedDriverBase* ledDriver) :
		TransitionBase(ledDriver) {
	resetBuffers();
}

void TransitionSlide::resetBuffers() {
	memset(m_matrixOld, 0, sizeof(m_matrixOld));
	memset(m_matrixNew, 0, sizeof(m_matrixNew));
}

void TransitionSlide::begin(uint16_t matrixOld[10], uint16_t matrixNew[10]) {
	memcpy(m_matrixOld, matrixOld, 20);
	memcpy(m_matrixNew, matrixNew, 20);
	resetTransition();
	nextStep();
}

uint8_t TransitionSlide::nextStep() {
	uint8_t ret = 0;
	clearBuffer();
	_counter++;
	if (_counter < 22) {
		for (byte i = 0; i < _min(_counter, 12); i++) {
			shiftColumnDown(m_matrixOld, i);
		}
	} else {
		for (byte i = max(_counter - 22 - 9, 0); i < _min(_counter - 21, 12); i++) {
			shiftColumnUp(m_matrixOld, m_matrixNew, i);
		}
	}

	m_pLedDriver->writeScreenBuffer(m_matrixOld);
	if (_counter > 20) {
		ret = 1;
	}
	if (_counter > 41) {
		ret = 2;
	}
	showOnScreen();
	return (ret);
}

void TransitionSlide::shiftColumnDown(uint16_t matrixOld[10], byte column) {
	uint16_t tempMatrix = (0x01 << (15 - column));
	for (byte i = 9; i > 0; i--) {
		matrixOld[i] &= ~tempMatrix;
		matrixOld[i] |= (matrixOld[i - 1] & tempMatrix);
	}
	matrixOld[0] &= ~tempMatrix;
}

void TransitionSlide::shiftColumnUp(uint16_t matrixOut[10], uint16_t matrixNew[10], byte column) {
	uint16_t tempMatrix = (0x01 << (15 - column));
	byte tempIdx;
	for (byte i = 0; i < 10; i++) {
		matrixOut[i] &= ~tempMatrix;
		tempIdx = i - (9 + column - (_counter - 22));
		if (tempIdx < 10 && tempIdx >= 0) {
			matrixOut[i] |= (matrixNew[tempIdx] & tempMatrix);
		}
	}
}

