#include "globals.h"

extern Logger l;
extern Led led;

class Info
{
public:
	Info() {}

	void renderCredits() {
		M5.Lcd.setTextColor(WHITE, BLACK);

		// Display credits
		M5.Lcd.setCursor(20, 20);
		M5.Lcd.println("Credits:");

		M5.Lcd.setCursor(10, 20);
		M5.Lcd.println("Nemo Project");
		M5.Lcd.setCursor(10, 40);
		M5.Lcd.println("https://github.com/n0xa/m5stick-nemo");

		M5.Lcd.setCursor(10, 60);
		M5.Lcd.println("Penetration Tool");
		M5.Lcd.setCursor(10, 80);
		M5.Lcd.println("https://github.com/risinek/esp32-wifi-penetration-tool");

		M5.Lcd.setCursor(10, 100);
		M5.Lcd.println("BLE Spam");
		M5.Lcd.setCursor(10, 120);
		M5.Lcd.println("https://github.com/RapierXbox/ESP32-Sour-Apple");

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(20, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Press any button to return");
	}

	void renderRepository() {
		M5.Lcd.setTextColor(WHITE, BLACK);

		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(10, 20);
		M5.Lcd.println("Repository:");
		M5.Lcd.setCursor(10, 40);
		M5.Lcd.println("https://github.com/IMXNOOBX/fstick");

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(20, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Press any button to return");
	}

	void renderDonate() {
		M5.Lcd.setTextColor(WHITE, BLACK);

		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(5, 20);
		M5.Lcd.println("Please consider donating so i can keep improving this project and get a flipper zero to work on more projects :D");

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(20, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Press any button to return");
	}
};
