/******************************************************************************
Configuration.h
See README.md for help.
******************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/******************************************************************************
Software settings.
******************************************************************************/

//#define HW_QLOCK_PURE_WIFI_V1_0
//#define HW_QLOCK_PURE_WIFI_V1_1
//#define HW_QLOCKWIFIVE_MINI_V1_0
//#define HW_QLOCKWIFIVE_MINI_V1_1
//#define HW_QLOCKWIFIVE_V1_0
//#define HW_QLOCKWIFIVE_V1_1
#define HW_QLOCKWIFIVE_V1_2
//#define HW_QLOCKWORK

//#define STATIC_IP

#define WIFI_AP_TIMEOUT 300
#define WIFI_AP_PASS "123456789"
//#define WIFI_RESET
#define OTA_PASS "1234"
#define NTP_SERVER "pool.ntp.org"

//#define SELFTEST
//#define SHOW_IP
#define NONE_TECHNICAL_ZERO
#define AUTO_MODECHANGE_TIME 60
//#define DEDICATION "The only reason for time is so that everything doesn't happen at once.<br>(Albert Einstein)"

//#define LANGUAGE_ENGLISH
#define LANGUAGE_GERMAN
//#define LANGUAGE_SPANISH
//#define LANGUAGE_FRENCH
//#define LANGUAGE_ITALIEN
//#define LANGUAGE_NETHERLANDS

//#define UPDATE_INFO_UNSTABLE
//#define UPDATE_INFO_STABLE

//#define TIMEZONE_IDLW  // IDLW  Internation Date Line West UTC-12
//#define TIMEZONE_SST   // SST   Samoa Standard Time UTC-11
//#define TIMEZONE_HST   // HST   Hawaiian Standard Time UTC-10
//#define TIMEZONE_AKST  // AKST  Alaska Standard Time UTC-9
//#define TIMEZONE_USPST // USPST Pacific Standard Time (USA) UTC-8
//#define TIMEZONE_USMST // USMST Mountain Standard Time (USA) UTC-7
//#define TIMEZONE_USAZ  // USAZ  Mountain Standard Time (USA) UTC-7 (no DST)
//#define TIMEZONE_USCST // USCST Central Standard Time (USA) UTC-6
//#define TIMEZONE_USEST // USEST Eastern Standard Time (USA) UTC-5
//#define TIMEZONE_AST   // AST   Atlantic Standard Time UTC-4
//#define TIMEZONE_BST   // BST   Eastern Brazil Standard Time UTC-3
//#define TIMEZONE_VTZ   // VTZ   Greenland Eastern Standard Time UTC-2
//#define TIMEZONE_AZOT  // AZOT  Azores Time UTC-1
//#define TIMEZONE_GMT   // GMT   Greenwich Mean Time UTC
#define TIMEZONE_CET     // CET   Central Europe Time UTC+1
//#define TIMEZONE_EST   // EST   Eastern Europe Time UTC+2
//#define TIMEZONE_MSK   // MSK   Moscow Time UTC+3 (no DST)
//#define TIMEZONE_GST   // GST   Gulf Standard Time UTC+4
//#define TIMEZONE_PKT   // PKT   Pakistan Time UTC+5
//#define TIMEZONE_BDT   // BDT   Bangladesh Time UTC+6
//#define TIMEZONE_JT    // JT    Java Time UTC+7
//#define TIMEZONE_CNST  // CNST  China Standard Time UTC+8
//#define TIMEZONE_HKT   // HKT   Hong Kong Time UTC+8
//#define TIMEZONE_PYT   // PYT   Pyongyang Time (North Korea) UTC+8.5
//#define TIMEZONE_CWT   // CWT   Central West Time (Australia) UTC+8.75
//#define TIMEZONE_JST   // JST   Japan Standard Time UTC+9
//#define TIMEZONE_ACST  // ACST  Australian Central Standard Time UTC+9.5
//#define TIMEZONE_AEST  // AEST  Australian Eastern Standard Time UTC+10
//#define TIMEZONE_LHST  // LHST  Lord Howe Standard Time UTC+10.5
//#define TIMEZONE_SBT   // SBT   Solomon Islands Time UTC+11
//#define TIMEZONE_NZST  // NZST  New Zealand Standard Time UTC+12

