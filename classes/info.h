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
		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(10, 20);
		M5.Lcd.println("Nemo Project");
		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, 40);
		M5.Lcd.println("https://github.com/n0xa/m5stick-nemo");

		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(10, 50);
		M5.Lcd.println("Penetration Tool");
		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, 70);
		M5.Lcd.println("gh risinek/esp32-wifi-penetration-tool");

		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(10, 80);
		M5.Lcd.println("BLE Spam");
		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, 100);
		M5.Lcd.println("github.com/RapierXbox/ESP32-Sour-Apple");

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Click to return");
	}

	void renderRepository() {
		M5.Lcd.setTextColor(WHITE, BLACK);

		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(10, 40);
		M5.Lcd.println("Repository:");
		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, 80);
		M5.Lcd.println("https://github.com");
		M5.Lcd.setCursor(10, 90);
		M5.Lcd.println("/IMXNOOBX/fstick");

		M5.Lcd.qrcode("https://github.com/IMXNOOBX/fstick", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5);

		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Click to return");
	}

	void renderDonate() {
		M5.Lcd.setTextColor(WHITE, BLACK);

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(5, 20);
		M5.Lcd.println("Please consider donating so i can keep improving this project and get a flipper zero to work on more projects :D");

		M5.Lcd.qrcode("https://www.paypal.com/donate/?business=imxnoobx@gmail.com&cmd=_donations", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5);

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
		M5.Lcd.println("Click to return");
	}
};
