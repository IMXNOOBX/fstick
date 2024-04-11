#ifndef GLOBAL_H
#define GLOBAL_H

// #include <M5Unified.h>

#if defined(PLUS)
	#include <M5StickCPlus.h>
	#include <M5Display.h>

	#define DEVICE "M5StickCPlus"
	#define SCREEN_WIDTH 240
	#define SCREEN_HEIGHT 135
	// #define M5_LED 9
	#define SCREEN M5.Lcd
	#define BUZZER M5.Beep
#elif defined(PLUS2)
	#include <M5StickCPlus2.h>

	#define DEVICE "M5StickCPlus2"
	#define SCREEN_WIDTH 240
	#define SCREEN_HEIGHT 135
	#define M5_LED 19
	#define BACKLIGHT 27
	#define SCREEN StickCP2.Display
	#define BUZZER StickCP2.Speaker
#else
	#include <M5StickC.h>
	#include <M5Display.h>

	#define DEVICE "M5StickC"
	#define SCREEN_WIDTH 80
	#define SCREEN_HEIGHT 160
	#define SCREEN M5.Lcd
	#define BUZZER() do {} while (0)
#endif

#define NAME "FStick"
#define VERSION "v1.3.9"

#include <EEPROM.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

#include <esp_wifi.h>
#include <NimBLEDevice.h>


#include "../tools/utilities.h"
#include "../tools/battery.h"
#include "../tools/logger.h"
#include "../tools/notify.h"

#include "../menu/menu.h"

#include "../assets/logo.h"
#include "../assets/ircodes.h"
#include "../assets/android_models.h"

#include "infrared.h"
#include "settings.h"
#include "portal.h"
#include "info.h"
#include "wifi.h"
#include "ble.h"

/**
 * @brief Declare external references to classes
 */
extern Notify notify;
extern Logger logger; 
extern Battery battery;

extern CIrBlaster ir;
extern CSettings cfg;
extern CPortal portal;
extern CInfo info;
extern CWiFi wifi;
extern CBle ble;

#endif

/**
 * @brief To keep track of the colors easily
 */
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