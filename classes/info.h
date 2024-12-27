#include "globals.h"

extern Logger logger;
extern Notify notify;

class CInfo
{
public:
	CInfo() {}

	void renderCredits() {
		SCREEN.setTextColor(WHITE, BLACK);

		// Display credits
		SCREEN.setTextSize(2);
		SCREEN.setCursor(10, 20);
		SCREEN.println("Nemo Project");
		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, 40);
		SCREEN.println("gh n0xa/m5stick-nemo");

		SCREEN.setTextSize(2);
		SCREEN.setCursor(10, 50);
		SCREEN.println("Penetration Tool");
		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, 70);
		SCREEN.println("gh risinek/esp32-wifi-penetration-tool");

		SCREEN.setTextSize(2);
		SCREEN.setCursor(10, 80);
		SCREEN.println("BLE Spam");
		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, 100);
		SCREEN.println("gh RapierXbox/ESP32-Sour-Apple");

		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, SCREEN_HEIGHT - 20);
		SCREEN.println("Click to return");
	}

	void render_supporters() {
		SCREEN.setTextColor(PURPLE, BLACK);

		// Display credits
		SCREEN.setTextSize(2);
		SCREEN.setCursor(10, 20);
		SCREEN.println("Killer74-hub");
		SCREEN.setTextColor(WHITE, BLACK);
		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, 40);
		SCREEN.println("For a generous donation to keep");
		SCREEN.setCursor(10, 50);
		SCREEN.println("this project going! Thank you so much!");

		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, SCREEN_HEIGHT - 20);
		SCREEN.println("Click to return");
	}

	void renderRepository() {
		SCREEN.setTextColor(WHITE, BLACK);

		#if !defined(PLUS) && !defined(PLUS2)
			SCREEN.setTextSize(1);
			SCREEN.setCursor(5, 20);
			SCREEN.println("Repository:");
			SCREEN.setCursor(5, 35);
			SCREEN.println("github.com/");
			SCREEN.setCursor(5, 45);
			SCREEN.println("IMXNOOBX/fstick");

			SCREEN.qrcode("https://github.com/IMXNOOBX/fstick", 5, SCREEN_HEIGHT - 65, 60, 4);
		#else
			SCREEN.setTextSize(2);
			SCREEN.setCursor(10, 40);
			SCREEN.println("Repository:");
			SCREEN.setTextSize(1);
			SCREEN.setCursor(10, 80);
			SCREEN.println("https://github.com");
			SCREEN.setCursor(10, 90);
			SCREEN.println("/IMXNOOBX/fstick");

			SCREEN.qrcode("https://github.com/IMXNOOBX/fstick", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5);
		#endif

		SCREEN.setCursor(10, SCREEN_HEIGHT - 20);
		SCREEN.println("Click to return");
	}

	void renderDonate() {
		SCREEN.setTextColor(WHITE, BLACK);

		#if !defined(PLUS) && !defined(PLUS2)
			SCREEN.setTextSize(1);
			SCREEN.setCursor(5, 20);
			SCREEN.println("Please donate to help");
			SCREEN.setCursor(5, 30);
			SCREEN.println("improve this project!");

			SCREEN.qrcode("https://www.paypal.com/donate/?business=imxnoobx@gmail.com&cmd=_donations", 5, SCREEN_HEIGHT - 65, 60, 4);
		#else
			SCREEN.setTextSize(1);
			SCREEN.setCursor(5, 20);
			SCREEN.println("Please consider donating so i can keep improving this project and get a flipper zero to work on more projects :D");

			SCREEN.qrcode("https://www.paypal.com/donate/?business=imxnoobx@gmail.com&cmd=_donations", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5);
		#endif

		SCREEN.setTextSize(1);
		SCREEN.setCursor(10, SCREEN_HEIGHT - 20);
		SCREEN.println("Click to return");
	}
};
