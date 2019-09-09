/******************************************************************************
 Settings.h
 ******************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Colors.h"
#include "Configuration.h"
#include "Debug.h"
#include "Languages.h"
#include "Modes.h"
#include "Layouts.h"
#include "Event.h"

#define SETTINGS_MAGIC_NUMBER 0x09
#define SETTINGS_VERSION 0

#define MOOD_LEVEL_MAX 9

#define NUM_EVTS 8
#define LEN_LOC_STR 30 
#define LEN_API_KEY 34 

class Settings {
public:
	Settings();

	uint8_t getLanguage();
	void setLanguage(uint8_t language);

	boolean getUseLdr();
	void setUseLdr(bool set);
	void toggleUseLdr();

	int16_t getBrightness();
	void setBrightness(int16_t brightness);

	uint8_t getColorNum();
	void setColorNum(uint8_t colorNum);

	uint32_t getColor();
	void setColor(uint32_t color);

	uint8_t getColorChange();
	void setColorChange(uint8_t colorChange);

	uint16_t getMoodRate();
	void setMoodRate(uint16_t moodRate);

	uint8_t getTransition();
	void setTransition(uint8_t transition);

	uint8_t getTimeout();
	void setTimeout(uint8_t timeout);

	uint8_t getEventDisplayRate();
	void setEventDisplayRate(uint8_t event);

	boolean getShowTemp();
	void setShowTemp(bool set);
	void toggleShowTemp();

	boolean getItIs();
	void setItIs(bool set);
	void toggleItIs();

	boolean getAlarm1();
	void setAlarm1(bool);
	void toggleAlarm1();
	time_t getAlarmTime1();
	void setAlarmTime1(time_t alarmTime);
	uint8_t getAlarm1Weekdays();
	void setAlarm1Weekdays(uint8_t alarmWeekdays);

	boolean getAlarm2();
	void setAlarm2(bool);
	void toggleAlarm2();
	time_t getAlarmTime2();
	void setAlarmTime2(time_t alarmTime);
	uint8_t getAlarm2Weekdays();
	void setAlarm2Weekdays(uint8_t alarmWeekdays);

	time_t getNightOffTime();
	void setNightOffTime(time_t nightOffTime);

	time_t getDayOnTime();
	void setDayOnTime(time_t dayOnTime);

	eLayout getLayout();
	void setLayout(eLayout layout);

	boolean getEventEnabled(uint8_t eventNum);
	void setEventEnabled(uint8_t eventNum, boolean set);
	time_t getEventDate(uint8_t eventNum);
	void setEventTime(uint8_t eventNum, time_t eventTime);
	uint8_t getEventAnimation(uint8_t eventNum);
	void setEventAnimation(uint8_t eventNum, uint8_t eventAnimation);
	void setEventText(uint8_t eventNum, char text[], uint8_t size);
	char* getEventText(uint8_t eventNum, char* eventText, uint8_t size);
	void setEventColor(uint8_t eventNum, eColor color);
	eColor getEventColor(uint8_t eventNum);
	event_t getEvent(uint8_t eventNum);

	void setLocation(char text[], uint8_t size);
	char* getLocation(char* locationText, uint8_t size);

  void setWeatherAPIKey(char text[], uint8_t size);
  char* getWeatherAPIKey(char* locationText, uint8_t size);

	void saveToEEPROM();
	void resetToDefault();

private:
	struct MySettings {
		uint8_t magicNumber;
		uint8_t version;
		uint8_t language;
		boolean useLdr;
		int16_t brightness;
		uint8_t colorNum;
		uint32_t color;
		uint8_t colorChange;
		uint8_t transition;
		uint8_t timeout;
		boolean showTemp;
		boolean itIs;
		boolean alarm1;
		time_t alarmTime1;
		uint8_t alarm1Weekdays;
		boolean alarm2;
		time_t alarmTime2;
		uint8_t alarm2Weekdays;
		time_t nightOffTime;
		time_t dayOnTime;
		uint16_t moodRate;
		eLayout layout;
		uint8_t eventDisplayRate;
		event_t events[NUM_EVTS];
		char location[LEN_LOC_STR];
    	char weather_api_key[LEN_API_KEY];
	} mySettings;

	void loadFromEEPROM();
};

#endif
