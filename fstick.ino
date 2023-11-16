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
Settings cfg;
IrBlaster ir; // const uint16_t kIrSendPin = 9;  // IR Emitter Pin - M5 IR Unit
WifiManager wi;

// if (M5.Lcd.width() > 160)
extern const unsigned char logo[];
int last_update = millis() + 1000;

/**
 * @brief Menu options
 */
MenuAction subInfraRedUtilities[] = {
    {"Back", nullptr },
    {"Spam Sig", []() { ir.sendAllPowerCodes(); }, false, true, []() { ir.sendAllPowerCodesRender(); }, &ir.send_codes },
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
    {"Bat Saver", []() { cfg.setBattSaver(!cfg.getBattSaver()); }, false, false, []() { }, &cfg.battery_saver },
    {"Sounds", []() { cfg.setSound(!cfg.getSound()); }, false, false, []() { }, &cfg.sound_enable },
    {"Led", []() { cfg.setLed(!cfg.getLed()); }, false, false, []() { }, &cfg.led_enable },
    {"Restart", []() { M5.Axp.DeepSleep(5); } },
    {"Shutdown", []() { M5.Axp.PowerOff(); } },
};

MenuItem mainMenuOptions[] = {
    {"IR Utils", nullptr, 2, subInfraRedUtilities },
    {"WiFi Mng", nullptr, 7, subWifiManager },
    {"BLE Utils", nullptr, 5, subBleUtils },
    {"Settings", nullptr, 6, subSettingsMenu },
};

MenuRenderer mainMenu(NAME, mainMenuOptions, sizeof(mainMenuOptions) / sizeof(mainMenuOptions[0]));

/**
 * @brief Display startup menu
 */
void setup() {
	M5.begin();
	// while (!Serial) // Wait for the serial connection to be establised.
	delay(1000); 
	l.log(Logger::INFO, "Starting " + String(NAME) + "...");

	M5.Lcd.setRotation(1); // Adjust screen rotation as needed
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextColor(WHITE, BLACK);

	// Show time and battery information
	mainMenu.topBar();

	// Logo
	M5.Lcd.drawBitmap(10, 20, 105, 105, (uint16_t*)logo);

	// Display a banner
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
}

/**
 * @brief buttons
 * BtnA: M5 button, M5.BtnA.wasReleased()
 * BtnB: The other button, M5.BtnB.wasReleased()
 * Axp: Power button, M5.Axp.GetBtnPress()
 */
void loop() {
	M5.update(); // Its necesary to update the button states

    // Implement button handling to navigate the menu and perform actions
    if (M5.Axp.GetBtnPress()) {
		// l.log(Logger::INFO, "Pressed Axp button to navigate to the next option");
		battery.setLI(millis());
        mainMenu.nextOption();
	    mainMenu.render(true);
    }

	if (M5.BtnB.wasReleased()) {
		// l.log(Logger::INFO, "Pressed BtnB button to navigate to the previous option");
		battery.setLI(millis());
        mainMenu.previousOption();
		mainMenu.render(true);
    }

    if (M5.BtnA.wasReleased()) {
		// l.log(Logger::INFO, "Pressed BtnA button to select option");
		battery.setLI(millis());
        mainMenu.select();
		mainMenu.render(true);
		mainMenu.render_feature(); // Not clean but should do its job
    }

	if (last_update < millis()) { // refresh the screen every x seconds
		// mainMenu.render();
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