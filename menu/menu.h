#include "../classes/globals.h"

extern Logger l;

struct MenuAction
{
	const char *name;
	void (*action)(); // Function pointer for action
	bool isLooped;
};

struct MenuItem
{
	const char *name;
	MenuItem *submenu;
	int numOptions;
	MenuAction *actions; // Array of actions
};

class MenuRenderer
{
public:
	MenuRenderer(const char *title, MenuItem *options, int numMenuOptions)
	{
		this->title = title;
		this->menuOptions = options;
		this->numMenuOptions = numMenuOptions;
		this->currentMenuOption = 0;
		this->currentSubMenu = nullptr;

		l.log(Logger::INFO, "MenuRenderer initialized");
		l.log(Logger::INFO, "Menu title: " + String(title));
		l.log(Logger::INFO, "Menu options: " + String(numMenuOptions));
	}

	void topBar()
	{
		M5.Lcd.setTextColor(WHITE, BLACK);
		M5.Lcd.setCursor(10, 10);
		M5.Lcd.setTextSize(1);
		M5.Lcd.print(utilities::get_time_str());
		M5.Lcd.setCursor(SCREEN_WIDTH / 2 - 5, 10);
		M5.Lcd.setTextColor(BLUE);
		M5.Lcd.print("FS");

		/**
		 * @todo Clean this part :D
		 */

		int battery = utilities::get_battery();
		if (battery > 50)
			M5.Lcd.setTextColor(GREEN, BLACK);
		else if (battery > 25)
			M5.Lcd.setTextColor(YELLOW, BLACK);
		else
			M5.Lcd.setTextColor(RED, BLACK);
		M5.Lcd.setCursor(SCREEN_WIDTH - 30, 10);
		M5.Lcd.print(utilities::get_battery_str());
		M5.Lcd.setTextColor(WHITE, BLACK);
	}

	void render(bool reset = false)
	{
		if (reset)
			M5.Lcd.fillScreen(BLACK);

		topBar();
		M5.Lcd.setTextColor(WHITE);

		if (currentSubMenu == nullptr)
		{
			M5.Lcd.setTextSize(2);
			M5.Lcd.setCursor(10, 20);
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print(title);
			M5.Lcd.setTextColor(WHITE);

			M5.Lcd.setTextSize(4);
			M5.Lcd.setCursor(20, 60);
			M5.Lcd.print(menuOptions[currentMenuOption].name);

			M5.Lcd.setTextSize(1);
			M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
			M5.Lcd.print(
				"<" + String(currentMenuOption + 1) + "/" + String(numMenuOptions) +
				"> Navigate" +
				String(
					menuOptions[currentMenuOption].submenu != nullptr
						? " to submenu"
						: ""));
		}
		else
		{
			// Render the submenu
			int yOffset = 20;
			M5.Lcd.setCursor(10, yOffset);
			M5.Lcd.setTextSize(2);
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print(menuOptions[currentMenuOption].name);
			M5.Lcd.setTextColor(WHITE);
			yOffset += 20;

			// l.log(Logger::INFO, "Rendering submenu (" + String(currentSubMenuOption) + "/" + String(numSubMenuOptions) + ")");

			// for (int i = currentSubMenuOption; i < 3; i++)
			// {
			// 	if (i == currentSubMenuOption)
			// 	{
			// 		M5.Lcd.setTextColor(ORANGE);
			// 		M5.Lcd.setTextSize(3);
			// 	}
			// 	else
			// 	{
			// 		M5.Lcd.setTextColor(WHITE);
			// 		M5.Lcd.setTextSize(3);
			// 	}

			// 	M5.Lcd.print(String(i == currentSubMenuOption ? "> " : "") + currentSubMenu[i].name);
			// 	// yOffset += 20;
			// }

			int backOpt = currentSubMenuOption - 1;
			int nextOpt = currentSubMenuOption + 1;

			l.log(Logger::INFO, "Rendering submenu (" + String(currentSubMenuOption + 1) + "/" + String(numSubMenuOptions) + ") Back: " + String(backOpt) + " Next: " + String(nextOpt));

			if (backOpt >= 0)
			{
				M5.Lcd.setTextColor(WHITE);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print(currentSubMenu[backOpt].name);
				yOffset += 20;
			}

			if (currentSubMenuOption >= 0)
			{
				M5.Lcd.setTextColor(ORANGE);
				M5.Lcd.setTextSize(3);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print("> " + String(currentSubMenu[currentSubMenuOption].name));
				yOffset += 30;
			}

			if (nextOpt < numSubMenuOptions)
			{
				M5.Lcd.setTextColor(WHITE);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print(currentSubMenu[nextOpt].name);
				yOffset += 20;
			}

			if (nextOpt < numSubMenuOptions - 1)
			{
				M5.Lcd.setTextColor(WHITE);
				M5.Lcd.setTextSize(1);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print("...");
			}

			M5.Lcd.setTextColor(WHITE); // Reset text color

			M5.Lcd.setTextSize(1);
			M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
			M5.Lcd.print(String(currentSubMenuOption + 1) + "/" + String(numSubMenuOptions) + " Navigate");
		}
	}

	void select()
	{
		l.log(Logger::INFO, "Menu::select() called, user is in a " + String(currentSubMenu ? "submenu" : "menu"));
		if (currentSubMenu)
		{
			const char *name = currentSubMenu[currentSubMenuOption].name;
			l.log(Logger::INFO, "Menu::select() subaction name: " + String(name));
			if (name == "Back")
				return exitSubMenu();

			if (currentSubMenu[currentSubMenuOption].action != nullptr)
				currentSubMenu[currentSubMenuOption].action();
		}
		else
		{
			auto subMenu = menuOptions[currentMenuOption].actions;
			l.log(Logger::INFO, "Menu::select() submenu name: " + String(menuOptions[currentMenuOption].name));
			if (subMenu)
				enterSubMenu(subMenu);
		}
	}

	void nextOption()
	{
		if (currentSubMenu)
			currentSubMenuOption = (currentSubMenuOption + 1) % numSubMenuOptions;
		else
			currentMenuOption = (currentMenuOption + 1) % numMenuOptions;
	}

	void previousOption()
	{
		if (currentSubMenu)
			currentSubMenuOption = (currentSubMenuOption - 1 + numSubMenuOptions) % numSubMenuOptions;
		else
			currentMenuOption = (currentMenuOption - 1 + numMenuOptions) % numMenuOptions;
	}

	void enterSubMenu(MenuAction *submenu)
	{
		l.log(Logger::INFO, "Menu::enterSubMenu() called");
		this->currentSubMenu = submenu;
		this->currentSubMenuOption = 0;
		this->numSubMenuOptions = menuOptions[currentMenuOption].numOptions;
		l.log(Logger::INFO, "Entered submenu: " + String(menuOptions[currentMenuOption].name) + " (" + String(menuOptions[currentMenuOption].numOptions) + " options)");
	}

	void exitSubMenu()
	{
		l.log(Logger::INFO, "Menu::exitSubMenu() called");
		this->currentSubMenu = nullptr;
		this->currentSubMenuOption = 0;
		this->numSubMenuOptions = 0;
		l.log(Logger::INFO, "Menu::exitSubMenu() finished");
	}

private:
	const char *title;
	MenuItem *menuOptions;
	int numMenuOptions;
	int currentMenuOption;
	MenuAction *currentSubMenu;
	int numSubMenuOptions;
	int currentSubMenuOption;
};
