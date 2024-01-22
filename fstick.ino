/*
 * Copyright 2023 IMXNOOBX
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * If you use or modify this code, please include an attribution to the original author and repository.
 * This notice applies to all files within this repository
 */

// #define PLUS
#define PLUS2
#define DEV // Mostly to disable battery saver and some debug messages
#include "classes/globals.h"

/**
 * @brief initialize classes and variables
 */
Notify notify;
Logger logger;
Battery battery;

BLE b;
Info inf;
Settings cfg;
IrBlaster ir; // const uint16_t kIrSendPin = 9;  // IR Emitter Pin - M5 IR Unit
WifiManager wi;

// if (SCREEN.width() > 160)
extern const unsigned char logo[];
int i_last_update = millis() + 1000;
int is_ready = false;

/**
 * @brief Menu options
 */
MenuAction ma_sub_infrared[] = {
    {"Back", nullptr },
    {"Spam Sig", []() { ir.sendAllPowerCodes(); }, ActionType::LOOP, true, []() { ir.sendAllPowerCodesRender(); }, &ir.send_codes },
};

MenuAction ma_sub_wifi[] = {
    {"Back", nullptr },
    // {"Scan AP", []() { wi.scan_nearby_ap(); }, ActionType::ACTION, true, []() { wi.scan_nearby_ap_render(); } },
    {"Select AP", ActionType::ACTION_MENU, &wi.b_is_selecting_ap, []() { wi.scan_nearby_ap_select(); }, []() { wi.scan_nearby_ap_select(true, false); }, []() { wi.scan_nearby_ap_select(false, true); }, []() { wi.snaps_select(); } },
	{"Spam AP", []() { wi.loop_ap_spam(); }, ActionType::LOOP, false, nullptr, &wi.b_loop_spam_ap },
	{"Clone AP", []() { wi.loop_ap_clone(); }, ActionType::LOOP, false, nullptr, &wi.b_loop_clone_spam_ap },
	{"Rogue AP", []() { wi.loop_ap_rogue(); }, ActionType::LOOP, false, nullptr, &wi.b_loop_rogue_ap },
	{"Probe AP", []() { wi.loop_ap_probe(); }, ActionType::LOOP, false, nullptr, &wi.b_loop_probe_ap },
	{"Deauth", []() { wi.loop_deauth(); }, ActionType::LOOP, false, nullptr, &wi.b_loop_deauth_ap },
};

MenuAction ma_sub_ble[] = {
    {"Back", nullptr },
	{"Apple Spm", []() { b.t_advertise_apple(); }, ActionType::LOOP, false, nullptr, &b.advertise_apple },
	{"Android Sp", []() { b.t_advertise_android(); }, ActionType::LOOP, false, nullptr, &b.advertise_android },
	{"Samsung Sp", []() { b.t_advertise_samsung(); }, ActionType::LOOP, false, nullptr, &b.advertise_samsung },
	{"Windows S", []() { b.t_advertise_windows(); }, ActionType::LOOP, false, nullptr, &b.advertise_windows },
	{"@everyone", []() { b.t_advertise_everyone(); }, ActionType::LOOP, true, []() { b.r_advertise_everyone(); }, &b.advertise_everyone },
};

MenuAction ma_sub_settings[] = {
    {"Back", nullptr },
    {"Bat Saver", []() { cfg.toggleBattSaver(); }, ActionType::TOGGLE, false, nullptr, &cfg.battery_saver },
    {"Sounds", []() { cfg.toggleSound(); }, ActionType::TOGGLE, false, nullptr, &cfg.sound_enable },
    {"Led", []() { cfg.toggleLed(); }, ActionType::TOGGLE, false, nullptr, &cfg.led_enable },
    {"Time", ActionType::ACTION_MENU, &cfg.b_is_selecting_time, []() { cfg.set_hour(); }, []() { cfg.set_hour(true); }, []() { cfg.set_hour(false, true); }, []() { cfg.set_hour(false, false, true); } },
    {"Rotate Screen", []() { cfg.switchRotation(); }, ActionType::TOGGLE, false, nullptr },
	#if !defined(PLUS2)
    	{"Restart", []() { M5.Axp.DeepSleep(5); } },
    	{"Shutdown", []() { M5.Axp.PowerOff(); } },
	#else
	    {"Restart", []() { StickCP2.Power.timerSleep(5); } },
		{"Shutdown", []() { StickCP2.Power.powerOff(); } },
	#endif
};

MenuAction ma_sub_info[] = {
    {"Back", nullptr },
    {"Donate", true, []() { inf.renderDonate(); } },
    {"Supporters", true, []() { inf.render_supporters(); } },
    {"Repository", true, []() { inf.renderRepository(); } },
    {"Credits", true, []() { inf.renderCredits(); } },
};

MenuItem mi_menu[] = {
    {"Infra Red", nullptr, 2, ma_sub_infrared },
    {"WiFi", nullptr, 7, ma_sub_wifi },
    {"BLE", nullptr, 6, ma_sub_ble },
    {"Settings", nullptr, 8, ma_sub_settings },
    {"Info", nullptr, 5, ma_sub_info },
};