/******************************************************************************
Hardware settings.
******************************************************************************/
//#define SENSOR_DHT22
//--------------------
#define DHT_TEMPERATURE_OFFSET 0.3
#define DHT_HUMIDITY_OFFSET 0.4

//#define RTC_BACKUP
//------------------
#define RTC_TEMPERATURE_OFFSET -0.25

//-----------
#define LDR_HYSTERESIS 50
#define MIN_BRIGHTNESS 15
#define MAX_BRIGHTNESS 255

//--------------
#define BUTTON_TRESHOLD 300

//#define BUZZER
//--------------
#define BUZZTIME_ALARM_1 60
#define BUZZTIME_ALARM_2 60
#define BUZZTIME_TIMER 30

//--------------
#define MOOD_INTERVAL_MIN 300
#define MOOD_INTERVAL_MAX 4000

//#define LED_LIBRARY_LPD8806RGBW
//-------------------------------

#define LED_LIBRARY_NEOPIXEL
//----------------------------
#define LED_DRIVER_NEO_800KHZ_GRB
//#define LED_DRIVER_NEO_800KHZ_RGB
//#define LED_DRIVER_NEO_800KHZ_GRBW
//#define LED_DRIVER_NEO_800KHZ_RGBW
//#define LED_DRIVER_NEO_400KHZ_GRB
//#define LED_DRIVER_NEO_400KHZ_RGB

//#define LED_LIBRARY_FASTLED
//---------------------------
//#define LED_DRIVER_FAST_APA102
//#define LED_DRIVER_FAST_APA104
//#define LED_DRIVER_FAST_APA106
//#define LED_DRIVER_FAST_DOTSTAR
//#define LED_DRIVER_FAST_DMXSIMPLE
//#define LED_DRIVER_FAST_GW6205
//#define LED_DRIVER_FAST_GW6205_400
//#define LED_DRIVER_FAST_LPD1886
//#define LED_DRIVER_FAST_LPD1886_8BIT
//#define LED_DRIVER_FAST_LPD8806
//#define LED_DRIVER_FAST_NEOPIXEL
//#define LED_DRIVER_FAST_P9813
//#define LED_DRIVER_FAST_PL9823
//#define LED_DRIVER_FAST_SK6812
//#define LED_DRIVER_FAST_SK6822
//#define LED_DRIVER_FAST_SK9822
//#define LED_DRIVER_FAST_SM16716
//#define LED_DRIVER_FAST_TM1803
//#define LED_DRIVER_FAST_TM1804
//#define LED_DRIVER_FAST_TM1809
//#define LED_DRIVER_FAST_TM1812
//#define LED_DRIVER_FAST_TM1829
//#define LED_DRIVER_FAST_UCS1903
//#define LED_DRIVER_FAST_UCS1903B
//#define LED_DRIVER_FAST_UCS1904
//#define LED_DRIVER_FAST_UCS2903
//#define LED_DRIVER_FAST_WS2801
//#define LED_DRIVER_FAST_WS2803
//#define LED_DRIVER_FAST_WS2811
//#define LED_DRIVER_FAST_WS2811_400
//#define LED_DRIVER_FAST_WS2812
//#define LED_DRIVER_FAST_WS2812B
//#define LED_DRIVER_FAST_WS2813
//#define LED_DRIVER_FAST_WS2852

/******************************************************************************
Default values for EEPROM.
******************************************************************************/

#define DEFAULT_BRIGHTNESS 128
#define DEFAULT_COLOR COLOR_WHITE
#define DEFAULT_COLORCHANGE COLORCHANGE_NO
#define DEFAULT_SHOWTEMP false
#define DEFAULT_SHOWITIS true
#define DEFAULT_TRANSITION TRANSITION_FADE
#define DEFAULT_TIMEOUT 10
#define DEFAULT_USELDR false

/******************************************************************************
Misc.
******************************************************************************/

