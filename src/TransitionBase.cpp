/**
 * TransitionBase.cpp
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

#include "TransitionBase.h"

byte TransitionBase::_counter;
uint16_t TransitionBase::_usedColumns;
byte TransitionBase::_remainingCoulumnCount;
boolean TransitionBase::_ereasingDone;

void TransitionBase::resetTransition() {
	_counter = 0;
	_usedColumns = 0;
	_remainingCoulumnCount = 12;
	_ereasingDone = false;
}

TransitionBase::TransitionBase(LedDriverBase* ledDriver) :
		m_pLedDriver(ledDriver) {
}

void TransitionBase::setTransitionActive(bool isActive) {
	m_transitionActive = isActive;
}

bool TransitionBase::isTransitionActive() {
	return m_transitionActive;
}

void TransitionBase::clearBuffer() {
	m_pLedDriver->clearStrip();
}

void TransitionBase::showOnScreen() {
	m_pLedDriver->showStrip();
}

