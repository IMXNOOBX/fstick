#define PLUS
#include "classes/globals.h"

/**
 * @brief initialize classes and variables
 */
Led led;
Logger l;
IrBlaster ir; // const uint16_t kIrSendPin = 9;  // IR Emitter Pin - M5 IR Unit
WifiManager wi;
BLE b;
int last_update = millis() + 10000;
// if (M5.Lcd.width() > 160)
extern const unsigned char logo[];

/**
 * @brief Menu options
 */
MenuAction subInfraRedUtilities[] = {
    {"Back", nullptr},
    {"Off Signal", []() { ir.turnOnOff(); }},
    {"Spam Sig", []() { ir.turnOnOffLoop(); }},
};
MenuAction subWifiManager[] = {
    {"Back", nullptr},
    {"Scan AP", []() { wi.scanNetworks(); }},
	{"Create AP", []() { wi.spamAP(); }},
	{"Deauth", []() { wi.scanNetworksAndDeauth(3); }},
};
MenuAction subBleUtils[] = {
    {"Back", nullptr},
    {"Apple Spm", []() { b.advertiseApple(); }},
	{"Windows Spm", []() { b.advertiseWindows(); }},
	{"@everyone", []() { b.advertiseApple(); b.advertiseWindows(); }},
};

MenuAction subSettingsMenu[] = {
    {"Back", nullptr},
    {"Option 1", nullptr},
};

MenuItem mainMenuOptions[] = {
    {"Option 1", nullptr, 0, nullptr},
    {"IR Utils", nullptr, 3, subInfraRedUtilities},
    {"WiFi Mng", nullptr, 4, subWifiManager},.
    {"BLE Utils", nullptr, 4, subBleUtils},
    {"Settings", nullptr, 2, subSettingsMenu},
};

MenuRenderer mainMenu(NAME, mainMenuOptions, sizeof(mainMenuOptions) / sizeof(mainMenuOptions[0]));

/**
 * @brief Display startup menu
 */
void setup() {
	M5.begin();
	delay(1000); // Delay 1s ti allow serial monitor to connect
	l.log(Logger::INFO, "Starting " + String(NAME) + "...");

	M5.Lcd.setRotation(1); // Adjust screen rotation as needed
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextColor(WHITE);

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
	if(ir.init())
		l.log(Logger::INFO, "IrBlaster has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize IrBlaster");

	if(wi.init())
		l.log(Logger::INFO, "WifiManager has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize WifiManager");

	if(b.init())
		l.log(Logger::INFO, "BLE has been initialized successfully!");
	else
		l.log(Logger::ERROR, "Failed to initialize BLE");

	delay(1000);
	led.flash();
	M5.Lcd.setCursor(120, 120);
	M5.Lcd.setTextSize(1);
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
		l.log(Logger::INFO, "Pressed Axp button to navigate to the next option");
        mainMenu.nextOption();
	    mainMenu.render();
    }

	if (M5.BtnB.wasReleased()) {
		l.log(Logger::INFO, "Pressed BtnB button to navigate to the previous option");
        mainMenu.previousOption();
		mainMenu.render();
    }

    if (M5.BtnA.wasReleased()) {
		l.log(Logger::INFO, "Pressed BtnA button to select option");
        mainMenu.select();
		mainMenu.render();
    }

	// if (last_update < millis()) { // refresh the screen every second
	// 	mainMenu.render();
	// 	last_update = millis() + 10000; // Refresh every 10s
	// }
}