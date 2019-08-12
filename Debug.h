/******************************************************************************
Debug.h
******************************************************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include <TimeLib.h>

#define HIDE_DATE 0
#define SHOW_DATE 1

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#ifdef SYSLOG_SERVER 
#define SYSLOG(x) syslog.log(LOG_INFO, x);
#else
#define SYSLOG(x)
#endif

class Debug
{
public:
	Debug();
	void debugScreenBuffer(uint16_t screenBuffer[]);
	void debugTime(String label, time_t time, boolean showDate);
	void debugFps();

private:
	uint16_t frames = 0;
	uint32_t lastFpsCheck = 0;
};

#endif
