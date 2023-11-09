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

#define NAME "FS Stick"
#define VERSION "v1.0.2"

#include <M5Display.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFi.h>
#include <esp_wifi.h>
// #include <BLEUtils.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
#include <NimBLEDevice.h>

#include "../tools/utilities.h"
#include "../tools/logger.h"
#include "../tools/led.h"


#include "../menu/menu.h"

#include "../assets/logo.h"
#include "../assets/ircodes.h"

#include "infrared.h"
#include "wifi.h"
#include "ble.h"


extern Logger l; // Declare an external reference to Logger
extern Led led; // Declare an external reference to Led
extern IrBlaster ir; // Declare an external reference to IrBlaster
extern WifiManager wi; // Declare an external reference to WifiManager
extern BLE b; // Declare an external reference to BLE

#endif
