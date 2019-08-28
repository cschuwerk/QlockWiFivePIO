/**
 Effects.cpp
 Klasse für diverse Effekte

 @mc       Arduino/UNO
 @autor    Manuel Bracher / manuel.bracher@gmail.com
 @version  1.0
 @created  02.01.15

 Versionshistorie:
 V 1.0:  - Erstellt.
 */

#include "Effects.h"
#include "Letters.h"
#include "Settings.h"

// #define DEBUG
#include "Debug.h"

extern Settings settings;

void Effects::showTickerString(const char* str2disp, byte tickerSpeed, eColor color) {
	uint16_t matrix[16];
	memset(matrix, 0, sizeof(matrix));

	byte strLength = strlen(str2disp);
	unsigned int bufLen;
	char actChar;
	char lastChar;
	byte offsetV = 2;
	bool finish = false;
	unsigned int i = 0;
	unsigned int temp_shift = 0;

	while (!finish) {
		renderer.clearScreenBuffer(matrix);
		lastChar = 'W';
		unsigned int shift = 0; // Schiebekorrektur aufgrund variierender Buchstabenbreite
		for (byte k = 0; k < strLength; k++) {
			actChar = str2disp[k];
			if (actChar == ' ') {
				shift += 3;  //bei einem Space eine Lücke von:
			} else {
				shift -= lettersBig[lastChar - '!'][7];
				for (byte j = 0; j < 7; j++) {
					temp_shift = (1 - shift + i);
					if (temp_shift < 16) {
						matrix[offsetV + j] |= (lettersBig[actChar - '!'][j] << temp_shift) & 0b1111111111100000;
					}
				}
				if (k < (strLength - 1)) {
					shift += 6; // Max. Buchstabenbreite + ein Pixel Lücke
				}
				lastChar = actChar;
			}
		}
		writeToBuffer(matrix, 3 * (10 - tickerSpeed), color);
		bufLen = shift + 15;
		if (i == bufLen) {
			finish = true;
		} else {
			i++;
		}
	}
}

/**
 Intro
 */
void Effects::showIntro(eColor color) {
	uint16_t matrix[16];

	renderer.clearScreenBuffer(matrix);
	for (int j = 0; j < 11; j++) {
		for (byte i = 0; i < 10; i++) {
			matrix[i] |= 0b1 << (15 - j);
		}
		writeToBuffer(matrix, 5, color);
	}
	for (int j = 0; j < 11; j++) {
		for (int i = 0; i < 10; i++) {
			matrix[i] ^= 0b1 << (5 + j);
		}
		writeToBuffer(matrix, 5, color);
	}
	renderer.clearScreenBuffer(matrix);
	for (int i = 9; i >= 0; i--) {
		matrix[i] |= 0b1111111111100000;
		writeToBuffer(matrix, 5, color);
	}
	for (int i = 0; i < 10; i++) {
		matrix[i] ^= 0b1111111111100000;
		writeToBuffer(matrix, 5, color);
	}
}

/**
 Pulsierender Herz-Effekt
 */
void Effects::showHeartBig(eColor color) {
	uint16_t matrix[16];
	for (byte y = 0; y < 3; y++) {
		renderer.clearScreenBuffer(matrix);
		for(byte i=0; i<4; i++) {
			for (byte j = 0; j < 8; j++) {
				matrix[1 + j] |= (effectMasksHeart[i][j] << 5);
			}
			unsigned int duration = (i==3) ? 50 : 12;
			writeToBuffer(matrix, duration, color);
		}
		renderer.clearScreenBuffer(matrix);
		for (byte j = 0; j < 8; j++) {
				matrix[1 + j] |= (effectMasksHeart[3][j] << 5);
		}
		writeToBuffer(matrix, 30, color);
		for (byte j = 0; j < 8; j++) {
				matrix[1 + j] |= (effectMasksHeart[2][j] << 5);
		}
		writeToBuffer(matrix, 200, color);
		
		/* for (byte i = 0; i < 2; i++) {
			renderer.clearScreenBuffer(matrix);
			for (byte z = 2; z < 4; z++) {
				for (byte j = 0; j < 8; j++) {
					matrix[1 + j] |= (effectMasksHeart[z][j] << 5);
				}
				if(z==2) {
					writeToBuffer(matrix, 36, color); }
				else {
					writeToBuffer(matrix, 12, color); }				
			}
		}*/
	}
	renderer.clearScreenBuffer(matrix);
	/*for (byte j = 0; j < 8; j++) {
		matrix[1 + j] |= (effectMasksHeart[0][j] << 5);
	}*/
	writeToBuffer(matrix, 48, color);
}

