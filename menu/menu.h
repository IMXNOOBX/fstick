#include "../tools/utilities.h"
#include "../classes/globals.h"

struct MenuAction {
    const char* name;
    void (*action)(); // Function pointer for action
};

struct MenuItem {
    const char* name;
    MenuItem* submenu;
    int numOptions;
    MenuAction* actions; // Array of actions
};
class MenuRenderer
{
public:
	MenuRenderer(const char *title, MenuItem* options, int numMenuOptions)
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

			l.log(Logger::INFO, "Rendering submenu (" + String(currentSubMenuOption) + "/" + String(numSubMenuOptions) + ")");

			for (int i = 0; i < numSubMenuOptions; i++)
			{
				if (i == currentSubMenuOption)
					M5.Lcd.setTextColor(ORANGE);
				else
					M5.Lcd.setTextColor(WHITE);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print(String(i == currentSubMenuOption ? "> " : "") + currentSubMenu[i].name);
				yOffset += 20;
			}

			M5.Lcd.setTextColor(WHITE); // Reset text color

			M5.Lcd.setTextSize(1);
			M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
			M5.Lcd.print(String(currentSubMenuOption + 1) + "/" + String(numSubMenuOptions) +" Navigate");
		}
	}

	void select()
	{
		if (currentSubMenu) {
			const char* name = currentSubMenu[currentSubMenuOption].name;
			if (name == "Back")
				exitSubMenu();
		} else {
			auto subMenu = menuOptions[currentMenuOption].actions;

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

	void enterSubMenu(MenuAction* submenu)
	{
		this->currentSubMenu = submenu;
		this->currentSubMenuOption = 0;
		this->numSubMenuOptions = menuOptions[currentMenuOption].numOptions;
		l.log(Logger::INFO, "Entered submenu: " + String(menuOptions[currentMenuOption].name) + " (" + String(menuOptions[currentMenuOption].numOptions) + " options)");
	}

	void exitSubMenu()
	{
		this->currentSubMenu = nullptr;
		this->currentSubMenuOption = 0;
		this->numSubMenuOptions = 0;
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
