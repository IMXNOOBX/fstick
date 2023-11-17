#ifndef GLOBAL_H
#define GLOBAL_H

#if defined(PLUS)
	#include <M5StickCPlus.h>
	#define DEVICE "M5StickCPlus"
	#define SCREEN_WIDTH 240
	#define SCREEN_HEIGHT 135
	#define BIG_TEXT 4
	#define MEDIUM_TEXT 3
	#define SMALL_TEXT 2
	#define TINY_TEXT 1
#else
	#include <M5StickC.h>
	#define DEVICE "M5StickC"
	#define SCREEN_WIDTH 80
	#define SCREEN_HEIGHT 160
	#define BIG_TEXT 2
	#define MEDIUM_TEXT 2
	#define SMALL_TEXT 1
	#define TINY_TEXT 1
#endif

#define NAME "FStick"
#define VERSION "v1.0.2"

#include <M5Display.h>
#include <EEPROM.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFi.h>
#include <esp_wifi.h>
// #include <BLEUtils.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
#include <NimBLEDevice.h>

#include "../tools/utilities.h"
#include "../tools/battery.h"
#include "../tools/logger.h"
#include "../tools/led.h"

#include "../menu/menu.h"

#include "../assets/logo.h"
#include "../assets/ircodes.h"
#include "../assets/android_models.h"

#include "infrared.h"
#include "settings.h"
#include "info.h"
#include "wifi.h"
#include "ble.h"

/**
 * @brief Declare external references to classes
 * 
 */
extern Led led;
extern Logger l; 
extern Battery battery;
extern BLE b;
extern Info inf;
extern IrBlaster ir;
extern WifiManager wi;
extern Settings cfg;

#endif

// #define BLACK       0x0000 /*   0,   0,   0 */
// #define NAVY        0x000F /*   0,   0, 128 */
// #define DARKGREEN   0x03E0 /*   0, 128,   0 */
// #define DARKCYAN    0x03EF /*   0, 128, 128 */
// #define MAROON      0x7800 /* 128,   0,   0 */
// #define PURPLE      0x780F /* 128,   0, 128 */
// #define OLIVE       0x7BE0 /* 128, 128,   0 */
// #define LIGHTGREY   0xC618 /* 192, 192, 192 */
// #define DARKGREY    0x7BEF /* 128, 128, 128 */
// #define BLUE        0x001F /*   0,   0, 255 */
// #define GREEN       0x07E0 /*   0, 255,   0 */
// #define CYAN        0x07FF /*   0, 255, 255 */
// #define RED         0xF800 /* 255,   0,   0 */
// #define MAGENTA     0xF81F /* 255,   0, 255 */
// #define YELLOW      0xFFE0 /* 255, 255,   0 */
// #define WHITE       0xFFFF /* 255, 255, 255 */
// #define ORANGE      0xFDA0 /* 255, 180,   0 */
// #define GREENYELLOW 0xB7E0 /* 180, 255,   0 */
// #define PINK        0xFC9F