void Effects::showHeart(eColor color) {
	uint16_t matrix[16];
	for (byte y = 0; y < 3; y++) {
		renderer.clearScreenBuffer(matrix);
		for (byte j = 0; j < 8; j++) {
			matrix[1 + j] |= (effectMasksHeart[0][j] << 5);
		}
		writeToBuffer(matrix, 36, color);
		for (byte i = 0; i < 2; i++) {
			renderer.clearScreenBuffer(matrix);
			for (byte z = 0; z < 2; z++) {
				for (byte j = 0; j < 8; j++) {
					matrix[1 + j] |= (effectMasksHeart[z][j] << 5);
				}
				writeToBuffer(matrix, 12, color);
			}
		}
	}
	renderer.clearScreenBuffer(matrix);
	for (byte j = 0; j < 8; j++) {
		matrix[1 + j] |= (effectMasksHeart[0][j] << 5);
	}
	writeToBuffer(matrix, 48, color);
}

void Effects::showHeart(eColor color, unsigned int repetitions) {
  for(unsigned int i=0; i<repetitions; ++i) {
    Effects::showHeart(color);
  }
}

/**
 Feuerwerk-Effekt
 */
void Effects::showFireWork(byte posX, eColor color) {
	uint16_t matrix[16];

	for (byte i = 9; i >= 3; i--) {
		renderer.clearScreenBuffer(matrix);
		renderer.setPixelInScreenBuffer(posX, i, matrix);
		writeToBuffer(matrix, 7, color);
	}

	for (byte i = 0; i <= 2; i++) {
		renderer.clearScreenBuffer(matrix);
		for (byte j = 0; j < 10; j++) {
			matrix[j] |= (effectMasksFireWork[i][j] << (10 - posX)) & 0b1111111111100000;
		}
		writeToBuffer(matrix, 3 + round(10 * i / 3), color);
	}
	for (byte i = 0; i <= 10; i++) {
		renderer.clearScreenBuffer(matrix);
		for (byte j = 0; j < 10 - i; j++) {
			matrix[j + i] |= (effectMasksFireWork[3 + i % 3][j] << (10 - posX)) & 0b1111111111100000;
		}
		writeToBuffer(matrix, 20, color);
	}
}

/**
 Kerzen-Effekt
 */
void Effects::showCandle(eColor color) {
	uint16_t matrix[16];
  	uint32_t cBase = getDefaultColor(color);
  	uint32_t  cFlame = getDefaultColor(COLOR_YELLOW);
  	uint32_t pixelColor[][11] = {
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame,cFlame},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase}
    };
	for (byte k = 0; k < 5; k++) {
		for (int j = -4; j < 4; j++) {
			renderer.clearScreenBuffer(matrix);
			for (byte i = 5; i < 10; i++) {
				matrix[i] |= (effectMasksCandle[5][i] << 5);
			}
			for (byte i = 0; i < 5; i++) {
				matrix[i] |= (effectMasksCandle[4 - abs(j % 4)][i] << 5);
			}
			writeToBuffer(matrix, 10, pixelColor);
		}
	}
}