#define DEBUG
//#define DEBUG_MATRIX
//#define DEBUG_FPS

//#define SYSLOG_SERVER "192.168.0.1"
//#define SYSLOG_PORT 514

#define UPDATE_INFOSERVER "bracci.ch"
#define UPDATE_INFOFILE "/qlockwifive/updateinfo.json"

#define SERIAL_SPEED    115200

#ifdef HW_QLOCK_PURE_WIFI_V1_0
#define PRODUCT_NAME "QlockWiFive"
#define DEFAULT_LAYOUT E_LAYOUT_WIFIVE
#define LDR_IS_INVERSE true

#define PIN_IR_RECEIVER D4
#define PIN_LED         D6
#define PIN_BUZZER      D8
#define PIN_DHT22       D3
#define PIN_LEDS_CLOCK  D5
#define PIN_LEDS_DATA   D7
#define PIN_LDR         A0

#elif defined(HW_QLOCK_PURE_WIFI_V1_1) || defined(HW_QLOCKWIFIVE_V1_0) || defined(HW_QLOCKWIFIVE_V1_1)
#define PRODUCT_NAME "QlockWiFive"
#define DEFAULT_LAYOUT E_LAYOUT_WIFIVE
#define LDR_IS_INVERSE true

#define PIN_IR_RECEIVER D6
#define PIN_LED         D4
#define PIN_BUZZER      D8
#define PIN_DHT22       D3
#define PIN_LEDS_CLOCK  D5
#define PIN_LEDS_DATA   D7
#define PIN_LDR         A0

#elif defined(HW_QLOCKWIFIVE_V1_2)
#define PRODUCT_NAME "QlockWiFive"
#define DEFAULT_LAYOUT E_LAYOUT_CLT2
#define LDR_IS_INVERSE false

#define PIN_IR_RECEIVER D6
#define PIN_LED         D4 //D4
#define PIN_BUZZER      D6 //D8
#define PIN_DHT22       D3
#define PIN_LEDS_CLOCK  D5
#define PIN_LEDS_DATA   D8 //D6
#define PIN_LDR         A0

//#define BUTTONS
#define PLUS_BUTTON_PRESSING_AGAINST LOW
#define MINUS_BUTTON_PRESSING_AGAINST LOW
#define MODE_BUTTON_PRESSING_AGAINST HIGH
#define PIN_BUTTON_PLUS  03
#define PIN_BUTTON_MINUS 01
#define PIN_BUTTON_MODE  D0

#elif defined(HW_QLOCKWIFIVE_MINI_V1_0)
#define PRODUCT_NAME "QlockWiFiveMini"
#define DEFAULT_LAYOUT E_LAYOUT_WIFIVE_MINI
#define LDR_IS_INVERSE true

#define PIN_IR_RECEIVER D6
#define PIN_BUZZER      D8
#define PIN_DHT22       D3
#define PIN_LEDS_CLOCK  D5 // unused
#define PIN_LEDS_DATA   D7
#define PIN_LDR         A0

#define BUTTONS
#define PLUS_BUTTON_PRESSING_AGAINST  LOW
#define MINUS_BUTTON_PRESSING_AGAINST LOW
#define MODE_BUTTON_PRESSING_AGAINST  LOW
#define PIN_BUTTON_PLUS  D5
#define PIN_BUTTON_MINUS D0
#define PIN_BUTTON_MODE  D4

#elif defined(HW_QLOCKWIFIVE_MINI_V1_1)
#define PRODUCT_NAME "QlockWiFiveMini"
#define DEFAULT_LAYOUT E_LAYOUT_WIFIVE_MINI
#define LDR_IS_INVERSE false

#define PIN_IR_RECEIVER D6
#define PIN_BUZZER      D8
#define PIN_DHT22       D3
#define PIN_LEDS_CLOCK  D5 // unused
#define PIN_LEDS_DATA   D7
#define PIN_LDR         A0

#define BUTTONS
#define PLUS_BUTTON_PRESSING_AGAINST  LOW
#define MINUS_BUTTON_PRESSING_AGAINST LOW
#define MODE_BUTTON_PRESSING_AGAINST  LOW
#define PIN_BUTTON_PLUS  D5
#define PIN_BUTTON_MINUS D0
#define PIN_BUTTON_MODE  D4

