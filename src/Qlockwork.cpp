/******************************************************************************
QLOCKWORK
An advanced firmware for a DIY "word-clock".

@mc      ESP8266
@created 01.02.2017
******************************************************************************/

#define FIRMWARE_VERSION 20181124
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <ArduinoOTA.h>
#include <DS3232RTC.h>
#include <DHT.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Syslog.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <WiFiManager.h>
#include "Colors.h"
#include "Configuration.h"
#include "Debug.h"
#include "Event.h"
#include "Effects.h"
#include "Languages.h"
#include "LedDriver_FastLED.h"
#include "LedDriver_NeoPixel.h"
#include "LedDriver_LPD8806RGBW.h"
#include "Modes.h"
#include "Renderer.h"
#include "Settings.h"
#include "Timezones.h"
#include "TransitionFade.h"
#include "TransitionSlide.h"
#include "TransitionMatrix.h"
#ifdef BUTTONS
#include "Button.h"
#endif
#ifdef PIN_LDR
#include "LDR.h"
#endif

/******************************************************************************
Init.
******************************************************************************/
#define MILLIS_2_HZ 500

ESP8266WebServer esp8266WebServer(80);
ESP8266HTTPUpdateServer httpUpdater;
Renderer renderer;
Settings settings;
Debug debug;

#ifdef LED_LIBRARY_FASTLED
LedDriver_FastLED ledDriver(settings.getLayout());
#endif
#ifdef LED_LIBRARY_LPD8806RGBW
LedDriver_LPD8806RGBW ledDriver(settings.getLayout());
#endif
#ifdef LED_LIBRARY_NEOPIXEL
LedDriver_NeoPixel ledDriver(settings.getLayout());
#endif

#ifdef STATIC_IP
IPAddress ip(192, 168, 88, 88);
IPAddress gateway(192, 168, 88, 1);
IPAddress subnet(255, 255, 255, 0);
#endif // STATIC_IP

IPAddress myIP = { 0,0,0,0 };
char HostName[32];

uint16_t matrix[10] = {};
uint16_t matrixOld[10] = {};
bool screenBufferNeedsUpdate = true;
bool updateTransition = false;
Mode mode = STD_MODE_TIME;
Mode lastMode = mode;
uint32_t modeTimeout = 0;
uint32_t titleTimeout = 0;
uint32_t autoModeChange = AUTO_MODECHANGE_TIME;
uint8_t lastDay = 0;
uint8_t lastHour = 0;
uint8_t lastMinute = 0;
uint8_t lastFiveMinute = 0;
time_t lastTime = 0;
uint32_t lastMillis2Hz = 0;
uint32_t lastMillisMood = 0;
uint32_t lastMillisTransition = 0;
uint8_t randomHour = 0;
uint8_t randomMinute = 0;
uint8_t testColumn = 0;
uint8_t brightness = settings.getBrightness();
String effect = "";

// Weather conditions
String outdoorTitle = "";
int8_t outdoorTemperature = 0;
uint8_t outdoorHumidity = 0;
uint8_t outdoorCode = 0;
uint8_t errorCounterYahoo = 0;
char location[LEN_LOC_STR];
char weather_api_key[LEN_API_KEY];

// Update info
String updateInfo = "";

// Buzzer
#ifdef BUZZER
boolean timerSet = false;
time_t timer = 0;
uint8_t timerMinutes = 0;
uint8_t alarmOn = 0;
#endif

// LDR
#ifdef PIN_LDR
LDR ldr(PIN_LDR, LDR_IS_INVERSE, MIN_BRIGHTNESS, MAX_BRIGHTNESS, true, LDR_HYSTERESIS);
uint8_t ratedBrightness = 0;
uint32_t lastLdrCheck = 0;
#endif

// DHT22
float roomTemperature = 0;
float roomHumidity = 0;

#ifdef SENSOR_DHT22
DHT dht(PIN_DHT22, DHT22);
#endif

#ifdef RTC_BACKUP
DS3232RTC RTC;
#endif

// IR remote
#ifdef IR_REMOTE
IRrecv irrecv(PIN_IR_RECEIVER);
decode_results irDecodeResult;
#endif

// Syslog
#ifdef SYSLOG_SERVER
WiFiUDP Udp;
Syslog syslog(Udp, SYSLOG_SERVER, SYSLOG_PORT, PRODUCT_NAME, "QLOCKWIFIVE", LOG_KERN);
#endif

// Buttons
#ifdef BUTTONS
Button minusButton(PIN_BUTTON_MINUS, (byte)MINUS_BUTTON_PRESSING_AGAINST);
Button plusButton(PIN_BUTTON_PLUS, (byte)PLUS_BUTTON_PRESSING_AGAINST);
Button extModeDoubleButton(PIN_BUTTON_MINUS, PIN_BUTTON_PLUS, (byte)MINUS_BUTTON_PRESSING_AGAINST, (byte)PLUS_BUTTON_PRESSING_AGAINST);
Button modeButton(PIN_BUTTON_MODE, (byte)MODE_BUTTON_PRESSING_AGAINST);
#endif

// Transitions
bool transitionActive = false;
bool startTransition = false;
TransitionFade transitionFade((LedDriverBase*)&ledDriver);
TransitionSlide transitionSlide((LedDriverBase*)&ledDriver);
TransitionMatrix transitionMatrix((LedDriverBase*)&ledDriver);


/******************************************************************************
Function declarations
******************************************************************************/

// Basic functions
void setMode(Mode newMode);
time_t getRtcTime(void);
time_t getNtpTime(void);
void getUpdateInfo(void);
void setLedsOn();
void setLedsOff();
void setBrightnessFromLdr();
void setDisplayToToggle();
void setColor(uint32_t color);
void setColor(uint32_t color, bool disableCC);
void setColorNum(uint8_t colorNum);
void setColorNum(uint8_t colorNum, bool disableCC);
void showIP(IPAddress ipAddr);
void updateScreenBuffer(uint16_t screenBuffer[], uint32_t color, uint8_t brightness);

// Buttons
void buttonMinusPressed(void);
void buttonModePressed(void);
void buttonOnOffPressed(void);
void buttonPlusPressed(void);
void buttonSettingsPressed(void);
void buttonTimePressed(void);
void doubleExtModePressed(void);

// Webinterface
void setupWebServer();
void callRoot();
void callBack();
void handleButtonEvents();
void handleButtonSettings();
void handleCommitEvents();
void handleNotFound();
void handleReset();
void handleFactoryReset();
void handleWiFiReset();
void handleRoot();
void handleSelectMode();
void handleShowEffect();
void handleCommitSettings();

// Temperature & Weather
void getOutdoorConditions(String location);
void getRoomConditions();

// IR
void remoteAction(decode_results irDecodeResult);
void remoteFunctionTrigger(eMode functionMode, void (*func)());

/******************************************************************************
Setup().
******************************************************************************/

void setup()
{
	// Init serial port.
/************************************
 * Serielle darf bei QlockWiFive v1.2 nicht mehr aktiv sein, da RX/TX mit Tasten 
 * belegt sind. Muss also h�ndisch einkommentiert werden, wenn trotzdem gebraucht.
 */
	Serial.begin(SERIAL_SPEED);
	while (!Serial);

	// Here we go...
	DEBUG_PRINTLN();
	DEBUG_PRINTLN(PRODUCT_NAME);
	DEBUG_PRINTLN("Firmware: " + String(FIRMWARE_VERSION));
	DEBUG_PRINTLN("LED-Driver: " + ledDriver.getSignature());
	DEBUG_PRINTLN();

	// Init LED, Buzzer and LDR.

#ifdef PIN_LED
	DEBUG_PRINTLN("Setting up ESP-LED.");
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
#endif

#ifdef BUZZER
	DEBUG_PRINTLN("Setting up Buzzer.");
	pinMode(PIN_BUZZER, OUTPUT);
#endif

#ifdef SENSOR_DHT22
	DEBUG_PRINTLN("Setting up DHT22.");
	dht.begin();
#endif

#ifdef PIN_LDR
	DEBUG_PRINT("Setting up LDR. LDR is ");
	if (settings.getUseLdr()) {
		DEBUG_PRINTLN("enabled.");
	} else {
		DEBUG_PRINTLN("disabled.");
	}
	pinMode(PIN_LDR, INPUT);
	randomSeed(analogRead(PIN_LDR));
#endif

#ifdef SELFTEST
	renderer.setAllScreenBuffer(matrix);
	DEBUG_PRINTLN("Set all LEDs to white.");
	ledDriver.setBrightness(0x80);
	ledDriver.setScreenColor(getDefaultColor(COLOR_WHITE));
	ledDriver.setScreenBuffer(matrix);
	delay(2000);
	DEBUG_PRINTLN("Set all LEDs to red.");
	ledDriver.setBrightness(0xFF);
	ledDriver.setScreenColor(getDefaultColor(COLOR_RED), true);
	delay(2000);
	DEBUG_PRINTLN("Set all LEDs to green.");
	ledDriver.setScreenColor(getDefaultColor(COLOR_GREEN), true);
	delay(2000);
	DEBUG_PRINTLN("Set all LEDs to blue.");
	ledDriver.setScreenColor(getDefaultColor(COLOR_BLUE), true);
	delay(2000);
#endif

	// Start WiFi and services.

	renderer.clearScreenBuffer(matrix);
	ledDriver.setScreenBuffer(matrix);
	renderer.setSmallText("WI", TEXT_POS_TOP, matrix);
	renderer.setSmallText("FI", TEXT_POS_BOTTOM, matrix);
	ledDriver.setScreenColor(getDefaultColor(COLOR_WHITE));
	ledDriver.setScreenBuffer(matrix);
	WiFiManager wifiManager;
#ifdef WIFI_RESET
	wifiManager.resetSettings();
#endif

	memset(HostName, 0, sizeof(HostName));
	sprintf(HostName, "%s-%06X", PRODUCT_NAME, ESP.getChipId());

#ifdef STATIC_IP
	// Static IP Setup Info Here...
	wifiManager.setSTAStaticIPConfig(ip, gateway, subnet);
#endif

	wifiManager.setTimeout(WIFI_AP_TIMEOUT);
	wifiManager.autoConnect(HostName, WIFI_AP_PASS);
	if (WiFi.status() != WL_CONNECTED)
	{
		DEBUG_PRINTLN("WiFi not connected. Staying in AP mode.");
		WiFi.mode(WIFI_AP);
		ledDriver.setScreenColor(getDefaultColor(COLOR_RED), true);
		digitalWrite(PIN_BUZZER, HIGH);
		delay(1500);
		digitalWrite(PIN_BUZZER, LOW);
		delay(2000);
		myIP = WiFi.softAPIP();
	}
	else
	{
		WiFi.mode(WIFI_STA);
		Serial.println("WLAN connected. Switching to STA mode.");
		ledDriver.setScreenColor(getDefaultColor(COLOR_GREEN), true);
		for (uint8_t i = 0; i <= 2; i++)
		{
			digitalWrite(PIN_BUZZER, HIGH);
			delay(100);
			digitalWrite(PIN_BUZZER, LOW);
			delay(100);
		}
		delay(500);
		myIP = WiFi.localIP();
	}
#ifdef SHOW_IP
	showIP(myIP);
#endif
	renderer.clearScreenBuffer(matrix);

	SYSLOG(HostName);
	SYSLOG("Firmware: " + String(FIRMWARE_VERSION));
	SYSLOG("LED-Driver: " + ledDriver.getSignature());

	SYSLOG("LED-Layout: " + settings.getLayout());

	DEBUG_PRINTLN("Starting mDNS responder.");
	SYSLOG("Starting mDNS responder.");
	MDNS.begin(HostName);
	MDNS.addService("http", "tcp", 80);

	DEBUG_PRINTLN("Starting webserver on port 80.");
	SYSLOG("Starting webserver on port 80.");
	setupWebServer();

	DEBUG_PRINTLN("Starting updateserver on port 80.");
	SYSLOG("Starting updateserver on port 80.");
	httpUpdater.setup(&esp8266WebServer);

	DEBUG_PRINTLN("Starting OTA service.");
	SYSLOG("Starting OTA service.");
#ifdef SYSLOG_SERVER
	ArduinoOTA.onStart([]()
	{
		syslog.log(LOG_INFO, "Starting OTA.");
	});
	ArduinoOTA.onError([](ota_error_t error)
	{
		syslog.log(LOG_INFO, "OTA Error: " + String(error));
		if (error == OTA_AUTH_ERROR) syslog.log(LOG_INFO, "Auth failed.");
		else if (error == OTA_BEGIN_ERROR) syslog.log(LOG_INFO, "Begin failed.");
		else if (error == OTA_CONNECT_ERROR) syslog.log(LOG_INFO, "Connect failed.");
		else if (error == OTA_RECEIVE_ERROR) syslog.log(LOG_INFO, "Receive failed.");
		else if (error == OTA_END_ERROR) syslog.log(LOG_INFO, "End failed.");
	});
	ArduinoOTA.onEnd([]()
	{
		syslog.log(LOG_INFO, "End OTA.");
	});
#endif
	ArduinoOTA.setPassword(OTA_PASS);
	ArduinoOTA.begin();

#ifdef IR_REMOTE
	DEBUG_PRINTLN("Starting IR-receiver.");
	SYSLOG("Starting IR-receiver.");
	irrecv.enableIRIn();
#endif

	// Set some random values.
	randomSeed(analogRead(A0));

	randomHour = random(0, 24);
	DEBUG_PRINTLN("Random hour is: " + String(randomHour));
	SYSLOG("Random hour is: " + String(randomHour));

	randomMinute = random(1, 60);
	DEBUG_PRINTLN("Random minute is: " + String(randomMinute));
	SYSLOG("Random minute is: " + String(randomMinute));

	DEBUG_PRINTLN("Free RAM: " + String(system_get_free_heap_size()));
	SYSLOG("Free RAM: " + String(system_get_free_heap_size()));

	// Set ESP time at startup.
	// set timeprovider
	#ifdef RTC_BACKUP
		if (WiFi.status() == WL_CONNECTED) {
			DEBUG_PRINTLN("ESP is set from NTP with RTC backup.");
			setSyncProvider(getNtpTime);
			setSyncInterval(3600);
		}
		else {
			DEBUG_PRINTLN("ESP is set from RTC.");
			setSyncProvider(getRtcTime);
			setSyncInterval(3600);
		}
	#else
		if (WiFi.status() == WL_CONNECTED) {
			DEBUG_PRINTLN("ESP is set from NTP.");
			setSyncProvider(getNtpTime);
			setSyncInterval(3600);
		}
		else DEBUG_PRINTLN("Setting time failed. No provider found.");
	#endif

	// Get weatherinfo from the internet.
	settings.getLocation(location, sizeof(location));
	if (WiFi.status() == WL_CONNECTED) {
		getOutdoorConditions(String(location));
	}

#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	// Update room conditions.
	getRoomConditions();
#endif

	// Set some variables to startvalues.
	lastDay = day();
	lastHour = hour();
	lastFiveMinute = minute() / 5;
	lastMinute = minute();
	lastTime = now();
}

