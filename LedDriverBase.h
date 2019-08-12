/******************************************************************************
 LedDriverBase.h
 ******************************************************************************/

#ifndef LEDDRIVERBASE_H
#define LEDDRIVERBASE_H

#include "Colors.h"
#include "Layouts.h"

class LedDriverBase {

public:
	LedDriverBase(eLayout layout);
	virtual ~LedDriverBase() {};

	virtual String getSignature() = 0;
	virtual void clear() = 0;
	virtual void show() = 0;
	virtual void setPixel(uint8_t num, uint32_t color) = 0;

	void setLayout(eLayout layout);
	void setScreenColor(uint32_t color, bool update = false);
	void setBrightness(uint8_t brightness, bool update = false);
	void setScreenBuffer(uint16_t screenBuffer[]);
	void updateScreen();
	void writeScreenBuffer(uint8_t scaleFactor = 100);
	void writeScreenBuffer(uint16_t screenBuffer[], uint8_t scaleFactor = 100);
	void writeScreenBuffer(uint16_t screenBuffer[], uint32_t color);

	void clearStrip();
	void showStrip();

	static uint32_t getDefaultColor(uint8_t color, uint8_t brightness = 0xFF);
	static uint32_t getScaledColor(uint32_t color, uint8_t brightness = 0xFF);
	static uint32_t wheel(byte wheelPos, byte brightness = 0xFF);
	static uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
	void updateColorWheel(bool update);
private:
	const layout_t* m_pLayout;
	uint16_t m_screenBuffer[10];
	uint8_t m_brightness;
	uint32_t m_color;
	uint8_t m_wheelPos;
protected:
	uint16_t m_numLeds;
};

#endif