#elif defined(HW_QLOCKWORK)
#define PRODUCT_NAME "Qlockwork"
#define DEFAULT_LAYOUT E_LAYOUT_DEFAULT
#define LDR_IS_INVERSE false

#define PIN_IR_RECEIVER D3
#define PIN_LED         D4
#define PIN_BUZZER      D5
#define PIN_DHT22       D6
#define PIN_LEDS_CLOCK  D7
#define PIN_LEDS_DATA   D8
#define PIN_LDR         A0

#endif

// Fernbedienungszeugs
//#define IR_REMOTE_CLT
//#define IR_REMOTE_QLOCKWIFIVE
//#define IR_REMOTE_24
//#define IR_REMOTE_44

//-----------------
//#define IR_LETTER_OFF
//#define IR_CODE_ONOFF    16769565 // HX1838 Remote CH+
//#define IR_CODE_TIME     16753245 // HX1838 Remote CH-
//#define IR_CODE_MODE     16736925 // HX1838 Remote CH
//#define IR_CODE_SETTINGS 16748655 // HX1838 Remote EQ
//#define IR_CODE_PLUS     16754775 // HX1838 Remote +
//#define IR_CODE_MINUS    16769055 // HX1838 Remote -

#ifdef IR_REMOTE_CLT
#define IR_REMOTE

#define IR_CODE_ONOFF        (0xFFE01F)  // CLT2 V1.1 Remote Power
#define IR_CODE_TIME         (0xFFA05F) // CLT2 V1.1 Remote Time
#define IR_CODE_SETTINGS     (0xFFC837) // CLT2 V1.1 Remote Mode
#define IR_CODE_PLUS         (0xFF58A7) // CLT2 V1.1 Remote Bright. Plus
#define IR_CODE_MINUS        (0xFF48B7) // CLT2 V1.1 Remote Bright. Minus

#define IR_CODE_MODE         (0xFF20DF) // CLT2 V1.1 Remote Region
#define IR_CODE_SECONDS      (0xFF609F) // CLT2 V1.1 Remote Seconds

#define IR_CODE_RED          (0xFF906F) // CLT2 V1.1 Remote Red
#define IR_CODE_GREEN        (0xFF10EF) // CLT2 V1.1 Remote Green
#define IR_CODE_DARKBLUE     (0xFF50AF) // CLT2 V1.1 Remote Darkblue
#define IR_CODE_PINK         (0xFFD02F) // CLT2 V1.1 Remote Pink
#define IR_CODE_WHITE        (0xFFB04F) // CLT2 V1.1 Remote White
#define IR_CODE_BRIGHTBLUE   (0xFF30CF) // CLT2 V1.1 Remote Brightblue
#define IR_CODE_YELLOW       (0xFF708F) // CLT2 V1.1 Remote Yellow
#define IR_CODE_RGB          (0xFFF00F) // CLT2 V1.1 Remote RGB

#define IR_CODE_NORMAL       (0xFFA857) // CLT2 V1.1 Remote Normal
#define IR_CODE_FADE         (0xFF28D7) // CLT2 V1.1 Remote Fade
#define IR_CODE_MATRIX       (0xFF6897) // CLT2 V1.1 Remote Matrix
#define IR_CODE_SLIDE        (0xFFE817) // CLT2 V1.1 Remote Slide

#define IR_CODE_HR_PLUS      (0xFF9867) // CLT2 V1.1 Remote Hour plus
#define IR_CODE_HR_MINUS     (0xFF8877) // CLT2 V1.1 Remote Hour minus
#define IR_CODE_MIN_PLUS     (0xFF18E7) // CLT2 V1.1 Remote Min plus
#define IR_CODE_MIN_MINUS    (0xFF08F7) // CLT2 V1.1 Remote Min minus

#define IR_CODE_LDR          (0xFFD827) // CLT2 V1.1 Remote LDR

#elif defined IR_REMOTE_QLOCKWIFIVE
#define IR_REMOTE

