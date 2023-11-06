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

#include "../tools/utilities.h"
#include "../tools/logger.h"
#include "../tools/led.h"


#include "../menu/menu.h"

#include "../assets/logo.h"
// #include "../assets/ircodes.h"

#include "infrared.h"
#include "wifi.h"

// extern "C" {
// 	#include "esp_wifi.h"
// 	esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
// 	esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
// }

extern Logger l; // Declare an external reference to Logger
extern IrBlaster ir; // Declare an external reference to IrBlaster
extern Led led; // Declare an external reference to Led
extern WifiManager wi; // Declare an external reference to WifiManager

#endif
