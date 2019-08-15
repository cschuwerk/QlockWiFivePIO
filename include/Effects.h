/**
 * Effects.h
 * Masken für diverse Effekte
 *
 * @mc       Arduino/UNO
 * @autor    Manuel Bracher / manuel.bracher@gmail.com
 * @version  1.0
 * @created  02.01.15
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 */

#ifndef EFFECTS_H
#define EFFECTS_H

#include "Renderer.h"
#include "Layouts.h"
#include "Configuration.h"
#include "Colors.h"

extern Renderer renderer;
extern layout_t layout;

#ifdef LED_LIBRARY_FASTLED
#include "LedDriver_FastLed.h"
extern LedDriver_FastLED ledDriver;
#endif
#ifdef LED_LIBRARY_LPD8806RGBW
#include "LedDriver_LPD8806RGBW.h"
extern LedDriver_LPD8806RGBW ledDriver;
#endif
#ifdef LED_LIBRARY_NEOPIXEL
#include "LedDriver_Neopixel.h"
extern LedDriver_NeoPixel ledDriver;
#endif

#define RGB_SPEED_CORRECTION 11

class Effects {
public:
	enum eEffects : uint8_t
	{
		NO_EFFECT = 255,
		EFFECT_FIREWORK = 0,
		EFFECT_HEART,
		EFFECT_CANDLE,

		BITMAP_MIN,
		BITMAP_HEART = BITMAP_MIN,
		BITMAP_DOTCIRCLE,
		BITMAP_WEDDINGRING,
		BITMAP_BDPIE,
		BITMAP_CHAMPGLASS1,
		BITMAP_CHAMPGLASS2,
		BITMAP_CHRISTTREE1,
		BITMAP_CHRISTTREE2,
		BITMAP_SMILEY,
		BITMAP_SMILEY_WINK,
		BITMAP_LOVE_U,

		ANI_BITMAP_MIN,
		ANI_BITMAP_CHAMPGLASS = ANI_BITMAP_MIN,
		ANI_BITMAP_CHRISTTREE,
		ANI_BITMAP_SMILEY_WINK,
		ANI_COUNT
	};


public:
	static void showTickerString(const char* str2disp, byte tickerSpeed, eColor color);
	static void showIntro(eColor color);

	static void showFireWork(byte posX, eColor color);
	static void showHeart(eColor color);
  static void showHeart(eColor color, unsigned int repetitions);
	static void showCandle(eColor color);
	static void showBJ(eColor color);
	static void showBitmap(byte bitmapIdx, byte duration, eColor color);
	static void showAnimatedBitmap(byte animatedBitmap, eColor color);
	static void writeToBuffer(uint16_t aMatrix[], unsigned int aDuration, eColor color);
  static void writeToBuffer(uint16_t aMatrix[], unsigned int aDuration, uint32_t color[10][11]);

};

const uint16_t effectMasksHeart[][10] = {
	{ // 0:heart small
		0b00000000000,
		0b00011011000,
		0b00111111100,
		0b00011111000,
		0b00001110000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 1:heart big
		0b00111011100,
		0b01100100110,
		0b01000000010,
		0b00100000100,
		0b00010001000,
		0b00001010000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	}
};

const uint16_t effectMasksCandle[][10] = {
	{ // 2:candle 0
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 3:candle 1&7
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00001110000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 4:candle 2&6
		0b00000000000,
		0b00000000000,
		0b00001110000,
		0b00001110000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 5:candle 3%5
		0b00000000000,
		0b00001110000,
		0b00001110000,
		0b00001110000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 6:candle 4
		0b00000100000,
		0b00001110000,
		0b00001110000,
		0b00001110000,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 7:candle base
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00001110000,
		0b00001110000,
		0b00001110000,
		0b00001110000
	}
};

const uint16_t effectMasksFireWork[][10] = {
	{ // 8:fireWork1
		0b00000000000,
		0b00000000000,
		0b00001110000,
		0b00011111000,
		0b00001110000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 9:fireWork2
		0b00000000000,
		0b00001010000,
		0b00010101000,
		0b00101110100,
		0b00010101000,
		0b00001010000,
		0b00000000000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 10:fireWork3
		0b00001010000,
		0b00010101000,
		0b00001110000,
		0b01011111010,
		0b00001110000,
		0b00010101000,
		0b00001010000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 11:fireWork4
		0b00001010000,
		0b00100100100,
		0b00001010000,
		0b00101110100,
		0b00001010000,
		0b00100100100,
		0b00001010000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 12:fireWork5
		0b00000100000,
		0b00100000100,
		0b00010101000,
		0b00101010100,
		0b00010101000,
		0b00100000100,
		0b00000100000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	},
	{ // 13:fireWork6
		0b00001010000,
		0b00100100100,
		0b00101010100,
		0b01010101010,
		0b00101010100,
		0b00100100100,
		0b00001010000,
		0b00000000000,
		0b00000000000,
		0b00000000000
	}
};

const uint16_t bitmaps[][11] = {
	{28, 62, 127, 255, 510, 1020, 510, 255, 127, 62, 28}, // ASCII-Code 0x6 =>(6) Herz
	{0, 40, 130, 0, 257, 0, 257, 0, 130, 40, 0}, // ASCII-Code 0x8 =>(8) gepunkteter Kreis
	{48, 72, 132, 132, 72, 48, 72, 132, 132, 72, 48}, // ASCII-Code 0xE =>(14) Eheringe
	{960, 1021, 960, 1018, 960, 1018, 960, 1018, 960, 1021, 960}, // ASCII-Code 0x18 => (24) Geburtstagskuchen
	{14, 540, 1020, 540, 14, 0, 7, 270, 510, 270, 7}, // ASCII-Code 0x10 =>(16) Sektgläser 1
	{7, 270, 510, 270, 7, 0, 14, 540, 1020, 540, 14}, // ASCII-Code 0x12 => (18) Sektgläser 2
	{128, 288, 328, 338, 340, 1023, 340, 338, 328, 288, 128}, // ASCII-Code 0x14 => (20) Weihnachtsbaum 1
	{0, 256, 320, 336, 340, 1022, 340, 336, 320, 256, 0}, // ASCII-Code 0x14 => (22) Weihnachtsbaum 2
	{0, 4, 138, 266, 260, 304, 260, 266, 138, 4, 0}, // ASCII-Code 0x1C => (28) Smily :-)
	{0, 68, 138, 266, 260, 304, 260, 264, 136, 68, 0}, // ASCII-Code 0x1C => (30) Smily ;-)
	{
		0b0000000111,
		0b0000000000,
		0b0000001000,
		0b0000011100,
		0b0000111100,
		0b0001111000,
		0b0000111100,
		0b0000011100,
		0b1110001000,
		0b1000000000,
		0b1110000000
	} // I Love U
};

#endif //EFFECTS_H