#define IR_CODE_ONOFF        (0xFFE01F) // QWF V1.0 Remote Power
#define IR_CODE_TIME         (0xFFA05F) // QWF V1.0 Remote Time
#define IR_CODE_SETTINGS     (0xFFC837) // QWF V1.0 Remote Mode
#define IR_CODE_PLUS         (0xFF58A7) // QWF V1.0 Remote Bright. Plus
#define IR_CODE_MINUS        (0xFF48B7) // QWF V1.0 Remote Bright. Minus

#define IR_CODE_MODE         (0xFFD827) // QWF V1.0 Remote Region
#define IR_CODE_SECONDS      (0xFF20DF) // QWF V1.0 Remote Seconds
#define IR_CODE_DATE         (0xFF609F) // QWF V1.0 Remote Date

#define IR_CODE_RED          (0xFF906F) // QWF V1.0 Remote Red
#define IR_CODE_GREEN        (0xFF10EF) // QWF V1.0 Remote Green
#define IR_CODE_DARKBLUE     (0xFF50AF) // QWF V1.0 Remote Darkblue
#define IR_CODE_PINK         (0xFFD02F) // QWF V1.0 Remote Pink
#define IR_CODE_WHITE        (0xFFB04F) // QWF V1.0 Remote White
#define IR_CODE_BRIGHTBLUE   (0xFF30CF) // QWF V1.0 Remote Brightblue
#define IR_CODE_YELLOW       (0xFF708F) // QWF V1.0 Remote Yellow
#define IR_CODE_ORANGE       (0xFFF00F) // QWF V1.0 Remote RGB
#define IR_CODE_RGB          (0xFFFF01) // Remote none

#define IR_CODE_NORMAL       (0xFF9867) // QWF V1.0 Remote Normal
#define IR_CODE_FADE         (0xFF18E7) // QWF V1.0 Remote Fade
#define IR_CODE_MATRIX       (0xFF8877) // QWF V1.0 Remote Matrix
#define IR_CODE_SLIDE        (0xFF08F7) // QWF V1.0 Remote Slide

#define IR_CODE_MOOD         (0xFFA857) // QWF V1.0 Remote Mood
#define IR_CODE_5MIN         (0xFF28D7) // QWF V1.0 Remote 5min
#define IR_CODE_1H           (0xFF6897) // QWF V1.0 Remote 1h
#define IR_CODE_24H          (0xFFE817) // QWF V1.0 Remote 24h

#define IR_CODE_HR_PLUS      (0xFFFFF2) // Remote none
#define IR_CODE_HR_MINUS     (0xFFFFF3) // Remote none
#define IR_CODE_MIN_PLUS     (0xFFFFF4) // Remote none
#define IR_CODE_MIN_MINUS    (0xFFFFF5) // Remote none

#define IR_CODE_LDR          (0xFFFFF6) // Remote none

#elif defined IR_REMOTE_44
#define IR_REMOTE

#define IR_CODE_ONOFF        (0xFF02FD) // CLT2 V1.1 Remote Power
#define IR_CODE_TIME         (0xFF827D) // CLT2 V1.1 Remote Play/Pause
#define IR_CODE_SETTINGS     (0xFF20DF) // CLT2 V1.1 Remote JUMP3
#define IR_CODE_PLUS         (0xFF3AC5) // CLT2 V1.1 Remote Bright. Plus
#define IR_CODE_MINUS        (0xFFBA45) // CLT2 V1.1 Remote Bright. Minus

#define IR_CODE_MODE         (0xFFE817) // CLT2 V1.1 Remote QUICK
#define IR_CODE_SECONDS      (0xFFC837) // CLT2 V1.1 Remote SLOW