void Effects::showCoffee(eColor color, unsigned int repetitions) {
	uint16_t matrix[16];
  	uint32_t cBase = getDefaultColor(color);
  	uint32_t  cSteam = getDefaultColor(COLOR_WHITE);
	uint32_t  cCoffee = getDefaultColor(COLOR_BROWN);
  	uint32_t pixelColor[][11] = {
      {cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam},
      {cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam},
      {cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam},
      {cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam,cSteam},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cCoffee, cCoffee, cCoffee, cCoffee, cCoffee, cCoffee, cCoffee, cBase, cBase, cBase},
      {cBase, cBase, cCoffee, cCoffee, cCoffee, cCoffee, cCoffee, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase},
      {cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase, cBase}
    };
	for (byte k = 0; k < repetitions; k++) {
		
		for (int j = 1; j < 10; j++) {

			renderer.clearScreenBuffer(matrix);
			// cup
			for (byte i = 4; i < 10; i++) {
				matrix[i] |= (effectMasksCoffee[0][i] << 5);
			}
			//steam
			for (byte i = 0; i < 10; i++) {
				matrix[i] |= (effectMasksCoffee[j][i] << 5);
			}
			writeToBuffer(matrix, 10, pixelColor);
		}
	}
}

/**
 Bitmap
 */
void Effects::showBitmap(byte bitmapIdx, byte duration, eColor color) {
	uint16_t matrix[16];
	renderer.clearScreenBuffer(matrix);
	for (byte i = 0; i < 10; i++) {
		for (byte j = 0; j < 11; j++) {
			matrix[i] |= ((bitmaps[bitmapIdx - BITMAP_MIN][j] >> i) & 0x0001) << (15 - j);
		}
	}
	writeToBuffer(matrix, duration, color);
}

/**
 Bitmap-Effekt
 */
void Effects::showAnimatedBitmap(byte animatedBitmap, eColor color) {
	switch (animatedBitmap) {
	case ANI_BITMAP_CHAMPGLASS:
		for (byte i = 0; i < 6; i++) {
			showBitmap(BITMAP_CHAMPGLASS1 + i % 2, 25, color);
		}
		break;
	case ANI_BITMAP_CHRISTTREE:
		for (byte i = 0; i < 4; i++) {
			showBitmap(BITMAP_CHRISTTREE1 + i % 2, 25, color);
		}
		break;
	case ANI_BITMAP_SMILEY_WINK:
		showBitmap(BITMAP_SMILEY, 2 * 25, color);
		showBitmap(BITMAP_SMILEY_WINK, 25, color);
		showBitmap(BITMAP_SMILEY, 25, color);
		break;
	case ANI_COUNT:
		for(int i=10;i>=0;--i) {
			unsigned long ms_start = millis();
			showNumber(i,1.0,color);
			while(millis() < ms_start+1000) { continue; }
		}
		break;
	default:
		break;
	}
}

void Effects::writeToBuffer(uint16_t aMatrix[], unsigned int aDuration, eColor color) {
	ledDriver.setBrightness(settings.getBrightness());
	ledDriver.setScreenColor(getDefaultColor(color));
	ledDriver.setScreenBuffer(aMatrix);
	delay(aDuration * RGB_SPEED_CORRECTION);
}

void Effects::writeToBuffer(uint16_t aMatrix[], unsigned int aDuration, uint32_t color[10][11]) {
  ledDriver.setBrightness(settings.getBrightness());
  ledDriver.setScreenColor(color);
  ledDriver.setScreenBuffer(aMatrix);
  delay(aDuration * RGB_SPEED_CORRECTION);
}

void Effects::showNumber(int number, byte duration, eColor color)
{
	uint16_t matrix[16] = {};
	for (uint8_t i = 0; i <= 6; i++) {
		matrix[1 + i] |= numbersBig[number / 10][i] << 11;
		matrix[1 + i] |= numbersBig[number % 10][i] << 5;
	}
	writeToBuffer(matrix, duration, color);
	
}