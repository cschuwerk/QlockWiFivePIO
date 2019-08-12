# QLOCKWORK
### An advanced firmware for a DIY "word-clock".

Qlockwork is an ESP8266 (NodeMCU or WeMos D1 mini) firmware for a so called "word-clock".

The clock adjusts the time and date once per hour via NTP with a time server on the Internet. If an RTC is installed, the time of the ESP is compared with the RTC every minute and adjusted in case of a deviation.

At first startup and if there is a new version of the settings the EEPROM of the clock is written with default values. From there on the settings are read from EEPROM at startup.
At startup the clock performs a short self test. The sequence of the colors should be: white, red, green and blue. The clock also shows the local IP address it received via DHCP. Use this address in a browser to access the clocks web site to control it.

WiFi manager: If the clock can not connect to any WLAN at startup, it turns on an access point. Connect a mobile phone or tablet to the AP and enter the WLAN credentials. A white "WiFi" is shown on the clock. If no WLAN is connected or the timeout has expired, there is a long beep and "WiFi" turns red. On success there are three short beeps and "WiFi" will be green. WLAN is not required. After the WLAN timeout the clock works without NTP. In this case the clock uses the optional RTC or if no RTC is present has to be set manually.

Events can be shown every five minutes on a particular day of the year as a textfeed. You can set them in "Events.h". Expand the array with events as shown in the default values. Set variables for recurrent texts to save RAM. You can set a color for every event. There is no comma behind the last entry.

Updates of the firmware could be made via USB, OTA or the clocks web-server. You will find more help and information on how to configure and compile the firmware in "Readme.md" in the zip-archive. You will also find a circuit diagram, a partslist and some pictures in the "/misc" directory. All sensors, the RTC and the buzzer are optional. The clock will run with the ESP8266 module only.

Disclaimer: Qlockwork uses lots of third party libraries. I can not guarantee the integrity of these libraries. You use Qlockwork at your own risk.

Visit this forum for comments, suggestions and bug reports: http://diskussion.christians-bastel-laden.de/viewtopic.php?f=23&t=2843

