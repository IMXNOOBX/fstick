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

#define PLUS
#define DEV // Mostly to disable battery saver and some debug messages
#include "classes/globals.h"

/**
 * @brief initialize classes and variables
 */
Led led;
Logger l;
Battery battery;

BLE b;
Info inf;
Settings cfg;
IrBlaster ir; // const uint16_t kIrSendPin = 9;  // IR Emitter Pin - M5 IR Unit
WifiManager wi;

// if (M5.Lcd.width() > 160)
extern const unsigned char logo[];
int last_update = millis() + 1000;
int is_ready = false;

/**
 * @brief Menu options
 */
MenuAction subInfraRedUtilities[] = {
    {"Back", nullptr },
    {"Spam Sig", []() { ir.sendAllPowerCodes(); }, true, true, []() { ir.sendAllPowerCodesRender(); }, &ir.send_codes },
};
MenuAction subWifiManager[] = {
    {"Back", nullptr },
    {"Scan AP", []() { wi.scanNetworks(); }, false, true, []() { wi.scanNetworksRender(); } },
	{"Spam AP", []() { wi.accessPointLoop(); }, true },
	{"Clone AP", []() { wi.cloneAPLoop(); }, true },
	{"Rogue AP", []() { wi.rogueAPloop(); }, true },
	{"Probe AP", []() { wi.probeAPloop(); }, true },
	{"Deauth", []() { wi.deauthLoop(); }, true },
};
MenuAction subBleUtils[] = {
    {"Back", nullptr },
    {"Apple Spm", []() { b.advertiseApple(); } },
    {"Android Sp", []() { b.advertiseAndroid(); } },
	{"Windows S", []() { b.advertiseWindows(); } },
	{"@everyone", []() { b.toggleAdvertiseEveryone(); }, true, true, []() { b.advertiseEveryoneRender(); }, &b.advertise_everyone },
};
MenuAction subSettingsMenu[] = {
    {"Back", nullptr },
    {"Bat Saver", []() { cfg.toggleBattSaver(); }, false, false, nullptr, new bool(cfg.battery_saver) },
    {"Sounds", []() { cfg.toggleSound(); }, false, false, nullptr, new bool(cfg.sound_enable) },
    {"Led", []() { cfg.toggleLed(); }, false, false, nullptr, new bool(cfg.led_enable) },
    {"Restart", []() { M5.Axp.DeepSleep(5); } },
    {"Shutdown", []() { M5.Axp.PowerOff(); } },
};
MenuAction subInfoMenu[] = {
    {"Back", nullptr },
    {"Donate", true, []() { inf.renderDonate(); } },
    {"Repository", true, []() { inf.renderRepository(); } },
    {"Credits", true, []() { inf.renderCredits(); } },
};

MenuItem mainMenuOptions[] = {
    {"IR Utils", nullptr, 2, subInfraRedUtilities },
    {"WiFi Mng", nullptr, 7, subWifiManager },
    {"BLE Utils", nullptr, 5, subBleUtils },
    {"Settings", nullptr, 6, subSettingsMenu },
    {"Info", nullptr, 4, subInfoMenu },
};

MenuRenderer mainMenu(NAME, mainMenuOptions, sizeof(mainMenuOptions) / sizeof(mainMenuOptions[0]));

/**
 * @brief Display startup menu
 */
void setup() {
	M5.begin();
	
	#ifdef DEV
	{
		// while (!Serial) // Wait for the serial connection to be establised.
		delay(1000);
	}
	#endif 
	l.log(Logger::INFO, "Starting " + String(NAME) + "...");

	M5.Lcd.setRotation(1);
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextColor(WHITE, BLACK);

	mainMenu.topBar();

	// Logo
	M5.Lcd.drawBitmap(10, 20, 105, 105, (uint16_t*)logo);

	M5.Lcd.setCursor(120, 40);
	M5.Lcd.setTextSize(2);
	M5.Lcd.print(NAME);
	M5.Lcd.setCursor(120, 60);
	M5.Lcd.setTextSize(1);
	M5.Lcd.println(VERSION);
	M5.Lcd.setCursor(120, 70);
	M5.Lcd.println(DEVICE);
	/**
	 * @brief initialize classes
	 */
	l.setShouldDisplayLog(true); // Set log output to screen

	if(cfg.init())
		l.log(Logger::INFO, "Settings has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize BLE");

	delay(500); // I know its not clean, but looks good for the end user

	if(ir.init())
		l.log(Logger::INFO, "IrBlaster has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize IrBlaster");

	delay(500);

	if(wi.init())
		l.log(Logger::INFO, "WifiManager has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize WifiManager");

	delay(500);

	if(b.init())
		l.log(Logger::INFO, "BLE has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize BLE");

	delay(500);

	l.setShouldDisplayLog(false); // Remove log output from screen
		
	led.flash();
	M5.Lcd.setTextSize(1);
	M5.Lcd.setCursor(120, SCREEN_HEIGHT - 20);
	M5.Lcd.print("Click to continue");
	l.log(Logger::INFO, "Menu is ready to use!");

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

    if (M5.Axp.GetBtnPress()) {
		#ifdef DEV
		{
			l.log(Logger::INFO, "Pressed Axp button to navigate to the next option");
		}
		#endif

		battery.setLI(millis());
        mainMenu.nextOption();
	    mainMenu.render(true);
		battery.restoreBrightness();
    }

	if (M5.BtnB.wasReleased()) {
		#ifdef DEV
		{
			l.log(Logger::INFO, "Pressed BtnB button to navigate to the previous option");
		}
		#endif
		
		if (millis() - battery.getLI() < 250) {
			if(cfg.getSecretCount() >= 2) {
				cfg.toggleScretMode();
				l.log(Logger::WARNING, "Secret mode has been activated, restart the device to exit it.");
			}
		} else {
			cfg.resetSecretCount();
		}
		
		battery.setLI(millis());
        mainMenu.previousOption();
		mainMenu.render(true);
		battery.restoreBrightness();
    }

    if (M5.BtnA.wasReleased()) {
		#ifdef DEV
		{
			l.log(Logger::INFO, "Pressed BtnA button to select option");
		}
		#endif

		battery.setLI(millis());
        mainMenu.select();
		mainMenu.render(true);
		mainMenu.render_feature(); // Not clean but should do its job
		battery.restoreBrightness();
    }

	if (cfg.getSecretMode()) 
		mainMenu.render_hww();
	
	if (last_update < millis()) { // refresh the screen every x seconds
		mainMenu.render_feature();
		mainMenu.topBar(); // Should be the last to be on top of everything
		last_update = millis() + 1000; // Refresh every 1s
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