/******************************************************************************
Loop().
******************************************************************************/

void loop()
{
	// Run once a day.

	if (day() != lastDay)
	{
		lastDay = day();
		screenBufferNeedsUpdate = true;

		// Change color.
		if (settings.getColorChange() == COLORCHANGE_DAY)
		{
			setColorNum(random(0, COLOR_COUNT + 1), false);
			DEBUG_PRINTLN("Color changed to: " + String(settings.getColorNum()));
			SYSLOG("Color changed to: " + String(settings.getColorNum()));
		}
	}

	// Run once every hour.

	if (hour() != lastHour)
	{
		lastHour = hour();
		screenBufferNeedsUpdate = true;

		DEBUG_PRINTLN("Free RAM: " + String(system_get_free_heap_size()));
		SYSLOG("Free RAM: " + String(system_get_free_heap_size()));

		#if defined(UPDATE_INFO_STABLE) || defined(UPDATE_INFO_UNSTABLE)
		// Get updateinfo once a day at a random hour.
		if (hour() == randomHour)
		{
			if (WiFi.status() == WL_CONNECTED) getUpdateInfo();
		}
		#endif

		// Change color.
		if (settings.getColorChange() == COLORCHANGE_HOUR)
		{
			setColorNum(random(0, COLOR_COUNT + 1), false);
			DEBUG_PRINTLN("Color changed to: " + String(settings.getColorNum()));
			SYSLOG("Color changed to: " + String(settings.getColorNum()));
		}
	}

	// Run once every five minutes.

	if ((minute() / 5) != lastFiveMinute) {
		lastFiveMinute = minute() / 5;
		screenBufferNeedsUpdate = true;
		DEBUG_PRINTLN("Reached new five minutes.");
		if ((settings.getTransition() != TRANSITION_NORMAL) && (mode == STD_MODE_TIME)) {
			startTransition = true;
		}
		if (settings.getColorChange() == COLORCHANGE_FIVE){
			setColorNum(random(0, COLOR_COUNT - 1), false);
			DEBUG_PRINTLN("Color changed to: " + String(settings.getColorNum()));
			SYSLOG("Color changed to: " + String(settings.getColorNum()));
		}
	}

	// Run once every minute.

	if (minute() != lastMinute)
	{
		lastMinute = minute();
		screenBufferNeedsUpdate = true;

		#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
		// Update room conditions.
		getRoomConditions();
		#endif

		#ifdef DEBUG
			debug.debugTime("Time (ESP):", now(), SHOW_DATE);
		#endif

		if ((settings.getTransition() == TRANSITION_FADE) && (mode == STD_MODE_TIME)) {
			startTransition = true;
		}

		// Set nightmode/daymode.
		if ((hour() == hour(settings.getNightOffTime())) && (minute() == minute(settings.getNightOffTime())))
		{
			DEBUG_PRINTLN("Night off.");
			SYSLOG("Night off.");
			setLedsOff();
		}

		if ((mode == STD_MODE_BLANK) && (hour() == hour(settings.getDayOnTime())) && (minute() == minute(settings.getDayOnTime())))
		{
			DEBUG_PRINTLN("Day on.");
			SYSLOG("Day on.");
			setLedsOn();
		}

		// ************************************************************************
		// Run once every random minute.
		// ************************************************************************
		if (minute() == randomMinute) {
			// Get weather from Yahoo.
			if (WiFi.status() == WL_CONNECTED) {
				settings.getLocation(location, sizeof(location));
				getOutdoorConditions(String(location));
			}
		}
	}

	// Run once every second.

	if (now() != lastTime)
	{
		lastTime = now();

		if ((mode == STD_MODE_SECONDS) || (mode == EXT_MODE_TEST))
		{
			screenBufferNeedsUpdate = true;
		}

		if (mode == EXT_MODE_TEST)
		{
			testColumn++;
		}

		#ifdef PIN_LED
		// Flash ESP LED.
		if (digitalRead(PIN_LED) == LOW)
		{
			digitalWrite(PIN_LED, HIGH);
		}
		else
		{
			digitalWrite(PIN_LED, LOW);
		}
		#endif

		#ifdef BUZZER
		// Switch on alarm for alarm1.
		if (settings.getAlarm1() && (hour() == hour(settings.getAlarmTime1())) && (minute() == minute(settings.getAlarmTime1())) && (second() == 0) && bitRead(settings.getAlarm1Weekdays(), weekday()))
		{
			alarmOn = BUZZTIME_ALARM_1;
			DEBUG_PRINTLN("Alarm 1: on");
			SYSLOG("Alarm 1: on");
		}

		// Switch on alarm for alarm2.
		if (settings.getAlarm2() && (hour() == hour(settings.getAlarmTime2())) && (minute() == minute(settings.getAlarmTime2())) && (second() == 0) && bitRead(settings.getAlarm2Weekdays(), weekday()))
		{
			alarmOn = BUZZTIME_ALARM_2;
			DEBUG_PRINTLN("Alarm 2: on");
			SYSLOG("Alarm 2: on");
		}

		// Switch on alarm for timer.
		if (timerSet && (now() == timer))
		{
			setMode(STD_MODE_SET_TIMER);
			timerMinutes = 0;
			timerSet = false;
			alarmOn = BUZZTIME_TIMER;
			DEBUG_PRINTLN("Timeralarm: on");
			SYSLOG("Timeralarm: on");
		}

		// Make some noise.
		if (alarmOn)
		{
			alarmOn--;
			if (digitalRead(PIN_BUZZER) == LOW)
			{
				digitalWrite(PIN_BUZZER, HIGH);
			}
			else
			{
				digitalWrite(PIN_BUZZER, LOW);
			}
		}
		else
		{
			digitalWrite(PIN_BUZZER, LOW);
		}
		#endif

		// Show temperatures.
		if (settings.getShowTemp() && (mode == STD_MODE_TIME))
		{
			autoModeChange--;
			if (!autoModeChange)
			{
				autoModeChange = AUTO_MODECHANGE_TIME;
				setMode(STD_MODE_EXT_TEMP);
			}
		}
	}

	// millis overflow
	if (lastMillis2Hz > millis()){
		lastMillis2Hz = millis();
	}

	// execute every 0.5 second
	if ((lastMillis2Hz + MILLIS_2_HZ) < millis()){
		lastMillis2Hz = millis();
		switch (mode) {
			#ifdef PIN_LDR
			case EXT_MODE_LDR:
			#endif
			case EXT_MODE_EVT:
			#ifdef BUZZER
			case STD_MODE_SET_TIMER:
			case STD_MODE_ALARM_1:
			case STD_MODE_SET_ALARM_1:
			case STD_MODE_ALARM_2:
			case STD_MODE_SET_ALARM_2:
			#endif
			case EXT_MODE_COLOR:
			case EXT_MODE_COLORCHANGE:
			case EXT_MODE_MOOD_RATE:
			case EXT_MODE_TIMEOUT:
			case EXT_MODE_TRANSITION:
			case EXT_MODE_SHOW_TEMP:
			case EXT_MODE_LANGUAGE:
			case EXT_MODE_TIMESET:
			case EXT_MODE_IT_IS:
			case EXT_MODE_DAYSET:
			case EXT_MODE_MONTHSET:
			case EXT_MODE_YEARSET:
			case EXT_MODE_NIGHTOFF:
			case EXT_MODE_DAYON:
      		case EXT_MODE_EFFECT:
				screenBufferNeedsUpdate = true;
			break;
				default:
			break;
		}

	}

	// millis overflow
	if (lastMillisMood > millis()){
		lastMillisMood = millis();
	}
	if (settings.getColorChange() == COLORCHANGE_MOOD){
		if((lastMillisMood + MOOD_INTERVAL_MIN + ((MOOD_INTERVAL_MAX - MOOD_INTERVAL_MIN) * (MOOD_LEVEL_MAX - settings.getMoodRate()) / MOOD_LEVEL_MAX)) < millis()){
			lastMillisMood = millis();
			ledDriver.updateColorWheel(false);
			screenBufferNeedsUpdate = true;
		}
	}

if(transitionActive) {
	uint16_t updateInterval = 0xFFFF;
	switch(settings.getTransition()) {
		case TRANSITION_SLIDE:
		case TRANSITION_MATRIX:
			updateInterval = 120;
			break;
		case TRANSITION_FADE:
			updateInterval = 60;
			break;
		default:
			break;
		}
		// millis overflow
		if (lastMillisTransition > millis()) {
			lastMillisTransition = millis();
		}
		if ((lastMillisTransition + updateInterval) < millis()) {
			lastMillisTransition = millis();
			updateTransition = true;
			screenBufferNeedsUpdate = true;
		}
	}

	// Run always.

	// Call HTTP- and OTA-handle.
	esp8266WebServer.handleClient();
	ArduinoOTA.handle();

	#ifdef PIN_LDR
	// Set brigthness from LDR.
	setBrightnessFromLdr();
	#endif

	// Render a new screenbuffer if needed.
	if (screenBufferNeedsUpdate)
	{
		screenBufferNeedsUpdate = false;

		// Save old screenbuffer.
		for (uint8_t i = 0; i <= 9; i++)
		{
			matrixOld[i] = matrix[i];
		}

		renderer.clearScreenBuffer(matrix);

		// Clemens: Enable alarm symbol to show Wifi connectivity
		if (WiFi.status() == WL_CONNECTED) {
		renderer.setAlarmLed(matrix); 
		}
		else {
			renderer.deactivateAlarmLed(matrix);
		}


		// Render a new screenbuffer.
		switch (mode)
		{
    	// **** Mode Time ******
		case STD_MODE_TIME:
			for (byte evtID = 0; evtID < NUM_EVTS; evtID++) {
				if (settings.getEventEnabled(evtID) && (day() == day(settings.getEventDate(evtID))) && (month() == month(settings.getEventDate(evtID)))) {
					startTransition = false;
					event_t event = settings.getEvent(evtID);
					switch (settings.getEventDisplayRate()) {
						case 0:
							while (!(minute() % 5)) {
								Event::show(event);
							}
							break;
						case 1:
							while (!(minute() % 15)) {
								Event::show(event);
							}
							break;
						case 2:
							while (!(minute() % 30)) {
								Event::show(event);
							}
							break;
						case 3:
							while (!(minute() % 60)) {
								Event::show(event);
							}
							break;
						default:
							break;
					}
				}
			}
     
			renderer.setTime(hour(), minute(), settings.getLanguage(), matrix);
			renderer.setCorners(minute(), matrix);
			#ifdef BUZZER
			if (settings.getAlarm1() || settings.getAlarm2() || timerSet)
			{
				renderer.setAlarmLed(matrix);
			}
			#endif
			if (!settings.getItIs() && (minute() % 30))
			{
				renderer.clearEntryWords(settings.getLanguage(), matrix);
			}
			break;

	case EXT_MODE_TIMER:
		//time_t countdown = now()+now();
		break;

    case EXT_MODE_EFFECT:
      
      if(effect == "candle") {
        Effects::showCandle(COLOR_RED_25);
        Effects::showCandle(COLOR_RED_25);
      }
      else if (effect == "firework") {
        Effects::showFireWork(5, COLOR_RED);
        Effects::showFireWork(3, COLOR_GREEN);
        Effects::showFireWork(6, COLOR_BLUE);
        Effects::showFireWork(4, COLOR_MAGENTA);
        Effects::showFireWork(3, COLOR_ORANGE);
        Effects::showFireWork(5, COLOR_CYAN);
      }
      else if (effect == "heart") {
        Effects::showHeartBig(COLOR_RED);
		Effects::showHeartBig(COLOR_RED);
		Effects::showHeartBig(COLOR_RED);
		Effects::showHeartBig(COLOR_RED);
      }
	  else if (effect == "coffee") {
        Effects::showCoffee(COLOR_WHITE, 10);
      }
      else if (effect == "smile") {
		for(int n=0; n<6; ++n) {
        	Effects::showAnimatedBitmap(Effects::ANI_BITMAP_SMILEY_WINK, (eColor) COLOR_YELLOW);
		}
      }
	  else if (effect == "christmas") {
		for(int n=0; n<6; ++n) {
        	Effects::showAnimatedBitmap(Effects::ANI_BITMAP_CHRISTTREE, (eColor) COLOR_GREEN);
		}
      }
	  else if (effect == "champagne") {
		for(int n=0; n<6; ++n) {
        	Effects::showAnimatedBitmap(Effects::ANI_BITMAP_CHAMPGLASS, (eColor) COLOR_WHITE);
		}
      }
	  else if (effect == "countdown") {
        Effects::showAnimatedBitmap(Effects::ANI_COUNT, (eColor) COLOR_WHITE);
      }
      else {
		effect.replace( "ä", "ae" );
		effect.replace( "ü", "ue" );
		effect.replace( "ö", "oe" );
		effect.replace( "Ä", "AE" );
		effect.replace( "Ö", "OE" );
		effect.replace( "Ü", "UE" );
		effect.replace( "ß", "ss" );
        Effects::showTickerString(effect.c_str(), 5, COLOR_WHITE);
      }
      
      
      renderer.setTime(hour(), minute(), settings.getLanguage(), matrix);
      renderer.setCorners(minute(), matrix);
      mode = STD_MODE_TIME;
      break;

    	// **** Mode AM/PM ******
		case STD_MODE_AMPM:
			if (isAM())
			{
				renderer.setSmallText("AM", TEXT_POS_MIDDLE, matrix);
			}
			else
			{
				renderer.setSmallText("PM", TEXT_POS_MIDDLE, matrix);
			}
			break;
		case STD_MODE_SECONDS:
			renderer.setCorners(minute(), matrix);
			for (uint8_t i = 0; i <= 6; i++)
			{
				matrix[1 + i] |= numbersBig[second() / 10][i] << 11;
				matrix[1 + i] |= numbersBig[second() % 10][i] << 5;
			}
   
			break;



    	// **** Mode Weekday ******
		case STD_MODE_WEEKDAY:
			renderer.setSmallText(String(sWeekday[weekday()][0]) + String(sWeekday[weekday()][1]), TEXT_POS_MIDDLE, matrix);
			break;


    	// **** Mode Date ******
		case STD_MODE_DATE:
			if (day() < 10)
			{
				renderer.setSmallText(("0" + String(day())), TEXT_POS_TOP, matrix);
			}
			else
			{
				renderer.setSmallText(String(day()), TEXT_POS_TOP, matrix);
			}
			if (month() < 10)
			{
				renderer.setSmallText(("0" + String(month())), TEXT_POS_BOTTOM, matrix);
			}
			else
			{
				renderer.setSmallText(String(month()), TEXT_POS_BOTTOM, matrix);
			}
			renderer.setPixelInScreenBuffer(10, 4, matrix);
			renderer.setPixelInScreenBuffer(10, 9, matrix);
			break;


    	// **** Mode Title Temperature ******
		case STD_MODE_TITLE_TEMP:
			renderer.setSmallText("TE", TEXT_POS_TOP, matrix);
			renderer.setSmallText("MP", TEXT_POS_BOTTOM, matrix);
			break;


     	// **** Mode Temperature (internal) ******
		#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
		case STD_MODE_TEMP:
			renderer.clearScreenBuffer(matrix);
			if (roomTemperature == 0)
			{
				matrix[0] = 0b0000000001000000;
				matrix[1] = 0b0000000010100000;
				matrix[2] = 0b0000000010100000;
				matrix[3] = 0b0000000011100000;
			}
			if (roomTemperature > 0)
			{
				matrix[0] = 0b0000000001000000;
				matrix[1] = 0b0100000010100000;
				matrix[2] = 0b1110000010100000;
				matrix[3] = 0b0100000011100000;
			}
			if (roomTemperature < 0)
			{
				matrix[0] = 0b0000000001000000;
				matrix[1] = 0b0000000010100000;
				matrix[2] = 0b1110000010100000;
				matrix[3] = 0b0000000011100000;
			}
			renderer.setSmallText(String(int(abs(roomTemperature) + 0.5)), TEXT_POS_BOTTOM, matrix);
			break;
		#endif
		#ifdef SENSOR_DHT22
		case STD_MODE_HUMIDITY:
			renderer.clearScreenBuffer(matrix);
			renderer.setSmallText(String(int(((roomHumidity < 99)?roomHumidity:99) + 0.5)), TEXT_POS_TOP, matrix);
			matrix[6] = 0b0100100001000000;
			matrix[7] = 0b0001000010100000;
			matrix[8] = 0b0010000010100000;
			matrix[9] = 0b0100100011100000;
			break;
#endif



    	// **** Mode Temperature (External) ******
		case STD_MODE_EXT_TEMP:
			if (outdoorTemperature > 0)
			{
				matrix[1] = 0b0100000000000000;
				matrix[2] = 0b1110000000000000;
				matrix[3] = 0b0100000000000000;
			}
			if (outdoorTemperature < 0)
			{
				matrix[2] = 0b1110000000000000;
			}
			renderer.setSmallText(String(abs(outdoorTemperature)), TEXT_POS_BOTTOM, matrix);
			break;



    	// **** Mode Humidity (External) ******
		case STD_MODE_EXT_HUMIDITY:
			renderer.setSmallText(String((outdoorHumidity < 100)?outdoorHumidity:99), TEXT_POS_TOP, matrix);
			matrix[6] = 0b0100100000000000;
			matrix[7] = 0b0001000000000000;
			matrix[8] = 0b0010000000000000;
			matrix[9] = 0b0100100000000000;
			break;



      // **** Mode Title Alarm ******
#ifdef BUZZER
		case STD_MODE_TITLE_ALARM:
			renderer.setSmallText("AL", TEXT_POS_TOP, matrix);
			renderer.setSmallText("RM", TEXT_POS_BOTTOM, matrix);
			break;



      // **** Mode Set Timer ******
		case STD_MODE_SET_TIMER:
			renderer.setSmallText("TI", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0){
				renderer.deactivateAlarmLed(matrix);
				for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			}
			else
			{
				renderer.setAlarmLed(matrix);
				renderer.setSmallText(String(timerMinutes), TEXT_POS_BOTTOM, matrix);
			}
			break;


     // **** Mode Timer ******
		case STD_MODE_TIMER:
			renderer.setSmallText("TI", TEXT_POS_TOP, matrix);
			renderer.setSmallText(String((timer - now() + 60) / 60), TEXT_POS_BOTTOM, matrix);
			DEBUG_PRINTLN(String(timer - now()));
			break;



      // **** Mode Alarm 1 ******
		case STD_MODE_ALARM_1:
			renderer.setSmallText("A1", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getAlarm1())
				{
					renderer.setSmallText("EN", TEXT_POS_BOTTOM, matrix);
					renderer.setAlarmLed(matrix);
				}
				else
				{
					renderer.setSmallText("DA", TEXT_POS_BOTTOM, matrix);
				}
			}
			break;

		case STD_MODE_SET_ALARM_1:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				renderer.setTime(hour(settings.getAlarmTime1()), minute(settings.getAlarmTime1()), settings.getLanguage(), matrix);
				renderer.clearEntryWords(settings.getLanguage(), matrix);
				renderer.setAMPM(hour(settings.getAlarmTime1()), settings.getLanguage(), matrix);
				renderer.setAlarmLed(matrix);
			}
			break;


      // **** Mode Alarm 2 ******
		case STD_MODE_ALARM_2:
			renderer.setSmallText("A2", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getAlarm2())
				{
					renderer.setSmallText("EN", TEXT_POS_BOTTOM, matrix);
					renderer.setAlarmLed(matrix);
				}
				else
					renderer.setSmallText("DA", TEXT_POS_BOTTOM, matrix);
			}
			break;
		case STD_MODE_SET_ALARM_2:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				renderer.setTime(hour(settings.getAlarmTime2()), minute(settings.getAlarmTime2()), settings.getLanguage(), matrix);
				renderer.clearEntryWords(settings.getLanguage(), matrix);
				renderer.setAMPM(hour(settings.getAlarmTime2()), settings.getLanguage(), matrix);
				renderer.setAlarmLed(matrix);
			}
			break;