#define IR_CODE_RED          (0xFF1AE5) // CLT2 V1.1 Remote R
#define IR_CODE_GREEN        (0xFF9A65) // CLT2 V1.1 Remote G
#define IR_CODE_DARKBLUE     (0xFFA25D) // CLT2 V1.1 Remote B
#define IR_CODE_PINK         (0xFF58A7) // CLT2 V1.1 Remote Pink
#define IR_CODE_WHITE        (0xFF22DD) // CLT2 V1.1 Remote W
#define IR_CODE_BRIGHTBLUE   (0xFF926D) // CLT2 V1.1 Remote Brightblue
#define IR_CODE_YELLOW       (0xFF18E7) // CLT2 V1.1 Remote Yellow
#define IR_CODE_RGB          (0xFF30CF) // CLT2 V1.1 Remote DIY1

#define IR_CODE_NORMAL       (0xFFD02F) // CLT2 V1.1 Remote FLASH
#define IR_CODE_FADE         (0xFFE01F) // CLT2 V1.1 Remote FADE7
#define IR_CODE_MATRIX       (0xFF609F) // CLT2 V1.1 Remote FADE3
#define IR_CODE_SLIDE        (0xFFA05F) // CLT2 V1.1 Remote JUMP7

#define IR_CODE_HR_PLUS      (0xFFFFFB) // CLT2 V1.1 Remote none
#define IR_CODE_HR_MINUS     (0xFFFFFC) // CLT2 V1.1 Remote none
#define IR_CODE_MIN_PLUS     (0xFFFFFD) // CLT2 V1.1 Remote none
#define IR_CODE_MIN_MINUS    (0xFFFFFE) // CLT2 V1.1 Remote none

#define IR_CODE_LDR          (0xFFF00F) // CLT2 V1.1 Remote AUTO

#elif defined IR_REMOTE_24
#define IR_REMOTE

#define IR_CODE_ONOFF        (0xF7C03F) // Remote ON
#define IR_CODE_TIME         (0xFFFFF0) // Remote none
#define IR_CODE_SETTINGS     (0xF740BF) // Remote OFF
#define IR_CODE_PLUS         (0xF700FF) // Remote Bright. Plus
#define IR_CODE_MINUS        (0xF7807F) // Remote Bright. Minus

#define IR_CODE_MODE         (0xFFFFF1) // Remote none
#define IR_CODE_SECONDS      (0xFFFFF2) // Remote none

#define IR_CODE_RED          (0xF720DF) // Remote R
#define IR_CODE_GREEN        (0xF7A05F) // Remote G
#define IR_CODE_DARKBLUE     (0xF7609F) // Remote B
#define IR_CODE_PINK         (0xF76897) // Remote 6/3
#define IR_CODE_WHITE        (0xF7E01F) // Remote W
#define IR_CODE_BRIGHTBLUE   (0xF7B04F) // Remote 4/2
#define IR_CODE_YELLOW       (0xF728D7) // Remote 6/1
#define IR_CODE_RGB          (0xF748B7) // Remote 5/3

#define IR_CODE_RED_25       (0xF710EF) // Remote 3/1
#define IR_CODE_RED_50       (0xF730CF) // Remote 4/1
#define IR_CODE_ORANGE       (0xF708F7) // Remote 5/1
#define IR_CODE_MINT         (0xF7906F) // Remote 3/2
#define IR_CODE_CYAN_25      (0xF78877) // Remote 5/2
#define IR_CODE_BLUE_25      (0xF7A857) // Remote 6/2
#define IR_CODE_BLUE_50      (0xF750AF) // Remote 3/3
#define IR_CODE_MAGENTA_25   (0xF7708F) // Remote 4/3

#define IR_CODE_NORMAL       (0xF7D02F) // Remote FLASH
#define IR_CODE_FADE         (0xF7C837) // Remote FADE
#define IR_CODE_MATRIX       (0xF7E817) // Remote SMOOTH
#define IR_CODE_SLIDE        (0xF7F00F) // Remote STROBE

#define IR_CODE_HR_PLUS      (0xFFFFF3) // Remote none
#define IR_CODE_HR_MINUS     (0xFFFFF4) // Remote none
#define IR_CODE_MIN_PLUS     (0xFFFFF5) // Remote none
#define IR_CODE_MIN_MINUS    (0xFFFFF6) // Remote none

#define IR_CODE_LDR          (0xFFFFF7) // Remote none
#endif

#endif // CONFIGURATION_H
