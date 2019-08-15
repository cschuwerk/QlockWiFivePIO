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

#ifndef TRANSITION_MATRIX_H
#define TRANSITION_MATRIX_H

#include "Arduino.h"
#include "TransitionBase.h"

#define MATRIX_COLOR 0x00FF00

class TransitionMatrix: public TransitionBase {
public:
	TransitionMatrix(LedDriverBase* ledDriver);
	virtual ~TransitionMatrix() {};

	uint8_t nextStep();
	void begin(uint16_t matrixOld[10], uint16_t matrixNew[10]);

private:
	void resetBuffers();
	void shiftDownMatrixErease(uint16_t matrixMatrix[12], uint16_t matrixWeak[12]);
	void shiftDownMatrixWrite(uint16_t matrixMatrix[12], uint16_t matrixWeak[12]);
	uint16_t m_matrixOld[12];
	uint16_t m_matrixNew[12];
	uint16_t m_matrixMatrix[12];
	uint16_t m_matrixWeak[12];
	uint16_t m_matrixMatrixTail[12];
};

#endif //TRANSITION_MATRIX_H
