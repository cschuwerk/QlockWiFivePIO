/**
 * TransitionFade.cpp
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

#include "TransitionFade.h"
#include "Debug.h"
#define DEBUG

TransitionFade::TransitionFade(LedDriverBase* ledDriver) :
		TransitionBase(ledDriver) {
	resetBuffers();
	m_marixChanged = false;
}

void TransitionFade::resetBuffers() {
	memset(m_matrixOld, 0, sizeof(m_matrixOld));
	memset(m_matrixNew, 0, sizeof(m_matrixNew));
	memset(m_matrixCommon, 0, sizeof(m_matrixCommon));
	memset(m_matrixFadeIn, 0, sizeof(m_matrixFadeIn));
	memset(m_matrixFadeOut, 0, sizeof(m_matrixFadeOut));
}

void TransitionFade::begin(uint16_t matrixOld[10], uint16_t matrixNew[10]) {
	resetBuffers();
	memcpy(m_matrixOld, matrixOld, 20);
	memcpy(m_matrixNew, matrixNew, 20);
	for (uint8_t i = 0; i < 10; i++) {
		m_matrixCommon[i] = m_matrixOld[i] & m_matrixNew[i];
		m_matrixFadeIn[i] = m_matrixNew[i] & ~m_matrixCommon[i];
		m_matrixFadeOut[i] = m_matrixOld[i] & ~m_matrixCommon[i];
	}

	m_marixChanged = false;
	for (uint8_t i = 0; i < 10; i++) {
		if (m_matrixNew[i] != m_matrixOld[i])
			m_marixChanged = true;
	}

	resetTransition();
	nextStep();
}

uint8_t TransitionFade::nextStep() {
	uint8_t ret = 0;
	_counter++;
	uint8_t endCount;
	clearBuffer();
	if (m_marixChanged) {
		endCount = 50;
		m_pLedDriver->writeScreenBuffer(m_matrixFadeIn, (uint8_t) (_counter * 2));
		m_pLedDriver->writeScreenBuffer(m_matrixFadeOut, (uint8_t) ((50 - _counter) * 2));
		m_pLedDriver->writeScreenBuffer(m_matrixCommon);
	} else if (_counter < 50) {
		endCount = 100;
		m_pLedDriver->writeScreenBuffer(m_matrixCommon, (uint8_t) ((50 - _counter) * 2));
	} else {
		endCount = 100;
		m_pLedDriver->writeScreenBuffer(m_matrixCommon, (uint8_t) ((_counter - 50) * 2));
	}

	if (_counter >= endCount) {
		ret = 2;
	}
	showOnScreen();
	return (ret);
}

