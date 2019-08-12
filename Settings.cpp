/******************************************************************************
 Settings.cpp
 ******************************************************************************/

#include "Settings.h"

Settings::Settings() {
	loadFromEEPROM();
}

uint8_t Settings::getLanguage() {
	return mySettings.language;
}

void Settings::setLanguage(uint8_t language) {
	mySettings.language = language;
}

boolean Settings::getUseLdr() {
	return mySettings.useLdr;
}

void Settings::setUseLdr(bool set) {
	mySettings.useLdr = set;
}

void Settings::toggleUseLdr() {
	mySettings.useLdr = !mySettings.useLdr;
}

int16_t Settings::getBrightness() {
	return mySettings.brightness;
}

void Settings::setBrightness(int16_t ledBrightness) {
	mySettings.brightness = ledBrightness;
}

uint8_t Settings::getColorNum() {
	return mySettings.colorNum;
}

void Settings::setColorNum(uint8_t colorNum) {
	mySettings.colorNum = colorNum;
	mySettings.color = getDefaultColor(colorNum);
}

uint32_t Settings::getColor() {
	return mySettings.color;
}

void Settings::setColor(uint32_t color) {
	mySettings.color = color;
	mySettings.colorNum = COLOR_COUNT;
}

uint8_t Settings::getColorChange() {
	return mySettings.colorChange;
}

void Settings::setColorChange(uint8_t colorChange) {
	mySettings.colorChange = colorChange;
}

uint16_t Settings::getMoodRate() {
	return mySettings.moodRate;
}
void Settings::setMoodRate(uint16_t moodRate) {
	mySettings.moodRate = moodRate;
}

uint8_t Settings::getTransition() {
	return mySettings.transition;
}

void Settings::setTransition(uint8_t transition) {
	mySettings.transition = transition;
}

boolean Settings::getShowTemp() {
	return mySettings.showTemp;
}

void Settings::setShowTemp(bool set) {
	mySettings.showTemp = set;
}

void Settings::toggleShowTemp() {
	mySettings.showTemp = !mySettings.showTemp;
}

uint8_t Settings::getTimeout() {
	return mySettings.timeout;
}

void Settings::setTimeout(uint8_t timeout) {
	mySettings.timeout = timeout;
}

uint8_t Settings::getEventDisplayRate() {
	return mySettings.eventDisplayRate;
}

void Settings::setEventDisplayRate(uint8_t eventDisplayRate) {
	mySettings.eventDisplayRate = eventDisplayRate;
}

boolean Settings::getItIs() {
	return mySettings.itIs;
}

void Settings::setItIs(bool set) {
	mySettings.itIs = set;
}

void Settings::toggleItIs() {
	mySettings.itIs = !mySettings.itIs;
}

boolean Settings::getAlarm1() {
	return mySettings.alarm1;
}

void Settings::setAlarm1(bool set) {
	mySettings.alarm1 = set;
}

void Settings::toggleAlarm1() {
	mySettings.alarm1 = !mySettings.alarm1;
}

time_t Settings::getAlarmTime1() {
	return mySettings.alarmTime1;
}

void Settings::setAlarmTime1(time_t alarmTime) {
	mySettings.alarmTime1 = alarmTime;
}

uint8_t Settings::getAlarm1Weekdays() {
	return mySettings.alarm1Weekdays;
}

void Settings::setAlarm1Weekdays(uint8_t alarmWeekdays) {
	mySettings.alarm1Weekdays = alarmWeekdays;
}

boolean Settings::getAlarm2() {
	return mySettings.alarm2;
}

void Settings::setAlarm2(bool set) {
	mySettings.alarm2 = set;
}

void Settings::toggleAlarm2() {
	mySettings.alarm2 = !mySettings.alarm2;
}

time_t Settings::getAlarmTime2() {
	return mySettings.alarmTime2;
}

void Settings::setAlarmTime2(time_t alarmTime) {
	mySettings.alarmTime2 = alarmTime;
}

uint8_t Settings::getAlarm2Weekdays() {
	return mySettings.alarm2Weekdays;
}

void Settings::setAlarm2Weekdays(uint8_t alarmWeekdays) {
	mySettings.alarm2Weekdays = alarmWeekdays;
}

time_t Settings::getNightOffTime() {
	return mySettings.nightOffTime;
}

void Settings::setNightOffTime(time_t nightOffTime) {
	mySettings.nightOffTime = nightOffTime;
}

time_t Settings::getDayOnTime() {
	return mySettings.dayOnTime;
}

void Settings::setDayOnTime(time_t dayOnTime) {
	mySettings.dayOnTime = dayOnTime;
}

eLayout Settings::getLayout() {
	return mySettings.layout;
}

void Settings::setLayout(eLayout layout) {
	mySettings.layout = layout;
}

boolean Settings::getEventEnabled(uint8_t eventNum) {
	return mySettings.events[eventNum].enabled;
}

void Settings::setEventEnabled(uint8_t eventNum, boolean set) {
	mySettings.events[eventNum].enabled = set;
}