### Top features:
```
Almost no electronics needed.
Only ESP8266 (NodeMCU or WeMos D1 mini) and an LED-stripe.
Optional support for WiFi, RTC, LDR, Buzzer, temperature and humidity sensor and IR-remote.
Supports more than 30 types of LED stripes.
FastLED (RGB), LPD8806 (RGBW), NeoPixel (RGB and RGBW).
Horizontal and vertical LED layout.
Webpage to control the clock.
2 Transitions for timechange.
Room and outdoor temperature.
Room and outdoor humidity.
Textfeed for events and infos.
16 Front languages.
25 Colors.
Timer.
2 Alarmtimes.
NTP timesync.
RTC timesync as backup.
Timezones.
Automatic adjustment of daylight saving time.
USB, Over-the-air or webserver firmware updates.
WiFi manager for initial setup via accesspoint.
```
### Modes
```
Time.
AM/PM.
Seconds.
Weekday.
Date.
Room temperature (with RTC or DHT22 only).
Room humidity (with DHT22 only).
Outdoor temperature (Yahoo weather).
Outdoor humidity (Yahoo weather).
Timer.
Alarm 1.
Alarm 2.
LED-test.
```
### Settings
```
Brightness.
Timed colorchange.
Color.
Transition.
Fallbacktime.
Show temperature.
Front language.
Set time.
"It is" on/off.
Set date.
Night off time.
Day on time.
```
### Board
```
Follow the instructions here: https://github.com/esp8266/Arduino#installing-with-boards-manager
```
### Needed libraries:
```
via Arduino:
ArduinoOTA by Ivan Grokhotkov
ESP8266mDNS by Tony DiCola and Ivan Grokhotkov
ESP8266WebServer by Ivan Grokhotkov
ESP8266WiFi by Ivan Grokhotkov

via Web:
https://github.com/bracci/Adafruit_NeoPixel (ready for SK6812 RGBW)
https://github.com/adafruit/Adafruit_Sensor
https://github.com/bblanchon/ArduinoJson
https://github.com/adafruit/DHT-sensor-library
https://github.com/JChristensen/DS3232RTC
https://github.com/FastLED/FastLED
https://github.com/markszabo/IRremoteESP8266
https://github.com/bracci/LPD8806RGBW
https://github.com/arduino-libraries/RestClient
https://github.com/arcao/Syslog
https://github.com/PaulStoffregen/Time
https://github.com/JChristensen/Timezone
https://github.com/tzapu/WiFiManager

!!!!!! You can download the whole library-package from http://www.bracci.ch/downloads !!!!!!

There is a warning from FastLED about SPI when compiling. Just ignore it.

For OTA and web-server updates check out:
http://esp8266.github.io/Arduino/versions/2.3.0/doc/ota_updates/readme.html
Try "http://your_clocks_ip/update" to upload an update.
Don't forget to install Python 2.7 and to select “Add python.exe to path”.
```
### Standard modes
```
Time:                            The default mode of the clock. Shows the actual time. :)
                                 + or - to move directly to the next or previous standard mode.
Display AM/PM:                   Indicates whether it is AM or PM.
Seconds:                         Shows the seconds.
Weekday:                         Shows the weekday in the selected language.
Date:                            Shows day and month.
Title (TE MP):                   + or - to move directly to the next or previous category.
Room temperature:                Display of the measured temperature in the room (only with RTC or DHT22).
Room humidity:                   Display of the measured humidity in the room (only with DHT22).
Outdoor temperature:             Displays the temperature for a location (Yahoo Weather).
Outdorr humidity:                Humidity display for the selected location (Yahoo Weather).
Title (AL RM):                   + or - to move directly to the next or previous category.
Timer (TI):                      Sets the minute timer. Zero turns off the timer.
                                 Display of the remaining time if a timer is set.
                                 + or - to set the timer and start it.
Alarm1 (A1):                     Enable (EN) or disable (DA) alarm 1.
Alarm1:                          Set the first 24 hour alarm if A1 is EN.
Alarm2 (A2):                     Enable (EN) or disable (DA) alarm 2.
Alarm2:                          Set the second 24 hour alarm if A2 is EN.
```
### Extended modes
```
Title (MA IN):                   + or - to move directly to the next or previous category.
Autom. Helligkeitsregelung (LD): Enabled (EN) or disabled (DA).
Helligkeitsregelung:             Press + or - to change the brightness.
Color Change (CC):               Change the color in intervalls.
                                 Do not change (NO), every 5 minutes (FI), every hour (HR), every day (DY).
Color (CO):                      0: white, 1: red, 2: green, 3: blue and so on.
Transition (TR):                 Normal (NO) / Fade (FD).
Fallback (FB nn):                Time in seconds to change back to time. (0: disabled, default: 5)
Show outdoor temperature (ST):   Enable (EN) or disable (DA) temperature in time view once a minute.
Front language (DE/CH/EN/...):   Select the correct language for your frontpanel.
Title (TI ME):                   + or - to move directly to the next or previous category.
Set time:                        + or - to set the time manually. + sets the hour, - the minute.
                                 The seconds jump to zero every time you hit + or -.
"It is" (IT):                    Enable (EN) or disable (DA) "It is". It will be shown at every half and full hour anyway.
Set day (DD nn):                 + or - to set the day.
Set month (MM nn):               + or - to set the month.
Set year (YY nn):                + or - to set the year.
Title (NI OF):                   + or - or mode to set the time.
Night off time:                  + or - to set the time the clocks turns itself off at night. + sets the hour, - the minute.
Title (DY ON):                   + or - or mode to set the time.
Day on time:                     + or - to set the time the clocks turns itself on at day. + sets the hour, - the minute.
Title (TE ST):                   + or - to move directly to the next or previous category.
LED-Test:                        Moves a horizontal bar across the display.
```
### Configuration.h
```
/******************************************************************************
Software settings.
******************************************************************************/

#define HOSTNAME                   The name of the watch.
#define WIFI_AP_TIMEOUT            Time in seconds for the WiFiManager to set-up/search the WLAN.
#define OTA_PASS                   Password for "Over the Air" updates.
#define WIFI_RESET                 Reset all settings to default in WiFiManager at startup.
#define NTP_SERVER                 NTP server to be queried.
#define SELFTEST                   Test LEDs at startup. Colors are: white, red, green, blue. In this order.
#define SHOW_IP                    Show local IP at startup. Use this in the browser to access the clocks menue.
#define NONE_TECHNICAL_ZERO        Displays the zero without the diagonal line.
#define ALARM_LED_COLOR            Color of the alarm LED. If not defined the display color will be used.
                                   The possible colors are:
                                   WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
                                   GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
                                   BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
#define ABUSE_CORNER_LED_FOR_ALARM Use the upper right minute LED as alarm LED. Only works if ALARM_LED_COLOR is defined.
                                   If no alarm or timer is set the LED works as expected.
#define DEDICATION                 Show a text on the clocks webpage.
#define LOCATION                   Location for the weather conditions as entered on the Yahoo site.
                                   (Only letters, ' ', and ',' are allowed).
#define LANGUAGE_*                 Language of the clock. The front itself is set in the clocks menue.
#define UPDATE_INFO_*              The update info periodically anonymously checks if there is a firmwareupdate
                                   available. No user data is send to the host.
#define TIMEZONE_*                 The time zone in which the clock is located. Important for the UTC offset and the
                                   summer/winter time change.

/******************************************************************************
Hardware settings.
******************************************************************************/

#define ESP_LED                  Displays the function using the LED on the ESP. It flashes once a second.
#define RTC_BACKUP               Use an RTC as backup and room temperature.
#define SENSOR_DHT22             Use a DHT22 sensor module (not the plain sensor) for room temperature and humidity.

#define LDR                      Use an LDR for brightness control.
#define LDR_IS_INVERSE           Combined with #define LDR inverse the value of the LDR.
#define LDR_HYSTERESIS           Brightness control from a deviation in the range from 0 to 1023. Default: 30.
#define MIN_BRIGHTNESS           Minimum brightness of LEDs ranging from 0 to 255. Default: 10.
#define MAX_BRIGHTNESS           Maximum brightness of LEDs ranging from 0 to 255. Default 255.

#define BUZZER                   Use a buzzer to make noise for alarmtime and timer.
#define BUZZTIME_ALARM_1         Maximum time in seconds for alarm 1 to be active when not turned off manually.
#define BUZZTIME_ALARM_2         Maximum time in seconds for alarm 2 to be active when not turned off manually.
#define BUZZTIME_TIMER           Maximum time in seconds for the timer alarm to be active when not turned off manually.

#define IR_REMOTE                Use an IR remote control.
#define IR_LETTER_OFF            Turns off the LED behind the IR sensor permanently. This improves IR reception.
#define IR_CODE_*                Any remote control can be used. 6 keys are supported.
                                 Turn on "#define DEBUG" and press a button on the remote control.
                                 Then write the code displayed in the serial console to the file "Configuration.h".

#define LED_LIBRARY_LPD8806RGBW  LED Driver for LPD8806 RGBW LEDs.

#define LED_LIBRARY_NEOPIXEL     LED Driver for NeoPixel LEDs.
#define LED_DRIVER_NEO_*         Specifies the NeoPixel type in conjunction with LED_LIBRARAY_NEOPIXEL.
                                 400kHz, 800kHz, GRB, RGB, GRBW and RGBW.

#define LED_LIBRARY_FASTLED      FastLED Driver for LEDs.
#define LED_DRIVER_FAST_*        Specifies the type of LEDs in conjunction with LED_LIBRARAY_FASTLED.
                                 All LED drivers supported by FAST-LED (RGB only) can be used:
                                 APA102, APA104, APA106, DOTSTAR, DMXSIMPLE, GW6205, GW6205_400, LPD1886,
                                 LPD1886_8BIT, LPD8806, NEOPIXEL, P9813, PL9823, SK6812, SK6822, SK9822,
                                 SM16716, TM1803, TM1804, TM1809, TM1812, TM1829, UCS1903, UCS1903B,
                                 UCS1904, UCS2903, WS2801, WS2803, WS2811, WS2811_400, WS2812, WS2812B,
                                 WS2813, WS2852.

#define LED_LAYOUT_HORIZONTAL    Horizontal and corner LEDs at the end of the strip. (As seen from the front.)

111                    114                    112
   000 001 002 003 004 005 006 007 008 009 010
   021 020 019 018 017 016 015 014 013 012 011
   022 023 024 025 026 027 028 029 030 031 032
   043 042 041 040 039 038 037 036 035 034 033
   044 045 046 047 048 049 050 051 052 053 054
   065 064 063 062 061 060 059 058 057 056 055
   066 067 068 069 070 071 072 073 074 075 076
   087 086 085 084 083 082 081 080 079 078 077
   088 089 090 091 092 093 094 095 096 097 098
   109 108 107 106 105 104 103 102 101 100 099
110                                            113

#define LED_LAYOUT_HORIZONTAL and #define LED_LAYOUT_DUAL (NOT TESTED!)

222                                                    228                                                    224
223                                                    229                                                    225
   000 001 | 002 003 | 004 005 | 006 007 | 008 009 | 010 011 | 012 013 | 014 015 | 016 017 | 018 019 | 020 021
   043 042 | 041 040 | 039 038 | 037 036 | 035 034 | 033 032 | 031 030 | 029 028 | 027 026 | 025 024 | 023 022
   044 045 | 046 047 | 048 049 | 050 051 | 052 053 | 054 055 | 056 057 | 058 059 | 060 061 | 062 063 | 064 065
   087 086 | 085 084 | 083 082 | 081 080 | 079 078 | 077 076 | 075 074 | 073 072 | 071 070 | 069 068 | 067 066
   088 089 | 090 091 | 092 093 | 094 095 | 096 097 | 098 099 | 100 101 | 102 103 | 104 105 | 106 107 | 108 109
   131 130 | 129 128 | 127 126 | 125 124 | 123 122 | 121 120 | 119 118 | 117 116 | 115 114 | 113 112 | 111 110
   132 133 | 134 135 | 136 137 | 138 139 | 140 141 | 142 143 | 144 145 | 146 147 | 148 149 | 150 151 | 152 153
   175 174 | 173 172 | 171 170 | 169 168 | 167 166 | 165 164 | 163 162 | 161 160 | 159 158 | 157 156 | 155 154
   176 177 | 178 179 | 180 181 | 182 183 | 184 185 | 186 187 | 188 189 | 190 191 | 192 193 | 194 195 | 196 197
   219 218 | 217 216 | 215 214 | 213 212 | 211 210 | 209 208 | 207 206 | 205 204 | 203 202 | 201 200 | 199 198
220                                                                                                           226
221                                                                                                           227

#define LED_LAYOUT_VERTICAL      Vertical and corner LEDs within the strip. (As seen from the front.)

000                    114                    102
   001 021 022 041 042 061 062 081 082 101 103
   002 020 023 040 043 060 063 080 083 100 104
   003 019 024 039 044 059 064 079 084 099 105
   004 018 025 038 045 058 065 078 085 098 106
   005 017 026 037 046 057 066 077 086 097 107
   006 016 027 036 047 056 067 076 087 096 108
   007 015 028 035 048 055 068 075 088 095 109
   008 014 029 034 049 054 069 074 089 094 110
   009 013 030 033 050 053 070 073 090 093 111
   010 012 031 032 051 052 071 072 091 092 112
011                                           113

#define LED_LAYOUT_VERTICAL and #define LED_LAYOUT_DUAL (NOT WORKING YET!)

000                                                    228                                                    204
001                                                    229                                                    205
   002 023 | 024 043 | 044 063 | 064 083 | 084 103 | 104 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   003 022 | 025 042 | 045 062 | 065 082 | 085 100 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   004 021 | 026 041 | 046 061 | 066 081 | xxx 099 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   005 020 | 027 040 | 047 060 | 067 080 | 085 098 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   006 019 | 028 039 | 048 059 | 068 079 | 086 097 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   007 018 | 029 038 | 049 058 | 069 078 | 087 096 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   008 017 | 030 037 | 050 057 | 070 077 | 088 095 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   009 016 | 031 036 | 051 056 | 071 076 | 089 094 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   010 015 | 032 035 | 052 055 | 072 075 | 090 093 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
   011 014 | 033 034 | 053 054 | 073 074 | 091 092 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101 | 082 101
012                                                                                                           226
013                                                                                                           227

/******************************************************************************
Default values for EEPROM.
******************************************************************************/

#define DEFAULT_BRIGHTNESS
#define DEFAULT_COLOR
#define DEFAULT_COLORCHANGE
#define DEFAULT_SHOWTEMP
#define DEFAULT_SHOWITIS
#define DEFAULT_TRANSITION
#define DEFAULT_TIMEOUT
#define DEFAULT_USELDR

/******************************************************************************
Misc.
******************************************************************************/

#define DEBUG                    Outputs technical information in the serial console.
#define DEBUG_WEBSITE            Provides technical information on the web page.
#define DEBUG_MATRIX             Renders the output of the matrix for the German front in the serial console.
#define DEBUG_FPS                Number of loops per second.

#define SYSLOG_SERVER            Address of a syslog server for writing debug infos.
#define SYSLOG_PORT              Port of the syslog server.

#define UPDATE_INFOSERVER        Address of the updateinfo server.
#define UPDATE_INFOFILE          Path to the updateinfo file.

#define SERIAL_SPEED             Serial port speed for the console.

#define PIN_IR_RECEIVER D3       Pin for the IR receiver.
#define PIN_LED         D4       Pin for the LED (build into the ESP).
#define PIN_BUZZER      D5       Pin for the buzzer.
#define PIN_DHT22       D6       Pin for the DHT22 module.
#define PIN_LEDS_CLOCK  D7       Pin for the LED stripe "clock" if needed.
#define PIN_LEDS_DATA   D8       Pin for the LED stripe "data".
#define PIN_LDR         A0       Pin for the LDR.
```
### Events.h
```
const event_t event[]            Display a textfeed every 5 minutes on a particular day of year.
                                 The format of an entry in the array is:
                                 { month, day, "Text to display.", year, color },
                                 The last entry has no comma.
                                 The possible colors are:
                                 WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
                                 GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
                                 BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
```
### Hardware setup of the ESP:
```
D0 = GPIO16 = NodeMCU_LED
D1 = GPIO05 = PIN_WIRE_SCL
D2 = GPIO04 = PIN_WIRE_SDA
D3 = GPIO00 = PIN_IR_RECEIVER
D4 = GPIO02 = ESP8266_LED
D5 = GPIO14 = PIN_BUZZER
D6 = GPIO12 = PIN_DHT22
D7 = GPIO13 = PIN_LEDS_CLOCK
D8 = GPIO15 = PIN_LEDS_DATA
A0 = ADC0   = PIN_LDR
```
