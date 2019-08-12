/******************************************************************************
Languages.h
******************************************************************************/

#ifndef LANGUAGES_H
#define LANGUAGES_H

enum eLanguage : uint8_t
{
	LANGUAGE_DE_DE,
	LANGUAGE_DE_SW,
	LANGUAGE_DE_BA,
	LANGUAGE_DE_SA,
	LANGUAGE_DE_MKF_DE,
	LANGUAGE_DE_MKF_SW,
	LANGUAGE_DE_MKF_BA,
	LANGUAGE_DE_MKF_SA,
	LANGUAGE_D3,
	LANGUAGE_CH,
	LANGUAGE_CH_GS,
	LANGUAGE_EN,
	LANGUAGE_ES,
	LANGUAGE_FR,
	LANGUAGE_IT,
	LANGUAGE_NL,
	LANGUAGE_COUNT = LANGUAGE_NL
};

const char sLanguage[][4] =
{
	{ 'D', 'E', ' ', ' ' }, // 00
	{ 'D', 'E', 'S', 'W' }, // 01
	{ 'D', 'E', 'B', 'A' }, // 02
	{ 'D', 'E', 'S', 'A' }, // 03
	{ 'M', 'K', 'D', 'E' }, // 04
	{ 'M', 'K', 'S', 'W' }, // 05
	{ 'M', 'K', 'B', 'A' }, // 06
	{ 'M', 'K', 'S', 'A' }, // 07
	{ 'D', '3', ' ', ' ' }, // 08
	{ 'C', 'H', ' ', ' ' }, // 09
	{ 'C', 'H', 'G', 'S' }, // 10
	{ 'E', 'N', ' ', ' ' }, // 11
	{ 'E', 'S', ' ', ' ' }, // 12
	{ 'F', 'R', ' ', ' ' }, // 13
	{ 'I', 'T', ' ', ' ' }, // 14
	{ 'N', 'L', ' ', ' ' }  // 15
};

const char sLanguageStr[][20] PROGMEM =
{
	{ "German" }, // 00
	{ "German SW" }, // 01
	{ "German BA" }, // 02
	{ "German SA" }, // 03
	{ "German MK" }, // 04
	{ "German MK SW" }, // 05
	{ "German MK BA" }, // 06
	{ "German MK SA" }, // 07
	{ "German MK D3"}, // 08
	{ "Swiss German" }, // 09
	{ "Swiss German GSI" }, // 10
	{ "English" }, // 11
	{ "Spanish" }, // 12
	{ "French" }, // 13
	{ "Italian" }, // 14
	{ "Dutch" }  // 15
};

const char sAnimationStr[][20] PROGMEM =
{
	{ "None" }, // 00
	{ "Firework" }, // 01
	{ "Heart" }, // 02
	{ "Candle" }, // 03
	{ "BMP Heart" }, // 05
	{ "BMP Circle"}, // 06
	{ "BMP Ring" }, // 07
	{ "BMP BD Pie" }, // 08
	{ "BMP Champ 1" }, // 09
	{ "BMP Champ 2" }, // 10
	{ "BMP Christ Tree 1" }, // 11
	{ "BMP Christ Tree 2" }, // 12
	{ "BMP Smilie" },  // 13
	{ "BMP Smilie Wink" },  // 14
	{ "BMP Love U" },  // 15
	{ "BMP Ani Champagner" },  // 16
	{ "BMP Ani Christ Tree" },  // 17
	{ "BMP Ani Smilie Wink" },  // 18
};

#ifdef LANGUAGE_ENGLISH
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'S', 'U' }, // 01
	{ 'M', 'O' }, // 02
	{ 'T', 'U' }, // 03
	{ 'W', 'E' }, // 04
	{ 'T', 'H' }, // 05
	{ 'F', 'R' }, // 06
	{ 'S', 'A' }  // 07
};
#endif

#ifdef LANGUAGE_GERMAN
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'S', 'O' }, // 01
	{ 'M', 'O' }, // 02
	{ 'D', 'I' }, // 03
	{ 'M', 'I' }, // 04
	{ 'D', 'O' }, // 05
	{ 'F', 'R' }, // 06
	{ 'S', 'A' }  // 07
};
#endif

#ifdef LANGUAGE_SPANISH
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'D', 'O' }, // 01
	{ 'L', 'U' }, // 02
	{ 'M', 'A' }, // 03
	{ 'M', 'I' }, // 04
	{ 'J', 'U' }, // 05
	{ 'V', 'I' }, // 06
	{ 'S', 'A' }  // 07
};
#endif

#ifdef LANGUAGE_FRENCH
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'D', 'I' }, // 01
	{ 'L', 'U' }, // 02
	{ 'M', 'A' }, // 03
	{ 'M', 'E' }, // 04
	{ 'J', 'E' }, // 05
	{ 'V', 'E' }, // 06
	{ 'S', 'A' }  // 07
};
#endif

#ifdef LANGUAGE_ITALIEN
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'D', 'O' }, // 01
	{ 'L', 'U' }, // 02
	{ 'M', 'A' }, // 03
	{ 'M', 'E' }, // 04
	{ 'G', 'I' }, // 05
	{ 'V', 'E' }, // 06
	{ 'S', 'A' }  // 07
};
#endif

#ifdef LANGUAGE_NETHERLANDS
const char sWeekday[][2] =
{
	{ ' ', ' ' }, // 00
	{ 'Z', 'O' }, // 01
	{ 'M', 'A' }, // 02
	{ 'D', 'I' }, // 03
	{ 'W', 'O' }, // 04
	{ 'D', 'O' }, // 05
	{ 'V', 'R' }, // 06
	{ 'Z', 'A' }  // 07
};
#endif

const char sWeatherCondition[][25] PROGMEM =
{
	{ "tornado" }, // 00
	{ "tropical storm" }, // 01
	{ "hurricane" }, // 02
	{ "severe thunderstorms" }, // 03
	{ "thunderstorms" }, // 04
	{ "mixed rain and snow" }, // 05
	{ "mixed rain and sleet" }, // 06
	{ "mixed snow and sleet" }, // 07
	{ "freezing drizzle" }, // 08
	{ "drizzle" }, // 09
	{ "freezing rain" }, // 10
	{ "showers" }, // 11
	{ "showers" }, // 12
	{ "snow flurries" }, // 13
	{ "light snow showers" }, // 14
	{ "blowing snow" }, // 15
	{ "snow" }, // 16
	{ "hail" }, // 17
	{ "sleet" }, // 18
	{ "dust" }, // 19
	{ "foggy" }, // 20
	{ "haze" }, // 21
	{ "smoky" }, // 22
	{ "blustery" }, // 23
	{ "windy" }, // 24
	{ "cold" }, // 25
	{ "cloudy" }, // 26
	{ "mostly cloudy" }, // 27
	{ "mostly cloudy" }, // 28
	{ "partly cloudy" }, // 29
	{ "partly cloudy" }, // 30
	{ "clear" }, // 31
	{ "sunny" }, // 32
	{ "fair" }, // 33
	{ "fair" }, // 34
	{ "mixed rain and hail" }, // 35
	{ "hot" }, // 36
	{ "isolated thunderstorms" }, // 37
	{ "scattered thunderstorms" }, // 38
	{ "scattered thunderstorms" }, // 39
	{ "scattered showers" }, // 40
	{ "heavy snow" }, // 41
	{ "scattered snow showers" }, // 42
	{ "heavy snow" }, // 43
	{ "partly cloudy" }, // 44
	{ "thundershowers" }, // 45
	{ "snow showers" }, // 46
	{ "isolated thundershowers" }  // 47
};

#endif