#endif


    // **** Mode Title Main ******
		case EXT_MODE_TITLE_MAIN:
			renderer.setSmallText("MA", TEXT_POS_TOP, matrix);
			renderer.setSmallText("IN", TEXT_POS_BOTTOM, matrix);
			break;
#ifdef PIN_LDR
		case EXT_MODE_LDR:
			renderer.setSmallText("LD", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getUseLdr())
				{
					renderer.setSmallText("EN", TEXT_POS_BOTTOM, matrix);
				}
				else
				{
					renderer.setSmallText("DA", TEXT_POS_BOTTOM, matrix);
				}
			}
			break;
#endif
		case EXT_MODE_BRIGHTNESS:
			for (uint8_t x = 0; x < map(settings.getBrightness(), 0, 255, 1, 10); x++)
			{
				for (uint8_t y = 0; y <= x; y++)
				{
					matrix[9 - y] |= 1 << (14 - x);
				}
			}
			break;
		case EXT_MODE_COLORCHANGE:
		renderer.setSmallText("CC", TEXT_POS_TOP, matrix);
		if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
		else {
			if (settings.getColorChange() == COLORCHANGE_NO) renderer.setSmallText("NO", TEXT_POS_BOTTOM, matrix);
			if (settings.getColorChange() == COLORCHANGE_MOOD) renderer.setSmallText("MD", TEXT_POS_BOTTOM, matrix);
			if (settings.getColorChange() == COLORCHANGE_FIVE) renderer.setSmallText("FI", TEXT_POS_BOTTOM, matrix);
			if (settings.getColorChange() == COLORCHANGE_HOUR) renderer.setSmallText("HR", TEXT_POS_BOTTOM, matrix);
			if (settings.getColorChange() == COLORCHANGE_DAY) renderer.setSmallText("DY", TEXT_POS_BOTTOM, matrix);
		}
		break;
		case EXT_MODE_MOOD_RATE:
			renderer.setSmallText("MR", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else renderer.setSmallText(String(settings.getMoodRate()), TEXT_POS_BOTTOM, matrix);
			break;
		case EXT_MODE_COLOR:
			matrix[0] = 0b0000000000010000;
			matrix[1] = 0b0000000000010000;
			matrix[2] = 0b0000000000010000;
			matrix[3] = 0b0000000000010000;
			matrix[4] = 0b0000000000000000;
			renderer.setSmallText("CO", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				renderer.setSmallText(String(settings.getColorNum()), TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_TRANSITION:
			renderer.setSmallText("TR", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getTransition() == TRANSITION_NORMAL) renderer.setSmallText("NO", TEXT_POS_BOTTOM, matrix);
				if (settings.getTransition() == TRANSITION_FADE) renderer.setSmallText("FD", TEXT_POS_BOTTOM, matrix);
				if (settings.getTransition() == TRANSITION_MATRIX) renderer.setSmallText("MX", TEXT_POS_BOTTOM, matrix);
				if (settings.getTransition() == TRANSITION_SLIDE) renderer.setSmallText("SD", TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_TIMEOUT:
			renderer.setSmallText("FB", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				renderer.setSmallText(String(settings.getTimeout()), TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_EVT:
			renderer.setSmallText("EV", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i < 10; i++) matrix[i] = 0;
			else if (settings.getEventDisplayRate() < EVT_REP_COUNT){
				renderer.setSmallText(sEvtRep[settings.getEventDisplayRate()], TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_SHOW_TEMP:
			renderer.setSmallText("ST", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getShowTemp())
				{
					renderer.setSmallText("EN", TEXT_POS_BOTTOM, matrix);
				}
				else
				{
					renderer.setSmallText("DA", TEXT_POS_BOTTOM, matrix);
				}
			}
			break;
		case EXT_MODE_LANGUAGE:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				if (sLanguage[settings.getLanguage()][3] == ' ')
				{
					renderer.setSmallText(String(sLanguage[settings.getLanguage()][0]) + String(sLanguage[settings.getLanguage()][1]), TEXT_POS_MIDDLE, matrix);
				}
				else
				{
					renderer.setSmallText(String(sLanguage[settings.getLanguage()][0]) + String(sLanguage[settings.getLanguage()][1]), TEXT_POS_TOP, matrix);
					renderer.setSmallText(String(sLanguage[settings.getLanguage()][2]) + String(sLanguage[settings.getLanguage()][3]), TEXT_POS_BOTTOM, matrix);
				}
			}
			break;
		case EXT_MODE_TITLE_TIME:
			renderer.setSmallText("TI", TEXT_POS_TOP, matrix);
			renderer.setSmallText("ME", TEXT_POS_BOTTOM, matrix);
			break;
		case EXT_MODE_TIMESET:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				renderer.setTime(hour(), minute(), settings.getLanguage(), matrix);
				renderer.setCorners(minute(), matrix);
				renderer.clearEntryWords(settings.getLanguage(), matrix);
				renderer.setAMPM(hour(), settings.getLanguage(), matrix);
			}
			break;
		case EXT_MODE_IT_IS:
			renderer.setSmallText("IT", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				if (settings.getItIs())
				{
					renderer.setSmallText("EN", TEXT_POS_BOTTOM, matrix);
				}
				else
				{
					renderer.setSmallText("DA", TEXT_POS_BOTTOM, matrix);
				}
			}
			break;
		case EXT_MODE_DAYSET:
			renderer.setSmallText("DD", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				renderer.setSmallText(String(day()), TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_MONTHSET:
			renderer.setSmallText("MM", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				renderer.setSmallText(String(month()), TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_YEARSET:
			renderer.setSmallText("YY", TEXT_POS_TOP, matrix);
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0) for (uint8_t i = 5; i <= 9; i++) matrix[i] = 0;
			else
			{
				renderer.setSmallText(String(year() % 100), TEXT_POS_BOTTOM, matrix);
			}
			break;
		case EXT_MODE_TEXT_NIGHTOFF:
			renderer.setSmallText("NT", TEXT_POS_TOP, matrix);
			renderer.setSmallText("OF", TEXT_POS_BOTTOM, matrix);
			break;
		case EXT_MODE_NIGHTOFF:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				renderer.setTime(hour(settings.getNightOffTime()), minute(settings.getNightOffTime()), settings.getLanguage(), matrix);
				renderer.clearEntryWords(settings.getLanguage(), matrix);
				renderer.setAMPM(hour(settings.getNightOffTime()), settings.getLanguage(), matrix);
			}
			break;
		case EXT_MODE_TEXT_DAYON:
			renderer.setSmallText("DY", TEXT_POS_TOP, matrix);
			renderer.setSmallText("ON", TEXT_POS_BOTTOM, matrix);
			break;
		case EXT_MODE_DAYON:
			if ((lastMillis2Hz/MILLIS_2_HZ) % 2 == 0)
			{
				renderer.setTime(hour(settings.getDayOnTime()), minute(settings.getDayOnTime()), settings.getLanguage(), matrix);
				renderer.clearEntryWords(settings.getLanguage(), matrix);
				renderer.setAMPM(hour(settings.getDayOnTime()), settings.getLanguage(), matrix);
			}
			break;
		case EXT_MODE_TITLE_TEST:
			renderer.setSmallText("TE", TEXT_POS_TOP, matrix);
			renderer.setSmallText("ST", TEXT_POS_BOTTOM, matrix);
			break;
		case EXT_MODE_TEST:
			if (testColumn >= 10) testColumn = 0;
			matrix[testColumn] = 0b1111111111110000;
			break;
		case STD_MODE_BLANK:
			break;
		default:
			break;
			}

#if defined(IR_LETTER_OFF)
		// Turn off LED behind IR-sensor.
		renderer.unsetPixelInScreenBuffer(7, 9, matrix);
#endif

#ifdef DEBUG_MATRIX
		//debug.debugScreenBuffer(matrixOld);
		debug.debugScreenBuffer(matrix);
#endif

		if(transitionActive) {
			if(updateTransition){
				uint8_t transitionState = 0;
				updateTransition = false;
				switch(settings.getTransition()) {
					case TRANSITION_FADE:
						transitionState = transitionFade.nextStep();
						break;
					case TRANSITION_SLIDE:
						transitionState = transitionSlide.nextStep();
						break;
					case TRANSITION_MATRIX:
						transitionState = transitionMatrix.nextStep();
						break;
						default:
						break;
				}
				switch(transitionState){
					case 0:
						break;
					case 1:
						if (settings.getColorChange() != COLORCHANGE_MOOD) {
							ledDriver.setScreenColor(settings.getColor());
						}
						break;
					case 2:
						transitionActive = false;
						startTransition = false;
						screenBufferNeedsUpdate = true;
						break;
					default:
						break;
				}
			}
		} else if (startTransition) {
			DEBUG_PRINTLN("startTransition");
			startTransition = false;
			transitionActive = true;
			lastMillisTransition = millis();
			switch(settings.getTransition()) {
				case TRANSITION_FADE:
					transitionFade.begin(matrixOld, matrix);
					break;
				case TRANSITION_SLIDE:
					transitionSlide.begin(matrixOld, matrix);
					break;
				case TRANSITION_MATRIX:
					transitionMatrix.begin(matrixOld, matrix);
					break;
				default:
          transitionActive = false;
					break;
			}
		} else {
			updateScreenBuffer(matrix, settings.getColor(), brightness);
		}
	}

	// Wait for mode timeout then switch back to time.
	if ((millis() > (modeTimeout + settings.getTimeout() * 1000)) && modeTimeout && (settings.getTimeout() > 0))
	{
		setMode(STD_MODE_TIME);
	}

	// Wait for title timeout then set next mode.
	switch (mode)
	{
	case STD_MODE_TITLE_TEMP:
#ifdef BUZZER
	case STD_MODE_TITLE_ALARM:
#endif
	case EXT_MODE_TITLE_MAIN:
	case EXT_MODE_TITLE_TIME:
	case EXT_MODE_TEXT_NIGHTOFF:
	case EXT_MODE_TEXT_DAYON:
	case EXT_MODE_TITLE_TEST:
		if (millis() > titleTimeout + 2000)
		{
			setMode(mode++);
		}
		break;
	default:
		break;
	}

#ifdef BUTTONS
	// M+ und H+ im STD_MODE_BLANK gedrueckt?
	if ((mode == STD_MODE_TIME) && extModeDoubleButton.pressed()) {
		doubleExtModePressed();
	}

	// Taste Minuten++ (brighness++) gedrueckt?
	if (plusButton.pressed()) {
		buttonPlusPressed();
	}

	// Taste Stunden++ (brightness--) gedrueckt?
	if (minusButton.pressed()) {
		buttonMinusPressed();
	}

	// Taste Moduswechsel gedrueckt?
	if (modeButton.pressed()) {
		buttonModePressed();
	}
#endif
#ifdef IR_REMOTE
	// Look for IR commands.
	if (irrecv.decode(&irDecodeResult))
	{
#ifdef DEBUG
		Serial.print("IR signal: ");
		serialPrintUint64(irDecodeResult.value, DEC);
		Serial.println();
#endif
		remoteAction(irDecodeResult);
		irrecv.resume();
    settings.saveToEEPROM();
	}
#endif

#ifdef DEBUG_FPS
	debug.debugFps();
#endif
}

/******************************************************************************
"M+" & "H+" gedrueckt im BLANK MODE
******************************************************************************/
#ifdef BUTTONS
void doubleExtModePressed() {
	DEBUG_PRINTLN(F("Minutes plus AND hours plus pressed in STD_MODE_BLANK..."));
	//DEBUG_FLUSH();
	while (minusButton.pressedRaw() && plusButton.pressedRaw()) {
	}
	setMode(EXT_MODE_START);
	DEBUG_PRINT(F("Entering EXT_MODEs, mode is now "));
	DEBUG_PRINT(mode);
	DEBUG_PRINTLN(F("..."));
	//DEBUG_FLUSH();
}
#endif

/******************************************************************************
"On/off" pressed.
******************************************************************************/

void buttonOnOffPressed()
{
	DEBUG_PRINTLN("On/off pressed.");
	SYSLOG("On/off pressed.");

#ifdef BUZZER
	// Switch off alarm.
	if (alarmOn)
	{
		DEBUG_PRINTLN("Alarm off.");
		SYSLOG("Alarm off.");
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
		setMode(STD_MODE_TIME);
	}
#endif

	setDisplayToToggle();
}

/******************************************************************************
"Mode" pressed.
******************************************************************************/

void buttonModePressed()
{
	titleTimeout = millis();

#ifdef BUZZER
	// Switch off alarm.
	if (alarmOn)
	{
		DEBUG_PRINTLN("Alarm off.");
		SYSLOG("Alarm off.");
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
		setMode(STD_MODE_TIME);
		return;
	}
#endif

	// Set mode.
	setMode(mode++);
	switch (mode)
	{
	case STD_MODE_COUNT:
	case EXT_MODE_COUNT:
		setMode(STD_MODE_TIME);
		break;
	case EXT_MODE_COLOR:
		if (settings.getColorChange())
			setMode(mode++);
		break;
#ifdef BUZZER
	case STD_MODE_SET_TIMER:
		if (timerSet)
			setMode(mode++);
		break;
	case STD_MODE_TIMER:
		if (!timerSet)
			setMode(mode++);
		break;
	case STD_MODE_SET_ALARM_1:
		if (!settings.getAlarm1())
			setMode(mode++);
		break;
	case STD_MODE_SET_ALARM_2:
		if (!settings.getAlarm2())
			setMode(STD_MODE_TIME);
		break;
#endif
#ifdef PIN_LDR
	case EXT_MODE_BRIGHTNESS:
		if (settings.getUseLdr())
			setMode(mode++);
		break;
#endif
	case EXT_MODE_TEST:
		testColumn = 0;
		return;
	default:
		break;
	}

#ifdef RTC_BACKUP
	RTC.set(now());
#endif

	settings.saveToEEPROM();
}

/******************************************************************************
"Settings" pressed.
******************************************************************************/

void buttonSettingsPressed()
{
	DEBUG_PRINTLN("Settings pressed.");
	SYSLOG("Settings pressed.");
	titleTimeout = millis();

#ifdef BUZZER
	// Switch off alarm.
	if (alarmOn)
	{
		DEBUG_PRINTLN("Alarm off.");
		SYSLOG("Alarm off.");
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
		setMode(STD_MODE_TIME);
		return;
	}
#endif

	if (mode < EXT_MODE_START)
	{
		setMode(EXT_MODE_START);
	}
	else
	{
		buttonModePressed();
	}
}

/******************************************************************************
"Time" pressed.
******************************************************************************/

void buttonTimePressed()
{
	DEBUG_PRINTLN("Time pressed.");
	SYSLOG("Time pressed.");

	#ifdef BUZZER
		// Switch off alarm.
		if (alarmOn)
		{
			DEBUG_PRINTLN("Alarm off.");
			SYSLOG("Alarm off.");
			digitalWrite(PIN_BUZZER, LOW);
			alarmOn = false;
			setMode(STD_MODE_TIME);
			return;
		}
	#endif

	#ifdef RTC_BACKUP
		RTC.set(now());
	#endif

	settings.saveToEEPROM();
	modeTimeout = 0;
	renderer.clearScreenBuffer(matrix);
	setMode(STD_MODE_TIME);
}

/******************************************************************************
"Plus" pressed.
******************************************************************************/

void buttonPlusPressed()
{
	DEBUG_PRINTLN("Plus pressed.");
	SYSLOG("Plus pressed.");
	screenBufferNeedsUpdate = true;
	titleTimeout = millis();

	#ifdef BUZZER
		// Switch off alarm.
		if (alarmOn)
		{
			DEBUG_PRINTLN("Alarm off.");
			SYSLOG("Alarm off.");
			digitalWrite(PIN_BUZZER, LOW);
			alarmOn = false;
			return;
		}
	#endif

	switch (mode)
	{
	case STD_MODE_TIME:
		setMode(STD_MODE_TITLE_TEMP);
		break;
	#ifdef BUZZER
		case STD_MODE_TITLE_TEMP:
			setMode(STD_MODE_TITLE_ALARM);
			break;
		case STD_MODE_TITLE_ALARM:
			setMode(STD_MODE_TIME);
			break;
		case STD_MODE_SET_TIMER:
			if (timerMinutes < 100)
				timerMinutes++;
			timer = now() + timerMinutes * 60;
			timerSet = true;
			break;
		case STD_MODE_TIMER:
			timerSet = false;
			setMode(STD_MODE_SET_TIMER);
			break;
		case STD_MODE_ALARM_1:
			settings.toggleAlarm1();
			break;
		case STD_MODE_SET_ALARM_1:
			settings.setAlarmTime1(settings.getAlarmTime1() + 3600);
	#ifdef DEBUG
			debug.debugTime("Alarm 1:", settings.getAlarmTime1(), HIDE_DATE);
	#endif
			break;
		case STD_MODE_ALARM_2:
			settings.toggleAlarm2();
			break;
		case STD_MODE_SET_ALARM_2:
			settings.setAlarmTime2(settings.getAlarmTime2() + 3600);
	#ifdef DEBUG
			debug.debugTime("Alarm 2:", settings.getAlarmTime2(), HIDE_DATE);
	#endif
	#endif
		break;
	case EXT_MODE_TITLE_MAIN:
		setMode(EXT_MODE_TITLE_TIME);
		break;
	#ifdef PIN_LDR
		case EXT_MODE_LDR:
			settings.toggleUseLdr();
			if (!settings.getUseLdr()) brightness = settings.getBrightness();
			break;
	#endif
	case EXT_MODE_BRIGHTNESS:
		settings.setBrightness(constrain(settings.getBrightness() + map(1, 0, 9, 0, 255), MIN_BRIGHTNESS, MAX_BRIGHTNESS));
		brightness = settings.getBrightness();
		DEBUG_PRINTLN(settings.getBrightness());
		break;
	case EXT_MODE_COLORCHANGE:
		if (settings.getColorChange() < COLORCHANGE_COUNT)
			settings.setColorChange(settings.getColorChange() + 1);
		else
			settings.setColorChange(0);
		break;
	case EXT_MODE_MOOD_RATE:
		if (settings.getMoodRate() < MOOD_LEVEL_MAX)
			settings.setMoodRate(settings.getMoodRate() + 1);
		else
			settings.setMoodRate(0);
		break;
	case EXT_MODE_COLOR:
		if (settings.getColorNum() < COLOR_COUNT)
			settings.setColorNum(settings.getColorNum() + 1);
		else
			settings.setColorNum(0);
		break;
	case EXT_MODE_TRANSITION:
		if (settings.getTransition() < TRANSITION_COUNT)
			settings.setTransition(settings.getTransition() + 1);
		else
			settings.setTransition(0);
		break;
	case EXT_MODE_TIMEOUT:
		if (settings.getTimeout() < 99)
			settings.setTimeout(settings.getTimeout() + 1);
		break;
	case EXT_MODE_EVT:
		if (settings.getEventDisplayRate() < EVT_REP_COUNT - 1) settings.setEventDisplayRate(settings.getEventDisplayRate() + 1);
		else settings.setEventDisplayRate(0);
		break;
	case EXT_MODE_SHOW_TEMP:
		settings.toggleShowTemp();
		break;
	case EXT_MODE_LANGUAGE:
		if (settings.getLanguage() < LANGUAGE_COUNT)
			settings.setLanguage(settings.getLanguage() + 1);
		else
			settings.setLanguage(0);
		break;
	case EXT_MODE_TITLE_TIME:
		setMode(EXT_MODE_TITLE_TEST);
		break;
	case EXT_MODE_TIMESET:
		setTime(hour() + 1, minute(), second(), day(), month(), year());
	#ifdef DEBUG
			debug.debugTime("Time set:", now(), SHOW_DATE);
	#endif
		break;
	case EXT_MODE_IT_IS:
		settings.toggleItIs();
		break;
	case EXT_MODE_DAYSET:
		setTime(hour(), minute(), second(), day() + 1, month(), year());
		break;
	case EXT_MODE_MONTHSET:
		setTime(hour(), minute(), second(), day(), month() + 1, year());
		break;
	case EXT_MODE_YEARSET:
		setTime(hour(), minute(), second(), day(), month(), year() + 1);
		break;
	case EXT_MODE_TEXT_NIGHTOFF:
		setMode(EXT_MODE_NIGHTOFF);
		break;
	case EXT_MODE_NIGHTOFF:
		settings.setNightOffTime(settings.getNightOffTime() + 3600);
	#ifdef DEBUG
			debug.debugTime("Night off:", settings.getNightOffTime(), HIDE_DATE);
	#endif
		break;
	case EXT_MODE_TEXT_DAYON:
		setMode(EXT_MODE_DAYON);
		break;
	case EXT_MODE_DAYON:
		settings.setDayOnTime(settings.getDayOnTime() + 3600);
	#ifdef DEBUG
			debug.debugTime("Day on:", settings.getDayOnTime(), HIDE_DATE);
	#endif
		break;
	case EXT_MODE_TITLE_TEST:
		setMode(EXT_MODE_TITLE_MAIN);
		break;
	default:
		break;
	}
}

/******************************************************************************
"Minus" pressed
******************************************************************************/

void buttonMinusPressed()
{
	DEBUG_PRINTLN("Minus pressed.");
	SYSLOG("Minus pressed.");
	screenBufferNeedsUpdate = true;
	titleTimeout = millis();

#ifdef BUZZER
	// Switch off alarm.
	if (alarmOn)
	{
		DEBUG_PRINTLN("Alarm off.");
		SYSLOG("Alarm off.");
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
		return;
	}
#endif

	switch (mode)
	{
	case STD_MODE_TITLE_TEMP:
		setMode(STD_MODE_TIME);
		break;
#ifdef BUZZER
	case STD_MODE_TITLE_ALARM:
		setMode(STD_MODE_TITLE_TEMP);
		break;
	case STD_MODE_TIME:
		setMode(STD_MODE_TITLE_ALARM);
		break;
	case STD_MODE_SET_TIMER:
		if (timerMinutes > 0)
		{
			timerMinutes--;
			if (timerMinutes == 0)
				timerSet = false;
			else
			{
				timer = now() + timerMinutes * 60;
				timerSet = true;
			}
		}
		break;
	case STD_MODE_TIMER:
		timerSet = false;
		setMode(STD_MODE_SET_TIMER);
		break;
	case STD_MODE_ALARM_1:
		settings.toggleAlarm1();
		break;
	case STD_MODE_SET_ALARM_1:
		settings.setAlarmTime1(settings.getAlarmTime1() + 300);
#ifdef DEBUG
		debug.debugTime("Alarm 1:", settings.getAlarmTime1(), HIDE_DATE);
#endif
		break;
	case STD_MODE_ALARM_2:
		settings.toggleAlarm2();
		break;
	case STD_MODE_SET_ALARM_2:
		settings.setAlarmTime2(settings.getAlarmTime2() + 300);
#ifdef DEBUG
		debug.debugTime("Alarm 2:", settings.getAlarmTime2(), HIDE_DATE);
#endif
#endif
		break;
	case EXT_MODE_TITLE_MAIN:
		setMode(EXT_MODE_TITLE_TEST);
		break;
#ifdef PIN_LDR
	case EXT_MODE_LDR:
		settings.toggleUseLdr();
		if (!settings.getUseLdr()) brightness = settings.getBrightness();
		break;
#endif
	case EXT_MODE_BRIGHTNESS:
		settings.setBrightness(constrain(settings.getBrightness() - map(1, 0, 9, 0, 255), MIN_BRIGHTNESS, MAX_BRIGHTNESS));
		brightness = settings.getBrightness();
		DEBUG_PRINTLN(settings.getBrightness());
		break;
	case EXT_MODE_COLORCHANGE:
		if (settings.getColorChange() > 0)
			settings.setColorChange(settings.getColorChange() - 1);
		else
			settings.setColorChange(COLORCHANGE_COUNT);
		break;
	case EXT_MODE_MOOD_RATE:
		if (settings.getMoodRate() > 0)
			settings.setMoodRate(settings.getMoodRate() - 1);
		else
			settings.setMoodRate(MOOD_LEVEL_MAX);
		break;
	case EXT_MODE_COLOR:
		if (settings.getColorNum() > 0)
			settings.setColorNum(settings.getColorNum() - 1);
		else
			settings.setColorNum(COLOR_COUNT);
		break;
	case EXT_MODE_TRANSITION:
		if (settings.getTransition() > 0)
			settings.setTransition(settings.getTransition() - 1);
		else
			settings.setTransition(TRANSITION_COUNT);
		break;
	case EXT_MODE_TIMEOUT:
		if (settings.getTimeout() > 0)
			settings.setTimeout(settings.getTimeout() - 1);
		break;
	case EXT_MODE_EVT:
		if (settings.getEventDisplayRate() > 0) settings.setEventDisplayRate(settings.getEventDisplayRate() - 1);
		else settings.setEventDisplayRate(EVT_REP_COUNT - 1);
		break;
	case EXT_MODE_SHOW_TEMP:
		settings.toggleShowTemp();
		break;
	case EXT_MODE_LANGUAGE:
		if (settings.getLanguage() > 0)
			settings.setLanguage(settings.getLanguage() - 1);
		else
			settings.setLanguage(LANGUAGE_COUNT);
		break;
	case EXT_MODE_TITLE_TIME:
		setMode(EXT_MODE_TITLE_MAIN);
		break;
	case EXT_MODE_TIMESET:
		setTime(hour(), minute() + 1, 0, day(), month(), year());
#ifdef DEBUG
		debug.debugTime("Time set:", now(), HIDE_DATE);
#endif
		break;
	case EXT_MODE_IT_IS:
		settings.toggleItIs();
		break;
	case EXT_MODE_DAYSET:
		setTime(hour(), minute(), second(), day() - 1, month(), year());
		break;
	case EXT_MODE_MONTHSET:
		setTime(hour(), minute(), second(), day(), month() - 1, year());
		break;
	case EXT_MODE_YEARSET:
		setTime(hour(), minute(), second(), day(), month(), year() - 1);
		break;
	case EXT_MODE_TEXT_NIGHTOFF:
		setMode(EXT_MODE_NIGHTOFF);
		break;
	case EXT_MODE_NIGHTOFF:
		settings.setNightOffTime(settings.getNightOffTime() + 300);
#ifdef DEBUG
		debug.debugTime("Night off:", settings.getNightOffTime(), HIDE_DATE);
#endif
		break;
	case EXT_MODE_TEXT_DAYON:
		setMode(EXT_MODE_DAYON);
		break;
	case EXT_MODE_DAYON:
		settings.setDayOnTime(settings.getDayOnTime() + 300);
#ifdef DEBUG
		debug.debugTime("Day on:", settings.getDayOnTime(), HIDE_DATE);
#endif
		break;
	case EXT_MODE_TITLE_TEST:
		setMode(EXT_MODE_TITLE_TIME);
		break;
	default:
		break;
	}
}

#ifdef IR_REMOTE
/******************************************************************************
IR-signal received.
******************************************************************************/

void remoteAction(decode_results irDecodeResult)
{
	screenBufferNeedsUpdate = true;
	switch (irDecodeResult.value)
	{
	case IR_CODE_ONOFF:
		buttonOnOffPressed();
		break;
	case IR_CODE_TIME:
		buttonTimePressed();
		break;
	case IR_CODE_MODE:
		buttonModePressed();
		break;
	case IR_CODE_SETTINGS:
		buttonSettingsPressed();
		break;
	case IR_CODE_PLUS:
		if(!settings.getUseLdr() && (mode == STD_MODE_TIME)){
			remoteFunctionTrigger(EXT_MODE_BRIGHTNESS, buttonPlusPressed);
		} else {
			buttonPlusPressed();
		}
		break;
	case IR_CODE_MINUS:
		if(!settings.getUseLdr() && (mode == STD_MODE_TIME)){
			remoteFunctionTrigger(EXT_MODE_BRIGHTNESS, buttonMinusPressed);
		} else {
			buttonMinusPressed();
		}
		break;
	case IR_CODE_RED:
		setColorNum(COLOR_RED);
		break;
	case IR_CODE_GREEN:
		setColorNum(COLOR_GREEN);
		break;
	case IR_CODE_DARKBLUE:
		setColorNum(COLOR_BLUE);
		break;
	case IR_CODE_PINK:
		setColorNum(COLOR_PINK);
		break;
	case IR_CODE_WHITE:
		setColorNum(COLOR_WHITE);
		break;
	case IR_CODE_BRIGHTBLUE:
		setColorNum(COLOR_CYAN);
		break;
	case IR_CODE_YELLOW:
		setColorNum(COLOR_YELLOW);
		break;

#ifdef IR_CODE_RED_50
	case IR_CODE_RED_50:
		setColorNum(COLOR_RED_50);
		break;
#endif
#ifdef IR_CODE_RED_25
	case IR_CODE_RED_25:
		setColorNum(COLOR_RED_25);
		break;
#endif
#ifdef IR_CODE_ORANGE
	case IR_CODE_ORANGE:
		setColorNum(COLOR_ORANGE);
		break;
#endif
#ifdef IR_CODE_MINT
	case IR_CODE_MINT:
		setColorNum(COLOR_MINTGREEN);
		break;
#endif
#ifdef IR_CODE_BLUE_25
	case IR_CODE_BLUE_25:
		setColorNum(COLOR_BLUE_25);
		break;
#endif
#ifdef IR_CODE_CYAN_25
	case IR_CODE_CYAN_25:
		setColorNum(COLOR_CYAN_25);
		break;
#endif
#ifdef IR_CODE_BLUE_50
	case IR_CODE_BLUE_50:
		setColorNum(COLOR_BLUE_50);
		break;
#endif
#ifdef IR_CODE_MAGENTA_25
	case IR_CODE_MAGENTA_25:
		setColorNum(COLOR_MAGENTA_25);
		break;
#endif

#ifdef IR_CODE_MOOD
  case IR_CODE_MOOD:
    settings.setColorChange(COLORCHANGE_MOOD);
    break;
#endif

#ifdef IR_CODE_5MIN
  case IR_CODE_5MIN:
    settings.setColorChange(COLORCHANGE_FIVE);
    break;
#endif

#ifdef IR_CODE_1H
  case IR_CODE_1H:
    settings.setColorChange(COLORCHANGE_HOUR);
    break;
#endif

#ifdef IR_CODE_24H
  case IR_CODE_24H:
    settings.setColorChange(COLORCHANGE_DAY);
    break;
#endif

#ifdef IR_CODE_DATE
  case IR_CODE_DATE:
    remoteFunctionTrigger(STD_MODE_DATE, NULL);
    break;
#endif

	case IR_CODE_RGB:
		remoteFunctionTrigger(EXT_MODE_COLORCHANGE, buttonPlusPressed);
		break;
	case IR_CODE_NORMAL:
		settings.setTransition(TRANSITION_NORMAL);
		break;
	case IR_CODE_FADE:
		settings.setTransition(TRANSITION_FADE);
		transitionActive = false;
		startTransition = true;
		break;
	case IR_CODE_MATRIX:
		settings.setTransition(TRANSITION_MATRIX);
		transitionActive = false;
		startTransition = true;
		break;
	case IR_CODE_SLIDE:
		settings.setTransition(TRANSITION_SLIDE);
		transitionActive = false;
		startTransition = true;
		break;
	case IR_CODE_SECONDS:
		remoteFunctionTrigger(STD_MODE_SECONDS, NULL);
		break;
	case IR_CODE_HR_PLUS:
		if(mode == STD_MODE_TIME){
			setTime(hour() + 1, minute(), second(), day(), month(), year());
		}
		break;
	case IR_CODE_HR_MINUS:
		if(mode == STD_MODE_TIME){
			setTime(hour() - 1, minute(), second(), day(), month(), year());
		}
		break;
	case IR_CODE_MIN_PLUS:
		if(mode == STD_MODE_TIME){
			setTime(hour(), minute() + 1, 0, day(), month(), year());
		}
		break;
	case IR_CODE_MIN_MINUS:
		if(mode == STD_MODE_TIME){
			setTime(hour(), minute() - 1, 0, day(), month(), year());
		}
		break;
	case IR_CODE_LDR:
#ifdef PIN_LDR
		remoteFunctionTrigger(EXT_MODE_LDR, buttonPlusPressed);
		modeTimeout = millis();
#endif
		break;

	default:
		break;
	}
}
#endif

/******************************************************************************
Transitions.
******************************************************************************/

void updateScreenBuffer(uint16_t screenBuffer[], uint32_t color, uint8_t brightness) {
	ledDriver.setBrightness(brightness);
	if (settings.getColorChange() != COLORCHANGE_MOOD) {
		ledDriver.setScreenColor(color);
	}
	ledDriver.setScreenBuffer(screenBuffer);
}

void remoteFunctionTrigger(eMode functionMode, void (*func)()){
	if(mode != functionMode){
		setMode(functionMode);
    modeTimeout = millis();
	} else if (func != NULL){
		func();
		modeTimeout = millis();
	}
}

void showIP(IPAddress ipAddr){
	String ip0 = String(ipAddr[0]);
	String ip1 = String(ipAddr[1]);
	String ip2 = String(ipAddr[2]);
	String ip3 = String(ipAddr[3]);
	String ipString = "IP: " + ip0 + "." + ip1 + "." + ip2 + "." + ip3;
	Effects::showTickerString(ipString.c_str(), 5, COLOR_WHITE);
}

#ifdef PIN_LDR
/******************************************************************************
Set brightness from LDR.
******************************************************************************/

void setBrightnessFromLdr()
{
	if (settings.getUseLdr()) {
		if (millis() < lastLdrCheck) {
			// overflow...
			lastLdrCheck = millis();
		}
		if (millis() > (lastLdrCheck + 100)) {
			// get rated brightness from LDR
			uint8_t ratedBrightness = ldr.value();
			// set brightness to rated brightness
			lastLdrCheck = millis();
			if (brightness < ratedBrightness) brightness++;
			if (brightness > ratedBrightness) brightness--;
			if (brightness != ratedBrightness) {
				screenBufferNeedsUpdate = true;
				//DEBUG_PRINTLN("Brightness: " + String(brightness) + ", rated brightness: " + String(ratedBrightness));
			}
		}
	}
}
#endif

#if defined(UPDATE_INFO_STABLE) || defined(UPDATE_INFO_UNSTABLE)
/******************************************************************************
  Get update info.
******************************************************************************/

void getUpdateInfo()
{
#ifdef DEBUG
	Serial.println("Sending HTTP-request for update info.");
#endif
	char server[] = UPDATE_INFOSERVER;
	WiFiClient wifiClient;
	HttpClient client = HttpClient(wifiClient, server, 80);
	client.get(UPDATE_INFOFILE);
	uint16_t statusCode = client.responseStatusCode();
	if (statusCode == 200)
	{
		String response = client.responseBody();
#ifdef DEBUG
		Serial.printf("Status: %u\r\n", statusCode);
		Serial.printf("Response is %u bytes.\r\n", response.length());
		Serial.println(response);
		Serial.println("Parsing JSON.");
#endif
		//DynamicJsonBuffer jsonBuffer;
		StaticJsonBuffer<256> jsonBuffer;
		JsonObject &responseJson = jsonBuffer.parseObject(response);
		if (responseJson.success())
		{
    #ifdef UPDATE_INFO_STABLE
    			updateInfo = responseJson["channel"]["stable"]["version"].as<String>();
    #endif
    #ifdef UPDATE_INFO_UNSTABLE
    			updateInfo = responseJson["channel"]["unstable"]["version"].as<String>();
    #endif
			return;
		}
	}
#ifdef DEBUG
	else Serial.printf("Status: %u\r\n", statusCode);
	Serial.println("Error (" + String(UPDATE_INFOSERVER) + ")");
#endif
}
#endif

/******************************************************************************
  Get outdoor conditions
******************************************************************************/

void getOutdoorConditions(String location) {
#ifdef DEBUG
	Serial.println("Sending HTTP-request for weather.");
#endif
  
  String server = "api.openweathermap.org";
  settings.getWeatherAPIKey(weather_api_key, sizeof(weather_api_key));
  String path = "/data/2.5/weather?zip="+location+"&units=metric&APPID="+weather_api_key;
	
	WiFiClient wifiClient;
	HttpClient client = HttpClient(wifiClient, server.c_str(), 80);
  
  client.get(path);
	uint16_t statusCode = client.responseStatusCode();
	
	if (statusCode == 200) {
		String response = client.responseBody();
		//response = response.substring(response.indexOf('{'), response.lastIndexOf('}') + 1);

    #ifdef DEBUG
    		Serial.printf("Status: %u\r\n", statusCode);
    		Serial.printf("Response is %u bytes.\r\n", response.length());
    		Serial.println(response);
    		Serial.println("Parsing JSON.");
    #endif

    // Parse the JSON
    // see https://arduinojson.org/v5/assistant/
    const size_t capacity = JSON_ARRAY_SIZE(3) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 480;

		//DynamicJsonBuffer jsonBuffer;
		StaticJsonBuffer<capacity> jsonBuffer;
		JsonObject &responseJson = jsonBuffer.parseObject(response);

		if (responseJson.success()) {
      Serial.println("Parsing JSON successful.");
      outdoorCode = responseJson["weather"][0]["id"].as<int8_t>();
			outdoorTitle =  responseJson["weather"][0]["main"].as<String>() + ": " + responseJson["weather"][0]["description"].as<String>();
			outdoorTemperature = responseJson["main"]["temp"].as<int8_t>();
			outdoorHumidity = responseJson["main"]["humidity"].as<uint8_t>();
      #ifdef DEBUG
      			Serial.println(outdoorTitle);
      			Serial.printf("Temperature (Online): %dC\r\n", outdoorTemperature);
      			Serial.printf("Humidity (Online): %u%%\r\n", outdoorHumidity);
      			//Serial.println("Condition (Online): " + String(sWeatherCondition[outdoorCode]) + " (" + String(outdoorCode) + ")");
      #endif
			return;
		}
   else {
    #ifdef DEBUG
      Serial.println("Json parsing (outdoor temperature) failed!");
    #endif
   }
	}
	else {
    #ifdef DEBUG
	    Serial.printf("Status: %u\r\n", statusCode);
      Serial.println("API Key: " + String(weather_api_key));
      Serial.println("URL: " + server + path);
    #endif
    outdoorTitle = "Request failed.";
	}
    
}

#ifdef RTC_BACKUP
/******************************************************************************
rtc
******************************************************************************/
time_t getRtcTime() {
	DEBUG_PRINTLN("*** ESP set from RTC. ***");
	//DEBUG_PRINTLN("Drift was: " + String(now() - RTC.get()) + " seconds.");  // now() is not working when timeprovider itself calls this function
	return RTC.get();
}
#endif

/******************************************************************************
Get time from NTP.
******************************************************************************/

time_t getNtpTime() {
	if (WiFi.status() != WL_CONNECTED) {
		DEBUG_PRINTLN("Sending NTP-request failed. No WiFi.");
#ifdef RTC_BACKUP
		return getRtcTime();
#else
		return now();
#endif
	}
	esp8266WebServer.handleClient();
	DEBUG_PRINTLN("Sending NTP-request to \"" + String(NTP_SERVER) + "\".");
	uint8_t packetBuffer[49] = { };
	packetBuffer[0] = 0xE3;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x06;
	packetBuffer[3] = 0xEC;
	packetBuffer[12] = 0x31;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 0x31;
	packetBuffer[15] = 0x34;
	WiFiUDP wifiUdp;
	wifiUdp.begin(2390);
	char server[] = NTP_SERVER;
	IPAddress timeServerIP;
	WiFi.hostByName(server, timeServerIP);
	wifiUdp.beginPacket(timeServerIP, 123);
	wifiUdp.write(packetBuffer, 48);
	wifiUdp.endPacket();
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		if (wifiUdp.parsePacket() >= 48) {
			wifiUdp.read(packetBuffer, 48);
			uint32_t ntpTime = (packetBuffer[40] << 24) + (packetBuffer[41] << 16) + (packetBuffer[42] << 8) + packetBuffer[43];
			ntpTime -= 2208988800; // NTP time is seconds from 1900, we need from 1970
#ifdef DEBUG
			debug.debugTime("NTP-response: (UTC)", ntpTime, true);
#endif
#ifdef RTC_BACKUP
			DEBUG_PRINTLN("*** RTC set from NTP. ***");
			RTC.set(timeZone.toLocal(ntpTime));
#endif
			DEBUG_PRINTLN("*** ESP set from NTP. ***");
			//DEBUG_PRINTLN("Drift was: " + String(now() - timeZone.toLocal(ntpTime)) + " seconds."); // now() is not working when timeprovider itself calls this function
			return (timeZone.toLocal(ntpTime));
		}
	}
	DEBUG_PRINTLN("No NTP response.");
#ifdef RTC_BACKUP
	return getRtcTime();
#else
	return now();
#endif
}

#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
/******************************************************************************
  Get room conditions.
******************************************************************************/

void getRoomConditions()
{
#if defined(RTC_BACKUP) && !defined(SENSOR_DHT22)
	roomTemperature = RTC.temperature() / 4.0 + RTC_TEMPERATURE_OFFSET;
#ifdef DEBUG
	Serial.println("Temperature (RTC): " + String(roomTemperature) + "C");
#endif
#endif
#ifdef SENSOR_DHT22
	float dhtTemperature = dht.readTemperature();
	float dhtHumidity = dht.readHumidity();
	if (!isnan(dhtTemperature) && !isnan(dhtHumidity))
	{
		roomTemperature = dhtTemperature + DHT_TEMPERATURE_OFFSET;
		roomHumidity = dhtHumidity + DHT_HUMIDITY_OFFSET;
#ifdef DEBUG
		Serial.println("Temperature (DHT): " + String(roomTemperature) + "C");
		Serial.println("Humidity (DHT): " + String(roomHumidity) + "%");
#endif
	}
#endif
}
#endif

/******************************************************************************
Misc.
******************************************************************************/

// Set mode of display.
void setMode(Mode newMode)
{
	DEBUG_PRINTLN("Mode: " + String(newMode));
	screenBufferNeedsUpdate = true;
	lastMode = mode;
	mode = newMode;

	// Set modeTimeout.
	switch (mode)
	{
	case STD_MODE_AMPM:
	case STD_MODE_SECONDS:
	case STD_MODE_WEEKDAY:
	case STD_MODE_DATE:
#if defined(RTC_BACKUP) && !defined(SENSOR_DHT22)
	case STD_MODE_TEMP:
#endif
#ifdef SENSOR_DHT22
	case STD_MODE_TEMP:
	case STD_MODE_HUMIDITY:
#endif
	case STD_MODE_EXT_TEMP:
	case STD_MODE_EXT_HUMIDITY:
		modeTimeout = millis();
		break;
	default:
		modeTimeout = 0;
		break;
	}
}

// set color number (disable color change mode by default)
void setColorNum(uint8_t colorNum) {
	setColorNum(colorNum, true);
}

// set color number
void setColorNum(uint8_t colorNum, bool disableCC) {
	if (disableCC) {
		settings.setColorChange(COLORCHANGE_NO);
	}
	settings.setColorNum(colorNum);
	screenBufferNeedsUpdate = true;
}

// set color (disable color change mode by default)
void setColor(uint32_t color) {
	setColor(color, true);
}

// set color
void setColor(uint32_t color, bool disableCC) {
	if (disableCC) {
		settings.setColorChange(COLORCHANGE_NO);
	}
	settings.setColor(color);
	screenBufferNeedsUpdate = true;
}

// Switch LEDs off.
void setLedsOff()
{
	DEBUG_PRINTLN("LEDs off.");
	SYSLOG("LEDs off.");
	if(mode != STD_MODE_BLANK){
		setMode(STD_MODE_BLANK);
	}
}

// Switch LEDs on.
void setLedsOn()
{
	DEBUG_PRINTLN("LEDs on.");
	SYSLOG("LEDs on.");
	setMode(lastMode);
}

// Toggle LEDs.
void setDisplayToToggle()
{
	if (mode == STD_MODE_BLANK)
	{
		setLedsOn();
	}
	else
	{
		setLedsOff();
	}
}

/******************************************************************************
  Webserver.
******************************************************************************/

void setupWebServer()
{
	esp8266WebServer.onNotFound(handleNotFound);
	esp8266WebServer.on("/", handleRoot);
	esp8266WebServer.on("/handleButtonOnOff", []() {buttonOnOffPressed(); callRoot();});
	esp8266WebServer.on("/handleButtonSettings", handleButtonSettings);
	esp8266WebServer.on("/handleButtonEvents", handleButtonEvents);
	esp8266WebServer.on("/handleButtonMode", []() {buttonModePressed(); callBack();});
	esp8266WebServer.on("/handleButtonTime", []() {buttonTimePressed(); callBack();});
	esp8266WebServer.on("/commitSettings", []() {handleCommitSettings(); callBack();});
	esp8266WebServer.on("/commitEvents", []() {handleCommitEvents(); callBack();});
	esp8266WebServer.on("/reset", handleReset);
	esp8266WebServer.on("/factoryReset", handleFactoryReset);
	esp8266WebServer.on("/wifiReset", handleWiFiReset);
    esp8266WebServer.on("/effect", []() {handleShowEffect(); callBack();});
    esp8266WebServer.on("/mode", []() {handleSelectMode(); callBack();});
	esp8266WebServer.begin();
}

void callRoot()
{
	esp8266WebServer.send(200, "text/html", "<!doctype html><html><head><script>window.onload=function(){window.location.replace('/');}</script></head></html>");
}

void callBack()
{
	esp8266WebServer.send(200, "text/html", "<!doctype html><html><head><script>window.onload=function(){if(navigator.userAgent.indexOf(\"Firefox\")!=-1){window.location.replace('/');}else{window.history.back();}}</script></head></html>");
}

// Page 404.
void handleNotFound()
{
	esp8266WebServer.send(404, "text/plain", "404 - File Not Found.");
}

// Page /.
void handleRoot()
{
	String message = "<!doctype html>";
	message += "<html>";
	message += "<head>";
	message += "<title>" + String(PRODUCT_NAME) + "</title>";
	message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	message += "<meta http-equiv=\"refresh\" content=\"60\" charset=\"UTF-8\">";
	message += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.10.0-12/css/all.min.css\">";
	message += "<style>";
	message += "body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}";
	message += "button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:180px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
    message += "button.effect{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:60px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
	message += "input.textmessage{background-color:#FFFFFF;color:#333333;font-family:Sans-serif;width:300px;padding:8px;border:2px solid #1FA3EC;font-size:24px;border-radius:10px;}";
	message += "</style>";
	message += "</head>";
	message += "<body>";
	message += "<h1>" + String(PRODUCT_NAME) + "</h1>";
#ifdef DEDICATION
	message += DEDICATION;
	message += "<br><br>";
#endif
	if (mode == STD_MODE_BLANK) message += "<button onclick=\"window.location.href='/handleButtonOnOff'\"><i class=\"fas fa-toggle-off\"></i></button>";
	else message += "<button onclick=\"window.location.href='/handleButtonOnOff'\"><i class=\"fas fa-toggle-on\"></i></button>";
	message += "<button onclick=\"window.location.href='/handleButtonSettings'\"><i class=\"fas fa-cogs\"></i></button>";
	//message += "<br><br>";
	//message += "<button onclick=\"window.location.href='/handleButtonMode'\"><i class=\"fas fa-bars\"></i></button>";
	//message += "<button onclick=\"window.location.href='/handleButtonTime'\"><i class=\"fas fa-clock\"></i></button>";
  message += "<br><br>";
  message += "<h2>Mode</h2>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_TIME)+"'\" class=\"effect\"><i class=\"fas fa-clock\"></i></button>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_SECONDS)+"'\" class=\"effect\"><i class=\"fas fa-stopwatch\"></i></button>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_DATE)+"'\" class=\"effect\"><i class=\"fas fa-calendar-alt\"></i></button>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_WEEKDAY)+"'\" class=\"effect\"><i class=\"fas fa-calendar-day\"></i></button>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_EXT_TEMP)+"'\" class=\"effect\"><i class=\"fas fa-cloud-sun\"></i></button>";
  message += "<button onclick=\"window.location.href='/mode?id="+String(STD_MODE_EXT_HUMIDITY)+"'\" class=\"effect\"><i class=\"fas fa-tint\"></i></button>";
  message += "<h2>Message</h2>";
  message += "<form action=\"/effect\"><input type=\"text\" name=\"name\" value=\"\" class=\"textmessage\"> <button class=\"effect\"><i class=\"fas fa-share-square\"></i></button></form>";
  message += "<h2>Effects</h2>";
  message += "<button onclick=\"window.location.href='/effect?name=heart'\" class=\"effect\"><i class=\"fas fa-heart\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=smile'\" class=\"effect\"><i class=\"fas fa-smile-wink\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=firework'\" class=\"effect\"><i class=\"fas fa-bomb\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=candle'\" class=\"effect\"><i class=\"fas fa-birthday-cake\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=coffee'\" class=\"effect\"><i class=\"fas fa-coffee\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=countdown'\" class=\"effect\"><i class=\"fas fa-stopwatch\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=champagne'\" class=\"effect\"><i class=\"fas fa-glass-cheers\"></i></button>";
  message += "<button onclick=\"window.location.href='/effect?name=christmas'\" class=\"effect\"><i class=\"fas fa-tree\"></i></button>";
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	message += "<br><br><i class = \"fas fa-home\" style=\"font-size:20px;\"></i>";
	message += "<br><i class=\"fas fa-thermometer\" style=\"font-size:20px;\"></i> " + String(roomTemperature) + "&deg;C / " + String(roomTemperature * 9.0 / 5.0 + 32.0) + "&deg;F";
#endif
#ifdef SENSOR_DHT22
	message += "<br><i class=\"fas fa-tint\" style=\"font-size:20px;\"></i> " + String(roomHumidity) + "%RH";
	message += "<br><span style=\"font-size:20px;\">";
	if (roomHumidity < 30) message += "<i style=\"color:Red;\" class=\"fas fa-square\"\"></i>";
	else message += "<i style=\"color:Red;\" class=\"fas fa-square-o\"></i>";
	if ((roomHumidity >= 30) && (roomHumidity < 40)) message += "&nbsp;<i style=\"color:Orange;\" class=\"fas fa-square\"></i>";
	else message += "&nbsp;<i style=\"color:Orange;\" class=\"fas fa-square-o\"></i>";
	if ((roomHumidity >= 40) && (roomHumidity <= 50)) message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fas fa-square\"></i>";
	else message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fas fa-square-o\"></i>";
	if ((roomHumidity > 50) && (roomHumidity < 60)) message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fas fa-square\"></i>";
	else message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fas fa-square-o\"></i>";
	if (roomHumidity >= 60) message += "&nbsp;<i style=\"color:Blue;\" class=\"fas fa-square\"></i>";
	else message += "&nbsp;<i style=\"color:Blue;\" class=\"fas fa-square-o\"></i>";
	message += "</span>";
#endif
	if (WiFi.status() == WL_CONNECTED)
	{
		message += "<br><br><i class = \"fas fa-tree\" style=\"font-size:20px;\"></i>";
		message += "<br><i class = \"fas fa-thermometer\" style=\"font-size:20px;\"></i> " + String(outdoorTemperature) + "&deg;C / " + String(outdoorTemperature * 9.0 / 5.0 + 32.0) + "&deg;F";
		message += "<br><i class = \"fas fa-tint\" style=\"font-size:20px;\"></i> " + String(outdoorHumidity) + "%RH";
		message += "<br><span class = \"";
		switch (outdoorCode)
		{
		case 0:  // tornado
		case 1:  // tropical storm
		case 2:  // hurricane
			message += "fas fa-frown-o";
			break;
		case 3:  // severe thunderstorms
		case 4:  // thunderstorms
		case 37: // isolated thunderstorms
		case 38: // scattered thunderstorms
		case 39: // scattered thunderstorms
		case 45: // thundershowers
		case 47: // isolated thundershowers
			message += "fas fa-flash";
			break;
		case 5:  // mixed rain and snow
		case 6:  // mixed rain and sleet
		case 7:  // mixed snow and sleet
		case 8:  // freezing drizzle
		case 9:  // drizzle
		case 10: // freezing rain
		case 11: // showers
		case 12: // showers
			message += "fas fa-umbrella";
			break;
		case 13: // snow flurries
		case 14: // light snow showers
		case 15: // blowing snow
		case 16: // snow
		case 17: // hail
		case 41: // heavy snow
		case 42: // scattered snow showers
		case 43: // heavy snow
		case 46: // snow showers
			message += "fas fa-snowflake-o";
			break;
		case 23: // blustery
		case 24: // windy
			message += "fas fa-flag";
			break;
		case 31: // clear (night)
		case 33: // fair (night)
			message += "fas fa-moon-o";
			break;
		case 32: // sunny
		case 34: // fair (day)
		case 36: // hot
			message += "fas fa-sun-o";
			break;
		default:
			message += "fas fa-cloud";
			break;
		}
		//message += "\" style=\"font-size:20px;\"></span> " + String(FPSTR(sWeatherCondition[outdoorCode]));
    message += "\" style=\"font-size:20px;\"></span> " + outdoorTitle;
	}
	message += "<span style=\"font-size:12px;\">";
	message += "<br><br>" + String(PRODUCT_NAME) + " was <i class=\"fas fa-code\"></i> with <i class=\"fas fa-heart\"></i> by tmw-it.ch and <a href=\"http://bracci.ch\">bracci.</a>";
	message += "<br>Firmware: " + String(FIRMWARE_VERSION);
#if defined(UPDATE_INFO_STABLE) || defined(UPDATE_INFO_UNSTABLE)
	if (updateInfo > String(FIRMWARE_VERSION)) message += "<br><span style=\"color:red;\">Firmwareupdate available! (" + updateInfo + ")</span>";
#endif
#ifdef DEBUG_WEB
	time_t tempEspTime = now();
	message += "<br><br>Time: " + String(hour(tempEspTime)) + ":";
	if (minute(tempEspTime) < 10) message += "0";
	message += String(minute(tempEspTime));
	if (timeZone.locIsDST(now())) message += " (DST)";
	message += " up " + String(int((tempEspTime - powerOnTime) / 86400)) + " days, " + String(hour(tempEspTime - powerOnTime)) + ":";
	if (minute(tempEspTime - powerOnTime) < 10) message += "0";
	message += String(minute(tempEspTime - powerOnTime));
	message += "<br>" + String(dayStr(weekday(tempEspTime))) + ", " + String(monthStr(month(tempEspTime))) + " " + String(day(tempEspTime)) + ". " + String(year(tempEspTime));
	message += "<br>Free RAM: " + String(system_get_free_heap_size()) + " bytes";
	message += "<br>LED Driver: " + ledDriver.getSignature();
#ifdef PIN_LDR
	message += "<br>Brightness: " + String(ratedBrightness) + " (ABC: ";
	settings.getUseAbc() ? message += "enabled" : message += "disabled";
	message += ", min: " + String(MIN_BRIGHTNESS) + ", max : " + String(abcBrightness) + ")";
	message += "<br>LDR: " + String(ldrValue) + " (min: " + String(minLdrValue) + ", max: " + String(maxLdrValue) + ")";
#endif
	message += "<br>Error (NTP): " + String(errorCounterNtp);
#ifdef SENSOR_DHT22
	message += "<br>Error (DHT): " + String(errorCounterDht);
#endif
	message += "<br>Error (Yahoo): " + String(errorCounterYahoo);
	message += "<br>Weather: " + String(outdoorTitle);
	message += "<br>Reset reason: " + ESP.getResetReason();
#endif
	message += "</span>";
	message += "</body>";
	message += "</html>";
	esp8266WebServer.send(200, "text/html", message);
}

// Page settings.
void handleButtonSettings()
{
#ifdef DEBUG
	Serial.println("Settings pressed.");
#endif
	String message = "<!doctype html>";
	message += "<html>";
	message += "<head>";
	message += "<title>" + String(PRODUCT_NAME) + " Settings</title>";
	message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	message += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.10.0-12/css/all.min.css\">";
	message += "<style>";
	message += "body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}";
	message += "input[type=submit]{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:12px;border:5px solid #FFFFFF;font-size:20px;border-radius:10px;}";
	message += "button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
	message += "table{border-collapse:collapse;margin:0px auto;} td{padding:12px;border-bottom:1px solid #ddd;} tr:first-child{border-top:1px solid #ddd;} td:first-child{text-align:right;} td:last-child{text-align:left;}";
	message += "select{font-size:16px;}";
	message += "</style>";
	message += "</head>";
	message += "<body>";
	message += "<h1>" + String(PRODUCT_NAME) + " Settings</h1>";
	message += "<form action=\"/commitSettings\">";
	message += "<table>";
	// ------------------------------------------------------------------------
#ifdef BUZZER
	message += "<tr><td>";
	message += "Alarm 1:";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"a1\" value=\"1\"";
	if (settings.getAlarm1()) message += " checked";
	message += "> on ";
	message += "<input type=\"radio\" name=\"a1\" value=\"0\"";
	if (!settings.getAlarm1()) message += " checked";
	message += "> off&nbsp;&nbsp;&nbsp;";
	message += "<input type=\"time\" name=\"a1t\" value=\"";
	if (hour(settings.getAlarmTime1()) < 10) message += "0";
	message += String(hour(settings.getAlarmTime1())) + ":";
	if (minute(settings.getAlarmTime1()) < 10) message += "0";
	message += String(minute(settings.getAlarmTime1())) + "\">";
	message += " h<br><br>";
	message += "<input type=\"checkbox\" name=\"a1w2\" value=\"4\"";
	if (bitRead(settings.getAlarm1Weekdays(), 2)) message += " checked";
	message += "> Mo. ";
	message += "<input type=\"checkbox\" name=\"a1w3\" value=\"8\"";
	if (bitRead(settings.getAlarm1Weekdays(), 3)) message += " checked";
	message += "> Tu. ";
	message += "<input type=\"checkbox\" name=\"a1w4\" value=\"16\"";
	if (bitRead(settings.getAlarm1Weekdays(), 4)) message += " checked";
	message += "> We. ";
	message += "<input type=\"checkbox\" name=\"a1w5\" value=\"32\"";
	if (bitRead(settings.getAlarm1Weekdays(), 5)) message += " checked";
	message += "> Th. ";
	message += "<input type=\"checkbox\" name=\"a1w6\" value=\"64\"";
	if (bitRead(settings.getAlarm1Weekdays(), 6)) message += " checked";
	message += "> Fr. ";
	message += "<input type=\"checkbox\" name=\"a1w7\" value=\"128\"";
	if (bitRead(settings.getAlarm1Weekdays(), 7)) message += " checked";
	message += "> Sa. ";
	message += "<input type=\"checkbox\" name=\"a1w1\" value=\"2\"";
	if (bitRead(settings.getAlarm1Weekdays(), 1)) message += " checked";
	message += "> Su. ";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Alarm 2:";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"a2\" value=\"1\"";
	if (settings.getAlarm2()) message += " checked";
	message += "> on ";
	message += "<input type=\"radio\" name=\"a2\" value=\"0\"";
	if (!settings.getAlarm2()) message += " checked";
	message += "> off&nbsp;&nbsp;&nbsp;";
	message += "<input type=\"time\" name=\"a2t\" value=\"";
	if (hour(settings.getAlarmTime2()) < 10) message += "0";
	message += String(hour(settings.getAlarmTime2())) + ":";
	if (minute(settings.getAlarmTime2()) < 10) message += "0";
	message += String(minute(settings.getAlarmTime2())) + "\">";
	message += " h<br><br>";
	message += "<input type=\"checkbox\" name=\"a2w2\" value=\"4\"";
	if (bitRead(settings.getAlarm2Weekdays(), 2)) message += " checked";
	message += "> Mo. ";
	message += "<input type=\"checkbox\" name=\"a2w3\" value=\"8\"";
	if (bitRead(settings.getAlarm2Weekdays(), 3)) message += " checked";
	message += "> Tu. ";
	message += "<input type=\"checkbox\" name=\"a2w4\" value=\"16\"";
	if (bitRead(settings.getAlarm2Weekdays(), 4)) message += " checked";
	message += "> We. ";
	message += "<input type=\"checkbox\" name=\"a2w5\" value=\"32\"";
	if (bitRead(settings.getAlarm2Weekdays(), 5)) message += " checked";
	message += "> Th. ";
	message += "<input type=\"checkbox\" name=\"a2w6\" value=\"64\"";
	if (bitRead(settings.getAlarm2Weekdays(), 6)) message += " checked";
	message += "> Fr. ";
	message += "<input type=\"checkbox\" name=\"a2w7\" value=\"128\"";
	if (bitRead(settings.getAlarm2Weekdays(), 7)) message += " checked";
	message += "> Sa. ";
	message += "<input type=\"checkbox\" name=\"a2w1\" value=\"2\"";
	if (bitRead(settings.getAlarm2Weekdays(), 1)) message += " checked";
	message += "> Su. ";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Timer:";
	message += "</td><td>";
	message += "<select name=\"ti\">";
	message += "<option value=\"255\">stop timer</option>";
	message += "<option value=\"0\" selected>0</option>";
	for (int i = 1; i <= 10; i++)
	{
		message += "<option value=\"" + String(i) + "\">";
		if (i < 10) message += "0";
		message += String(i) + "</option>";
	}
	message += "<option value=\"15\">15</option>";
	message += "<option value=\"20\">20</option>";
	message += "<option value=\"25\">25</option>";
	message += "<option value=\"30\">30</option>";
	message += "<option value=\"45\">45</option>";
	message += "<option value=\"60\">60</option>";
	message += "</select> minutes.";
	message += "</td></tr>";
#endif
	// ------------------------------------------------------------------------
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	message += "<tr><td>";
	message += "Auto Mode Change:";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"am\" value=\"1\"";
	if (settings.getShowTemp()) message += " checked";
	message += "> on ";
	message += "<input type=\"radio\" name=\"am\" value=\"0\"";
	if (!settings.getShowTemp()) message += " checked";
	message += "> off";
	message += "</td></tr>";
#endif
	// ------------------------------------------------------------------------
#if defined(PIN_LDR)
	message += "<tr><td>";
	message += "Auto Brightness:";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"ab\" value=\"1\"";
	if (settings.getUseLdr()) message += " checked";
	message += "> on ";
	message += "<input type=\"radio\" name=\"ab\" value=\"0\"";
	if (!settings.getUseLdr()) message += " checked";
	message += "> off";
	message += "</td></tr>";
#endif
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Brightness:";
	message += "</td><td>";
	message += "<select name=\"br\">";
	for (int i = 10; i <= 100; i += 10)
	{
		message += "<option value=\"" + String(i) + "\"";
		if (i == (map(settings.getBrightness(), 0, 255, 0, 100)+5)/10*10) message += " selected";
		message += ">";
		message += String(i) + "</option>";
	}
	message += "</select> %";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	uint8_t colorNum = settings.getColorNum();
	message += "Color:";
	message += "</td><td>";
	message += "<select name=\"co\">";
	for(uint8_t i = 0; i <= COLOR_COUNT; i++){
		message += "<option value=\"" +String(i) + "\"";
		if (colorNum == i) message += " selected";
		message += ">";
		message += String(FPSTR(sColorStr[i])) + "</option>";
	}
	message += "</select>";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Color Change:";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"cc\" value=\"4\"";
	if (settings.getColorChange() == 4) message += " checked";
	message += "> day ";
	message += "<input type=\"radio\" name=\"cc\" value=\"3\"";
	if (settings.getColorChange() == 3) message += " checked";
	message += "> hour ";
	message += "<input type=\"radio\" name=\"cc\" value=\"2\"";
	if (settings.getColorChange() == 2) message += " checked";
	message += "> five ";
	message += "<input type=\"radio\" name=\"cc\" value=\"1\"";
	if (settings.getColorChange() == 1) message += " checked";
	message += "> mood";
	message += "<input type=\"radio\" name=\"cc\" value=\"0\"";
	if (settings.getColorChange() == 0) message += " checked";
	message += "> off";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Mood Rate:";
	message += "</td><td>";
	message += "<select name=\"mr\">";
	for (int i = 0; i < 10; i++)
	{
		message += "<option value=\"" + String(i) + "\"";
		if (i == settings.getMoodRate()) message += " selected";
		message += ">";
		message += String(i) + "</option>";
	}
	message += "</select>";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Transition:";
	uint8_t transition = settings.getTransition();
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"tr\" value=\"3\"";
	if (transition == 3) message += " checked";
	message += "> slide ";
	message += "<input type=\"radio\" name=\"tr\" value=\"2\"";
	if (transition == 2) message += " checked";
	message += "> matrix";
	message += "<input type=\"radio\" name=\"tr\" value=\"1\"";
	if (transition == 1) message += " checked";
	message += "> fade ";
	message += "<input type=\"radio\" name=\"tr\" value=\"0\"";
	if (transition == 0) message += " checked";
	message += "> normal";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Fallback Timeout:";
	message += "</td><td>";
	message += "<select name=\"to\">";
	for (int i = 0; i <= 60; i += 5)
	{
		message += "<option value=\"" + String(i) + "\"";
		if (i == settings.getTimeout()) message += " selected";
		message += ">";
		if (i < 10) message += "0";
		message += String(i) + "</option>";
	}
	message += "</select> sec.";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Night off:";
	message += "</td><td>";
	message += "<input type=\"time\" name=\"no\" value=\"";
	if (hour(settings.getNightOffTime()) < 10) message += "0";
	message += String(hour(settings.getNightOffTime())) + ":";
	if (minute(settings.getNightOffTime()) < 10) message += "0";
	message += String(minute(settings.getNightOffTime())) + "\">";
	message += " h";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Day on:";
	message += "</td><td>";
	message += "<input type=\"time\" name=\"do\" value=\"";
	if (hour(settings.getDayOnTime()) < 10) message += "0";
	message += String(hour(settings.getDayOnTime())) + ":";
	if (minute(settings.getDayOnTime()) < 10) message += "0";
	message += String(minute(settings.getDayOnTime())) + "\">";
	message += " h";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Show \"It is\":";
	message += "</td><td>";
	message += "<input type=\"radio\" name=\"ii\" value=\"1\"";
	if (settings.getItIs()) message += " checked";
	message += "> on ";
	message += "<input type=\"radio\" name=\"ii\" value=\"0\"";
	if (!settings.getItIs()) message += " checked";
	message += "> off";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Language:";
	message += "</td><td>";
	message += "<select name=\"la\">";
	for (uint8_t i = 0; i <= LANGUAGE_COUNT; i++){
	message += "<option value=\"" + String(i) +"\"";
	if (settings.getLanguage() == i) message += " selected";
	message += ">";
	message += String(FPSTR(sLanguageStr[i])) + "</option>";
	}
	message += "</select>";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	eLayout layout = settings.getLayout();
	message += "Layout:";
	message += "</td><td>";
	message += "<select name=\"ly\">";
	for(uint8_t i = 0; i < E_LAYOUT_MAX; i++){
		message += "<option value=\"" +String(i) + "\"";
		if (layout == i) message += " selected";
		message += ">";
		message += String(FPSTR(sLayoutStr[i])) + "</option>";
	}
	message += "</select>";
  message += " <i>(reboot needed)</i>";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Location:";
	message += "</td><td>";
	message += "<input type=\"text\" name=\"loc\" value=\""; 
	settings.getLocation(location, sizeof(location));
	message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter Location ...\">";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
  message += "<tr><td>";
  message += "OpenWeatherMap API Key:";
  message += "</td><td>";
  message += "<input type=\"text\" name=\"api_key\" value=\"";
  settings.getWeatherAPIKey(weather_api_key, sizeof(weather_api_key));
  message += String(weather_api_key) + "\" placeholder=\"Enter API key ...\">";
  message += "</td></tr>";
  // ------------------------------------------------------------------------
	message += "<tr><td>";
	message += "Set date/time:";
	message += "</td><td>";
	message += "<input type=\"datetime-local\" name=\"st\">";
	message += "</td></tr>";
	// ------------------------------------------------------------------------
	message += "</table>";
	message += "<br><button title=\"Save Settings.\"><i class=\"fas fa-check\"></i></button>";
	message += "</form>";
	message += "<button title=\"Events\" onclick=\"window.location.href='/handleButtonEvents'\"><i class=\"fas fa-birthday-cake\"></i></button>";
	message += "<br>";
	message += "<button title=\"Back\" onclick=\"window.location.href='/'\"><i class=\"fas fa-reply\"></i></button>";
	message += "</body>";
	message += "</html>";
	esp8266WebServer.send(200, "text/html", message);
}

// Page settings.
void handleButtonEvents()
{
#ifdef DEBUG
	Serial.println("Events pressed.");
#endif
	String message = "<!doctype html>";
	message += "<html>";
	message += "<head>";
	message += "<title>" + String(PRODUCT_NAME) + " Events</title>";
	message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	message += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.10.0-12/css/all.min.css\">";
	message += "<style>";
	message += "body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}";
	message += "input[type=submit]{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:12px;border:5px solid #FFFFFF;font-size:20px;border-radius:10px;}";
	message += "button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
	message += "table{border-collapse:collapse;margin:0px auto;} td{padding:12px;border-bottom:1px solid #ddd;} tr:first-child{border-top:1px solid #ddd;} td:first-child{text-align:right;} td:last-child{text-align:left;}";
	message += "select{font-size:16px;}";
	message += "</style>";
	message += "</head>";
	message += "<body>";
	message += "<h1>" + String(PRODUCT_NAME) + " Events</h1>";
	message += "<form action=\"/commitEvents\">";
	message += "<table>";
	message += "<tr><td>";
	message += "Repetition Rate:";
	message += "</td><td>";
	message += "<select name=\"evr\">";
	for(uint8_t i = 0; i < EVT_REP_COUNT; i++){
		message += "<option value=\"" + String(i) + "\"";
		if (settings.getEventDisplayRate() == i) message += " selected";
		message += ">" + String(FPSTR(sEvtRep[i])) + "</option>";
	}
	message += "</select> minutes.";
	message += "</td>";

	// ------------------------------------------------------------------------
	for (uint8_t i = 0; i < NUM_EVTS; i++){
		message += "<tr><td>";
		message += "Event " + String(i) + ":";
		message += "</td><td>";
		message += "<input type=\"radio\" name=\"ev" + String(i) + "\" value=\"1\"";
		if (settings.getEventEnabled(i)) message += " checked";
		message += "> on ";
		message += "<input type=\"radio\" name=\"ev" + String(i) + "\" value=\"0\"";
		if (!settings.getEventEnabled(i)) message += " checked";
		message += "> off&nbsp;&nbsp;&nbsp;";
		message += "<input type=\"date\" name=\"ev" + String(i) + "d\" value=\"";
		message += String(year(settings.getEventDate(i))) + "-";
		if (month(settings.getEventDate(i)) < 10) message += "0";
		message += String(month(settings.getEventDate(i))) + "-";
		if (day(settings.getEventDate(i)) < 10) message += "0";
		message += String(day(settings.getEventDate(i)))+ "\" min=\"1970-01-01\">";
		message += "<br><br>";
		message += "<input type=\"text\" name=\"ev" + String(i) + "t\" value=\"";
		char eventText[LEN_EVT_STR];
		settings.getEventText(i, eventText, sizeof(eventText));
		message += String(eventText) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_EVT_STR-1) + "}\" placeholder=\"Event text ...\">";
		message += "<br><br>";
		message += "<select name=\"ev" + String(i) + "ani\">";

		uint8_t eventAnimation = settings.getEventAnimation(i);
		message += "<option value=\"" + String(Effects::NO_EFFECT) + "\"";
		if (eventAnimation == Effects::NO_EFFECT) message += " selected";
		message += ">";
		message += String(FPSTR(sAnimationStr[0])) + "</option>";

		for(uint8_t j = 0; j < Effects::ANI_COUNT; j++){
			message += "<option value=\"" + String(j) + "\"";
			if (eventAnimation == j) message += " selected";
			message += ">";
			message += String(FPSTR(sAnimationStr[j+1])) + "</option>";
		}
		message += "</select>";
		message += "<br><br>";
		message += "<select name=\"ev" + String(i) + "c\">";
		uint8_t colorNum = settings.getEventColor(i);
		for(uint8_t j = 0; j <= COLOR_COUNT; j++){
			message += "<option value=\"" +String(j) + "\"";
			if (colorNum == j) message += " selected";
			message += ">";
			message += String(FPSTR(sColorStr[j])) + "</option>";
		}
		message += "</select>";

		message += "</td></tr>";
	}
	// ------------------------------------------------------------------------

	message += "</table>";
	message += "<br><button title=\"Save Settings.\"><i class=\"fas fa-check\"></i></button>";
	message += "</form>";
	message += "<button title=\"Back\" onclick=\"window.location.href='/handleButtonSettings'\"><i class=\"fas fa-reply\"></i></button>";
	message += "</body>";
	message += "</html>";
	esp8266WebServer.send(200, "text/html", message);
}

void handleCommitSettings()
{
#ifdef DEBUG
	Serial.println("Commit settings pressed.");
#endif
	// ------------------------------------------------------------------------
#ifdef BUZZER
	time_t alarmTimeFromWeb = 0;
	esp8266WebServer.arg("a1") == "0" ? settings.setAlarm1(false) : settings.setAlarm1(true);
	alarmTimeFromWeb = esp8266WebServer.arg("a1t").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("a1t").substring(3, 5).toInt() * 60;
	if (settings.getAlarmTime1() != alarmTimeFromWeb) settings.setAlarm1(true);
	settings.setAlarmTime1(alarmTimeFromWeb);
	uint8_t alarmWeekDays = 0;
	for(uint8_t i = 1; i <= 7; i++){
		alarmWeekDays += esp8266WebServer.arg("a1w" + String(i)).toInt();
	}
	settings.setAlarm1Weekdays(alarmWeekDays);
	// ------------------------------------------------------------------------
	esp8266WebServer.arg("a2") == "0" ? settings.setAlarm2(false) : settings.setAlarm2(true);
	alarmTimeFromWeb = esp8266WebServer.arg("a2t").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("a2t").substring(3, 5).toInt() * 60;
	if (settings.getAlarmTime2() != alarmTimeFromWeb) settings.setAlarm2(true);
	settings.setAlarmTime2(alarmTimeFromWeb);
	alarmWeekDays = 0;
	for(uint8_t i = 1; i <= 7; i++){
		alarmWeekDays += esp8266WebServer.arg("a2w" + String(i)).toInt();
	}
	settings.setAlarm2Weekdays(alarmWeekDays);
	// ------------------------------------------------------------------------

	uint8_t timerMins = esp8266WebServer.arg("ti").toInt();
	if ((timerMins > 0) && (timerMins < 100)) {
		timerMinutes = timerMins;
		timer = now() + timerMinutes * 60;
		timerSet = true;
	} else if (timerMins == 255) {
		timerSet = false;
		timerMinutes = 0;
	}
#endif
	// ------------------------------------------------------------------------
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	esp8266WebServer.arg("am") == "0" ? settings.setShowTemp(false) : settings.setShowTemp(true);
#endif
	// ------------------------------------------------------------------------
#if defined(PIN_LDR)
	settings.setUseLdr(esp8266WebServer.arg("ab") == "1");
#endif
	// ------------------------------------------------------------------------
	settings.setBrightness(constrain(map(esp8266WebServer.arg("br").toInt(), 0, 100, 0, 255), MIN_BRIGHTNESS, MAX_BRIGHTNESS));
	if (!settings.getUseLdr()) {
		brightness = settings.getBrightness();
	}
	// ------------------------------------------------------------------------
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	esp8266WebServer.arg("am") == "0" ? settings.setShowTemp(false) : settings.setShowTemp(true);
#endif
	// ------------------------------------------------------------------------
	setColorNum(esp8266WebServer.arg("co").toInt(), false);
	// ------------------------------------------------------------------------
	switch (esp8266WebServer.arg("cc").toInt())
	{
	case 0: settings.setColorChange(COLORCHANGE_NO); break;
	case 1: settings.setColorChange(COLORCHANGE_MOOD); break;
	case 2: settings.setColorChange(COLORCHANGE_FIVE); break;
	case 3: settings.setColorChange(COLORCHANGE_HOUR); break;
	case 4: settings.setColorChange(COLORCHANGE_DAY); break;
	}
	// ------------------------------------------------------------------------
	settings.setMoodRate(esp8266WebServer.arg("mr").toInt());
	// ------------------------------------------------------------------------
	switch (esp8266WebServer.arg("tr").toInt())
	{
	case 0: settings.setTransition(TRANSITION_NORMAL); break;
	case 1: settings.setTransition(TRANSITION_FADE); break;
	case 2: settings.setTransition(TRANSITION_MATRIX); break;
	case 3: settings.setTransition(TRANSITION_SLIDE); break;
	}
	// ------------------------------------------------------------------------
	settings.setTimeout(esp8266WebServer.arg("to").toInt());
	// ------------------------------------------------------------------------
	settings.setNightOffTime(esp8266WebServer.arg("no").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("no").substring(3, 5).toInt() * 60);
	// ------------------------------------------------------------------------
	settings.setDayOnTime(esp8266WebServer.arg("do").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("do").substring(3, 5).toInt() * 60);
	// ------------------------------------------------------------------------
	esp8266WebServer.arg("ii") == "0" ? settings.setItIs(false) : settings.setItIs(true);
	// ------------------------------------------------------------------------
	settings.setLanguage(esp8266WebServer.arg("la").toInt());
	// ------------------------------------------------------------------------
	settings.setLayout((eLayout)(esp8266WebServer.arg("ly").toInt()));
	// ------------------------------------------------------------------------
	if (esp8266WebServer.arg("st").length())
	{
		setTime(esp8266WebServer.arg("st").substring(11, 13).toInt(), esp8266WebServer.arg("st").substring(14, 16).toInt(), 0, esp8266WebServer.arg("st").substring(8, 10).toInt(), esp8266WebServer.arg("st").substring(5, 7).toInt(), esp8266WebServer.arg("st").substring(0, 4).toInt());
#ifdef RTC_BACKUP
		RTC.set(now());
#endif
	}
	// ------------------------------------------------------------------------
	char text[LEN_LOC_STR];
	memset(text, 0, sizeof(text));
	esp8266WebServer.arg("loc").toCharArray(text, sizeof(text), 0);
	settings.setLocation(text, sizeof(text));
	// ------------------------------------------------------------------------
  char key[LEN_API_KEY];
  memset(key, 0, sizeof(key));
  esp8266WebServer.arg("api_key").toCharArray(key, sizeof(key), 0);
  settings.setWeatherAPIKey(key, sizeof(key));
  // ------------------------------------------------------------------------
	settings.saveToEEPROM();
	screenBufferNeedsUpdate = true;
}

void handleCommitEvents()
{
#ifdef DEBUG
	Serial.println("Commit events pressed.");
#endif
	// ------------------------------------------------------------------------
	settings.setEventDisplayRate(esp8266WebServer.arg("evr").toInt());
	// ------------------------------------------------------------------------
	for (uint8_t i = 0; i < NUM_EVTS; i++){
		char text[LEN_EVT_STR];
		memset(text, 0, sizeof(text));
		esp8266WebServer.arg("ev" + String(i) + "t").toCharArray(text, sizeof(text), 0);
		settings.setEventText(i, text, sizeof(text));
		settings.setEventEnabled(i, esp8266WebServer.arg("ev" + String(i)).toInt());
		settings.setEventAnimation(i, esp8266WebServer.arg("ev" + String(i) + "ani").toInt());
		settings.setEventColor(i, (eColor)esp8266WebServer.arg("ev" + String(i) + "c").toInt());
		tmElements_t eventTm;
		eventTm.Year = esp8266WebServer.arg("ev" + String(i) + "d").substring(0, 4).toInt() - 1970;
		eventTm.Month = esp8266WebServer.arg("ev" + String(i) + "d").substring(5, 7).toInt();
		eventTm.Day = esp8266WebServer.arg("ev" + String(i) + "d").substring(8, 10).toInt();
		eventTm.Hour = 0;
		eventTm.Minute = 0;
		eventTm.Second = 0;
		time_t eventTime = makeTime(eventTm);
		settings.setEventTime(i, eventTime);
	}
	// ------------------------------------------------------------------------
	settings.saveToEEPROM();
	screenBufferNeedsUpdate = true;
}

void handleReset()
{
	esp8266WebServer.send(200, "text/plain", "OK.");
	ESP.restart();
}

void handleFactoryReset()
{
	settings.resetToDefault();
	esp8266WebServer.send(200, "text/plain", "OK.");
	ESP.restart();
}

void handleWiFiReset()
{
	esp8266WebServer.send(200, "text/plain", "OK.");
	WiFi.disconnect(true);
	ESP.restart();
}


void handleShowEffect()
{
  mode = EXT_MODE_EFFECT;
  effect = esp8266WebServer.arg("name");
  //esp8266WebServer.send(200, "text/plain", "OK.");

#ifdef DEBUG
  Serial.println("Web-Event set: " + effect);
#endif
}

void handleSelectMode() {
  int newMode = esp8266WebServer.arg("id").toInt();
  mode = (eMode) newMode;
  screenBufferNeedsUpdate = true;
  modeTimeout = millis();
}