MenuRenderer mainMenu(NAME, mi_menu, sizeof(mi_menu) / sizeof(mi_menu[0]));

/**
 * @brief Display startup menu
 */
void setup() {
	#if !defined(PLUS2)
		M5.begin();
	#else
		auto m5cfg = M5.config();
		StickCP2.begin(m5cfg);
	#endif
	
	#ifdef DEV
		logger.log(Logger::WARNING, "This is a DEVELOPMENT build made in " + String(__DATE__) + ", its not recommended for every day use.");

		int attempts = 0;
		while (!Serial.available()) { // Wait for the serial connection to be establised.
			logger.log(Logger::WARNING, "(" + String(attempts) + ") Waiting for serial to be available...");

			if (attempts > 3)
				break;

			attempts++;

			delay(300);
		}

		notify.send(2);
	#endif 
	logger.log(Logger::INFO, "Starting " + String(NAME) + "...");

	SCREEN.setRotation(1);
	SCREEN.fillScreen(BLACK);
	SCREEN.setTextColor(WHITE, BLACK);

	mainMenu.topBar();

	// Logo
	SCREEN.drawBitmap(10, 20, 105, 105, (uint16_t*)logo);

	SCREEN.setCursor(120, 40);
	SCREEN.setTextSize(2);
	SCREEN.print(NAME);
	SCREEN.setCursor(120, 60);
	SCREEN.setTextSize(1);
	SCREEN.println(VERSION);
	SCREEN.setCursor(120, 70);
	SCREEN.println(DEVICE);
	/**
	 * @brief initialize classes
	 */
	logger.setShouldDisplayLog(true); // Set log output to screen

	if(cfg.init())
		logger.log(Logger::INFO, "Settings has been initialized successfully!");
	else
		logger.log(Logger::ERROR, "Failed to initialize BLE");

	delay(500); // I know its not clean, but looks good for the end user

	if(ir.init())
		logger.log(Logger::INFO, "IrBlaster has been initialized successfully!");
	else
		logger.log(Logger::ERROR, "Failed to initialize IrBlaster");

	delay(500);

	if(wi.init())
		logger.log(Logger::INFO, "WifiManager has been initialized successfully!");
	else
		logger.log(Logger::ERROR, "Failed to initialize WifiManager");

	delay(500);

	if(b.init())
		logger.log(Logger::INFO, "BLE has been initialized successfully!");
	else
		logger.log(Logger::ERROR, "Failed to initialize BLE");

	delay(500);

	logger.setShouldDisplayLog(false); // Remove log output from screen
		
	notify.send();
	SCREEN.setTextSize(1);	
	SCREEN.setCursor(120, SCREEN_HEIGHT - 20);
	SCREEN.print("Click to continue");
	logger.log(Logger::INFO, "Menu is ready to use!");

	/**
	 * @brief Wait until user interacts with the device
	 * @details This is to prevent the user from missing the startup screen but i have to find a better way to do this
	 */
	while (!M5.BtnA.wasReleased()) {
		M5.update();
		battery.loop();
		delay(10);
	}

	mainMenu.render(true);
}

/**
 * @brief buttons
 * BtnA: M5 button, M5.BtnA.wasReleased()
 * BtnB: The other button, M5.BtnB.wasReleased()
 * Axp: Power button, M5.Axp.GetBtnPress()
 */
void loop() {
	M5.update(); // Its necesary to update the button states

	if (cfg.getSecretMode()) 
		return;

#if !defined(PLUS2)
    if (M5.Axp.GetBtnPress()) {
		#ifdef DEV
			logger.log(Logger::INFO, "Pressed Axp button to navigate to the next option");
		#endif

		battery.setLI(millis());
        mainMenu.next_option();
		battery.restoreBrightness();
    }
#else
	// fstick.ino:215:21: error: 'class m5::Power_Class' has no member named 'wasReleased'
	// if (StickCP2.Power.wasReleased()) {
#endif

	if (M5.BtnB.wasReleased()) {
		#ifdef DEV
			logger.log(Logger::INFO, "Pressed BtnB button to navigate to the previous option");
		#endif
		
		battery.setLI(millis());
        mainMenu.previous_option();
		battery.restoreBrightness();
    }

    if (M5.BtnA.wasReleased()) {
		#ifdef DEV
			logger.log(Logger::INFO, "Pressed BtnA button to select option");
		#endif

		battery.setLI(millis());
        mainMenu.select();
		// mainMenu.render(true);
		// mainMenu.render_feature(); // Not clean but should do its job
		battery.restoreBrightness();
    }

	if (M5.BtnA.wasReleasefor(3000)) {
		cfg.toggleScretMode();
		logger.log(Logger::WARNING, "Secret mode has been activated, restart the device to exit it.");
	}

	if (cfg.getSecretMode()) 
		mainMenu.render_hww();
	
	if (i_last_update < millis()) { // refresh the screen every x seconds
		mainMenu.render_feature(true);
		mainMenu.topBar(); // Should be the last to be on top of everything
		i_last_update = millis() + 1000; // Refresh every 1s
	}

	/**
	 * @brief Loops
	 * Run the loop() function of each class every tick
	 */
	ir.loop();
	wi.loop();
	b.loop();

	battery.loop();
}