uint8_t Settings::getEventAnimation(uint8_t eventNum) {
	return mySettings.events[eventNum].effect;
}

void Settings::setEventAnimation(uint8_t eventNum, uint8_t eventAnimation) {
	mySettings.events[eventNum].effect = (Effects::eEffects) eventAnimation;
}

time_t Settings::getEventDate(uint8_t eventNum) {
	return mySettings.events[eventNum].time;
}

void Settings::setEventTime(uint8_t eventNum, time_t eventTime) {
	mySettings.events[eventNum].time = eventTime;
}

void Settings::setEventText(uint8_t eventNum, char text[], uint8_t size) {
	memcpy(mySettings.events[eventNum].txt, text, _min(sizeof(mySettings.events[eventNum].txt), size));
}

char* Settings::getEventText(uint8_t eventNum, char* eventText, uint8_t size) {
	memset(eventText, 0, size);
	memcpy(eventText, mySettings.events[eventNum].txt, sizeof(mySettings.events[eventNum].txt));
	return eventText;
}

void Settings::setEventColor(uint8_t eventNum, eColor color) {
	mySettings.events[eventNum].color = color;
}

eColor Settings::getEventColor(uint8_t eventNum) {
	return mySettings.events[eventNum].color;
}

event_t Settings::getEvent(uint8_t eventNum) {
	return mySettings.events[eventNum];
}

void Settings::setLocation(char text[], uint8_t size) {
	memcpy(mySettings.location, text, _min(sizeof(mySettings.location), size));
}

char* Settings::getLocation(char* locationText, uint8_t size) {
	memset(locationText, 0, size);
	memcpy(locationText, mySettings.location, sizeof(mySettings.location));
	return locationText;
}

// Set all defaults.
void Settings::resetToDefault() {
	DEBUG_PRINTLN("*** Settings set to defaults in EEPROM. ***");
	mySettings.magicNumber = SETTINGS_MAGIC_NUMBER;
	mySettings.version = SETTINGS_VERSION;
#ifdef LANGUAGE_ENGLISH
	mySettings.language = LANGUAGE_EN;
#endif
#ifdef LANGUAGE_GERMAN
	mySettings.language = LANGUAGE_DE_DE;
#endif
#ifdef LANGUAGE_SPANISH
	mySettings.language = LANGUAGE_ES;
#endif
#ifdef LANGUAGE_FRENCH
	mySettings.language = LANGUAGE_FR;
#endif
#ifdef LANGUAGE_ITALIEN
	mySettings.language = LANGUAGE_IT;
#endif
#ifdef LANGUAGE_NETHERLANDS
	mySettings.language = LANGUAGE_NL;
#endif
	mySettings.useLdr = DEFAULT_USELDR;
	mySettings.brightness = DEFAULT_BRIGHTNESS;
	mySettings.colorNum = DEFAULT_COLOR;
	mySettings.color = getDefaultColor(mySettings.colorNum);
	mySettings.colorChange = DEFAULT_COLORCHANGE;
	mySettings.transition = DEFAULT_TRANSITION;
	mySettings.timeout = DEFAULT_TIMEOUT;
	mySettings.showTemp = DEFAULT_SHOWTEMP;
	mySettings.itIs = DEFAULT_SHOWITIS;
	mySettings.alarm1 = false;
	mySettings.alarmTime1 = 0;
	mySettings.alarm1Weekdays = 0;
	mySettings.alarm2 = false;
	mySettings.alarmTime2 = 0;
	mySettings.alarm2Weekdays = 0;
	mySettings.nightOffTime = 0;
	mySettings.dayOnTime = 0;
	mySettings.moodRate = 0;
	mySettings.layout = DEFAULT_LAYOUT;
	mySettings.eventDisplayRate = 0;
	for (uint8_t i = 0; i < NUM_EVTS; i++) {
		mySettings.events[i].enabled = false;
		mySettings.events[i].time = 0;
		memset(mySettings.events[i].txt, 0, sizeof(mySettings.events[i].txt));
		mySettings.events[i].effect = Effects::NO_EFFECT;
		mySettings.events[i].color = COLOR_WHITE;
	}
	memset(mySettings.location, 0, sizeof(mySettings.location));

	saveToEEPROM();
}

// Load settings from EEPROM.
void Settings::loadFromEEPROM() {
	DEBUG_PRINTLN("Settings loaded from EEPROM.");
	EEPROM.begin(512);
	EEPROM.get(0, mySettings);
	if ((mySettings.magicNumber != SETTINGS_MAGIC_NUMBER) || (mySettings.version != SETTINGS_VERSION))
		resetToDefault();
	EEPROM.end();
}

// Write settings to EEPROM.
void Settings::saveToEEPROM() {
	DEBUG_PRINTLN("Settings saved to EEPROM.");
	EEPROM.begin(512);
	EEPROM.put(0, mySettings);
	//EEPROM.commit();
	EEPROM.end();
}
