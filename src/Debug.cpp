/******************************************************************************
Debug.cpp
******************************************************************************/

#include "Debug.h"

Debug::Debug()
{
}

// Write screenbuffer to console.
void Debug::debugScreenBuffer(uint16_t screenBuffer[])
{
	const char buchstabensalat[][12] =
	{
		{ 'E', 'S', 'K', 'I', 'S', 'T', 'A', 'F', 'U', 'N', 'F', '1' },
		{ 'Z', 'E', 'H', 'N', 'Z', 'W', 'A', 'N', 'Z', 'I', 'G', '2' },
		{ 'D', 'R', 'E', 'I', 'V', 'I', 'E', 'R', 'T', 'E', 'L', '3' },
		{ 'V', 'O', 'R', 'F', 'U', 'N', 'K', 'N', 'A', 'C', 'H', '4' },
		{ 'H', 'A', 'L', 'B', 'A', 'E', 'L', 'F', 'U', 'N', 'F', 'A' },
		{ 'E', 'I', 'N', 'S', 'X', 'A', 'M', 'Z', 'W', 'E', 'I', ' ' },
		{ 'D', 'R', 'E', 'I', 'P', 'M', 'J', 'V', 'I', 'E', 'R', ' ' },
		{ 'S', 'E', 'C', 'H', 'S', 'N', 'L', 'A', 'C', 'H', 'T', ' ' },
		{ 'S', 'I', 'E', 'B', 'E', 'N', 'Z', 'W', 'O', 'L', 'F', ' ' },
		{ 'Z', 'E', 'H', 'N', 'E', 'U', 'N', 'K', 'U', 'H', 'R', ' ' }
	};
	//Serial.println("\033[0;0H"); // set cursor to 0, 0 position
	Serial.println(" -----------");
	for (uint8_t y = 0; y <= 9; y++)
	{
		Serial.print('|');
		for (uint8_t x = 0; x <= 10; x++)
		{
			Serial.print((bitRead(screenBuffer[y], 15 - x) ? buchstabensalat[y][x] : ' '));
		}
		Serial.print('|');
		Serial.println((bitRead(screenBuffer[y], 4) ? buchstabensalat[y][11] : ' '));
	}
	Serial.println(" -----------");
}

// Write time to console.
void Debug::debugTime(String label, time_t time, boolean showDate)
{
	if (showDate)
	{
		Serial.printf("%s %02d:%02d:%02d %02d.%02d.%04d\r\n", label.c_str(), hour(time), minute(time), second(time), day(time), month(time), year(time));
	}
	else
	{
		Serial.printf("%s %02d:%02d:%02d\r\n", label.c_str(), hour(time), minute(time), second(time));
	}
}

// Write FPS to console.
void Debug::debugFps()
{
	frames++;
	if (millis() - lastFpsCheck > 1000)
	{
		lastFpsCheck = millis();
		Serial.println("FPS: " + String(frames));
		frames = 0;
	}
}
