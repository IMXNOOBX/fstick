#define PLUS
#if defined(PLUS)
#include <M5StickCPlus.h>
#define BIG_TEXT 4
#define MEDIUM_TEXT 3
#define SMALL_TEXT 2
#define TINY_TEXT 1
#else
#include <M5StickC.h>
#define BIG_TEXT 2
#define MEDIUM_TEXT 2
#define SMALL_TEXT 1
#define TINY_TEXT 1
#endif

#include <M5Display.h>
#include "utilities.h"

// M5Display M5;

// Define menu options
const char *menuOptions[] = {"Option 1", "Option 2", "Option 3"};
const int numOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);
int currentOption = 0;

void setup()
{
	M5.begin();
	M5.Lcd.setRotation(3); // Adjust screen rotation as needed
	M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextColor(WHITE);
	M5.Lcd.setTextSize(2);

	// Display a banner
	M5.Lcd.setCursor(20, 40);
	M5.Lcd.print("Futuristical");

	// Show time and battery information
	M5.Lcd.setCursor(2, 10);
	M5.Lcd.setTextSize(1);
  	M5.Lcd.printf("Time: %02d:%02d:%02d\n", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
	M5.Lcd.setCursor(50, 10);
	M5.Lcd.print("Batery: " + String(utilities::get_batery()) + "%");

	delay(3000);
	displayMenu();
}

/**
 * @buttons
 * BtnA: M5 button
 * BtnB: The other button
 * Axp: Power button
 */

void loop()
{
	M5.update();

	if (M5.BtnB.wasReleased())
	{
		// Previous menu option
		currentOption = (currentOption - 1 + numOptions) % numOptions;
		displayMenu();
	}

	if (M5.Axp.GetBtnPress())
	{
		// Next menu option
		currentOption = (currentOption + 1) % numOptions;
		displayMenu();
	}
}

void displayMenu()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.print(menuOptions[currentOption]);

  // Show time and battery information
  M5.Lcd.setCursor(5, 225);
  M5.Lcd.print("Time: 12:34:56"); // Replace with actual time
  M5.Lcd.setCursor(180, 225);
  M5.Lcd.print("Bat: 80%"); // Replace with actual battery level
}