/******************************************************************************
 LedDriverBase.cpp
 ******************************************************************************/

#include "LedDriverBase.h"

LedDriverBase::LedDriverBase(eLayout layout) {
	setLayout(layout);
	m_brightness = 0xFF / 2;
	memset(m_screenBuffer, 0, sizeof(m_screenBuffer));
	m_wheelPos = 0;
	m_color = 0xFFFFFF;
}

void LedDriverBase::setLayout(eLayout layout) {
	m_pLayout = &layouts[layout];
}

void LedDriverBase::setBrightness(uint8_t brightness, bool update) {
	m_brightness = brightness;
	if (update) {
		updateScreen();
	}
}

void LedDriverBase::setScreenColor(uint32_t color, bool update) {
	m_color = color;
	if (update) {
		updateScreen();
	}
}

void LedDriverBase::setScreenBuffer(uint16_t screenBuffer[]) {
	memcpy(m_screenBuffer, screenBuffer, sizeof(m_screenBuffer));
	updateScreen();
}

void LedDriverBase::updateScreen() {
	clear();
	writeScreenBuffer();
	show();
}

void LedDriverBase::writeScreenBuffer(uint16_t screenBuffer[], uint32_t scaledColor) {
	for (uint8_t y = 0; y <= 9; y++) {
		for (uint8_t x = 0; x <= 10; x++) {
			if (bitRead(screenBuffer[y], 15 - x)){
				for(uint8_t i = 0; i < m_pLayout->ledsPerLetter; i++){
					setPixel(m_pLayout->matrix[y][x] + i, scaledColor);
				}
			}
		}
	}
	for (uint8_t y = 0; y < 4; y++) {
		if (bitRead(screenBuffer[y], 4)){
			for(uint8_t i = 0; i < m_pLayout->ledsPerCorner; i++){
				setPixel(m_pLayout->corners[y] + i, scaledColor);
			}
		}
	}
	if (bitRead(screenBuffer[4], 4)){
		for(uint8_t i = 0; i < m_pLayout->ledsPerAlarm; i++){
			setPixel(m_pLayout->alarm + i, scaledColor);
		}
	}
}

void LedDriverBase::writeScreenBuffer(uint16_t screenBuffer[], uint8_t scaleFactor) {
	uint32_t scaledColor = getScaledColor(m_color, (uint16_t) m_brightness * scaleFactor / 100);
	writeScreenBuffer(screenBuffer, scaledColor);
}

void LedDriverBase::writeScreenBuffer(uint8_t scaleFactor) {
	uint32_t scaledColor = getScaledColor(m_color, (uint16_t) m_brightness * scaleFactor / 100);
	writeScreenBuffer(m_screenBuffer, scaledColor);
}

uint32_t LedDriverBase::getColor(uint8_t r, uint8_t g, uint8_t b) {
	return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
}

uint32_t LedDriverBase::getScaledColor(uint32_t color, uint8_t brightness) {
	uint8_t red = map(brightness, 0, 255, 0, (color >> 16) & 0xFF);
	uint8_t green = map(brightness, 0, 255, 0, (color >> 8) & 0xFF);
	uint8_t blue = map(brightness, 0, 255, 0, (color & 0xFF));
	return getColor(red, green, blue);
}

void LedDriverBase::updateColorWheel(bool update) {
	m_wheelPos += 2;
	m_color = wheel(m_wheelPos);

	if (update) {
		updateScreen();
	}
}

uint32_t LedDriverBase::wheel(byte wheelPos, byte brightness) {
	if (wheelPos < 85) {
		return getScaledColor(getColor(wheelPos * 3, 255 - wheelPos * 3, 0), brightness);
	} else if (wheelPos < 170) {
		wheelPos -= 85;
		return getScaledColor(getColor(255 - wheelPos * 3, 0, wheelPos * 3), brightness);
	} else {
		wheelPos -= 170;
		return getScaledColor(getColor(0, wheelPos * 3, 255 - wheelPos * 3), brightness);
	}
}

void LedDriverBase::clearStrip() {
	clear();
}

void LedDriverBase::showStrip() {
	show();
}
