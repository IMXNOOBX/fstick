#include "../tools/utilities.h"
#include "../classes/globals.h"

struct MenuItem
{
	const char *name;
	MenuItem *submenu;
};

class MenuRenderer
{
public:
	MenuRenderer(const char *title, MenuItem *options, int numMenuOptions)
		: title(title), menuOptions(options), numMenuOptions(numMenuOptions), currentMenuOption(0), currentSubMenu(nullptr) {}

	void topBar()
	{
		M5.Lcd.setCursor(10, 10);
		M5.Lcd.setTextSize(1);
		M5.Lcd.printf("%02d:%02d:%02d\n", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
		M5.Lcd.setCursor(SCREEN_WIDTH - 80, 10);
		M5.Lcd.print("Battery: " + utilities::get_battery_str());
	}

	void render()
	{
		M5.Lcd.fillScreen(BLACK);
		M5.Lcd.setTextColor(WHITE);
		topBar();
		// M5.Lcd.setTextSize(2);
		// M5.Lcd.setCursor(10, 20);
		// M5.Lcd.print(title);

		// M5.Lcd.setTextSize(4);
		// M5.Lcd.setCursor(20, 60);
		// M5.Lcd.print(menuOptions[currentMenuOption].name);
		// M5.Lcd.setTextSize(2);

		// if (menuOptions[currentMenuOption].submenu != nullptr)
		// {
		// 	M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
		// 	M5.Lcd.print("> Submenu");
		// }
		// else
		// {
		// 	M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
		// 	M5.Lcd.print("<" + String(currentMenuOption) + "/" + String(numMenuOptions) + "> Navigate");
		// }

		if (currentSubMenu != nullptr)
		{
			M5.Lcd.setTextSize(2);
			M5.Lcd.setCursor(10, 20);
			M5.Lcd.print(title);

			M5.Lcd.setTextSize(4);
			M5.Lcd.setCursor(20, 60);
			M5.Lcd.print(menuOptions[currentMenuOption].name);
			M5.Lcd.setTextSize(2);

			if (menuOptions[currentMenuOption].submenu != nullptr)
			{
				M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
				M5.Lcd.print("> Submenu");
			}
			else
			{
				M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
				M5.Lcd.print("<" + String(currentMenuOption) + "/" + String(numMenuOptions) + "> Navigate");
			}
		}
		else
		{
			// Render the submenu
			int yOffset = SCREEN_HEIGHT - 20;
			M5.Lcd.setCursor(10, yOffset);
			M5.Lcd.setTextSize(1);
			M5.Lcd.print("Submenu:");
			yOffset += 20;

			this->numSubMenuOptions = sizeof(currentSubMenu) / sizeof(currentSubMenu[0]);

			for (int i = 0; i < numSubMenuOptions; i++)
			{
				if (i == currentSubMenuOption)
				{
					M5.Lcd.setTextColor(ORANGE);
					M5.Lcd.print(">");
				}
				else
				{
					M5.Lcd.setTextColor(WHITE);
					M5.Lcd.print(" ");
				}
				M5.Lcd.setCursor(40, yOffset);
				M5.Lcd.print(currentSubMenu[i].name);
				yOffset += 20;
			}
		}
	}

	void select()
	{
		this->currentSubMenu = menuOptions[currentMenuOption].submenu;

		if (currentSubMenu != nullptr)
		{
			// Navigate to a submenu option
			currentSubMenuOption = (currentSubMenuOption + 1) % numSubMenuOptions;
		}
		else
		{
			// Perform the action associated with the selected option
			// For example: menuOptions[currentMenuOption].action();
		}
	}

	void nextOption()
	{
		if (currentSubMenu != nullptr)
		{
			// Navigate to the next submenu option
			currentSubMenuOption = (currentSubMenuOption + 1) % numSubMenuOptions;
		}
		else
		{
			// Navigate to the next main menu option
			currentMenuOption = (currentMenuOption + 1) % numMenuOptions;
		}
	}

	void previousOption()
	{
		if (currentSubMenu != nullptr)
		{
			// Navigate to the previous submenu option
			currentSubMenuOption = (currentSubMenuOption - 1 + numSubMenuOptions) % numSubMenuOptions;
		}
		else
		{
			// Navigate to the previous main menu option
			currentMenuOption = (currentMenuOption - 1 + numMenuOptions) % numMenuOptions;
		}
	}

	void enterSubMenu(MenuItem *submenu)
	{
		currentSubMenu = submenu;
		currentSubMenuOption = 0;
	}

	void exitSubMenu()
	{
		currentSubMenu = nullptr;
		currentSubMenuOption = 0;
	}

private:
	const char *title;
	MenuItem *menuOptions;
	int numMenuOptions;
	int currentMenuOption;
	MenuItem *currentSubMenu;
	int numSubMenuOptions;
	int currentSubMenuOption;
};
