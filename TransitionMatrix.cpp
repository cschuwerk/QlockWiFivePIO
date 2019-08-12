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

#include "TransitionMatrix.h"

TransitionMatrix::TransitionMatrix(LedDriverBase* ledDriver) :
		TransitionBase(ledDriver) {
	resetBuffers();
}

void TransitionMatrix::resetBuffers() {
	memset(m_matrixOld, 0, sizeof(m_matrixOld));
	memset(m_matrixNew, 0, sizeof(m_matrixNew));
	memset(m_matrixMatrix, 0, sizeof(m_matrixMatrix));
	memset(m_matrixMatrix, 0, sizeof(m_matrixMatrixTail));
	memset(m_matrixWeak, 0, sizeof(m_matrixWeak));
}

void TransitionMatrix::begin(uint16_t matrixOld[10], uint16_t matrixNew[10]) {
	resetBuffers();
	memcpy(m_matrixOld, matrixOld, 20);
	memcpy(m_matrixNew, matrixNew, 20);
	resetTransition();
	nextStep();
}

uint8_t TransitionMatrix::nextStep() {
	byte loopCount = random(0, 5);
	byte column;
	uint8_t ret = 0;

	clearBuffer();

	if (!_ereasingDone) {
		shiftDownMatrixErease(m_matrixMatrix, m_matrixWeak);
		if (random(0, 3) > 0) {
			if ((_remainingCoulumnCount < 3) && (_remainingCoulumnCount > 0)) {
				m_matrixMatrix[0] |= ~_usedColumns;
				_usedColumns |= m_matrixMatrix[0];
				_remainingCoulumnCount = 0;
			} else {
				for (byte i = 0; i < loopCount; i++) {
					column = random(4, 16);
					if (!(_usedColumns & (0x01 << column))) {
						_usedColumns |= (0x01 << column);
						_remainingCoulumnCount--;
					}
					m_matrixMatrix[0] |= 0x01 << column;
				}
			}
		}
	} else {
		shiftDownMatrixWrite(m_matrixMatrix, m_matrixWeak);
		if ((random(0, 3) > 0) && (_remainingCoulumnCount > 0)) {
			if (_remainingCoulumnCount < 3) {
				m_matrixMatrix[0] |= ~_usedColumns;
				_usedColumns |= m_matrixMatrix[0];
				_remainingCoulumnCount = 0;
			} else {
				for (byte i = 0; i < loopCount; i++) {
					column = random(4, 16);
					if (!(_usedColumns & (0x01 << column))) {
						_usedColumns |= (0x01 << column);
						_remainingCoulumnCount--;
						m_matrixMatrix[0] |= 0x01 << column;
					}
				}
			}
		}
	}

	if (!_remainingCoulumnCount) {
		_counter++;
		if (_counter > 11) {
			if (!_ereasingDone) {
				for (byte i = 0; i < 11; i++) {
					m_matrixOld[i] = m_matrixNew[i];
				}
				_usedColumns = 0;
				_remainingCoulumnCount = 12;
				_counter = 0;
				_ereasingDone = true;
				ret = 1;
			} else {
				ret = 2;
			}
		}
	}

	m_pLedDriver->writeScreenBuffer(m_matrixOld);
	m_pLedDriver->writeScreenBuffer(m_matrixWeak, LedDriverBase::getScaledColor(MATRIX_COLOR, 0.1 * 0xFF));
	m_pLedDriver->writeScreenBuffer(m_matrixMatrixTail, LedDriverBase::getScaledColor(MATRIX_COLOR, 0.5 * 0xFF));
	m_pLedDriver->writeScreenBuffer(m_matrixMatrix, (uint32_t) MATRIX_COLOR);

	memcpy(m_matrixMatrixTail, m_matrixMatrix, 24);

	showOnScreen();

	return (ret);
}

void TransitionMatrix::shiftDownMatrixErease(uint16_t matrixMatrix[12], uint16_t matrixWeak[12]) {
	for (byte i = 10; i > 0; i--) {
		matrixMatrix[i] = matrixMatrix[i - 1];
		matrixWeak[i] |= matrixWeak[i - 1];
	}
	matrixWeak[0] = (matrixMatrix[1] | matrixWeak[1]);
	matrixMatrix[0] = 0;
}

void TransitionMatrix::shiftDownMatrixWrite(uint16_t matrixMatrix[12], uint16_t matrixWeak[12]) {
	for (byte i = 10; i > 0; i--) {
		matrixMatrix[i] = matrixMatrix[i - 1];
		matrixWeak[i] = matrixWeak[i - 1];
	}
	matrixWeak[0] = (~matrixMatrix[1] & matrixWeak[1]);
	matrixMatrix[0] = 0;
}

