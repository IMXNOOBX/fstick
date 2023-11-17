#include "../classes/globals.h"

extern Logger l;

// struct MenuAction
// {
// 	String name;
// 	void (*action)(); // Function pointer for action
// 	bool isLoop = false;
// 	bool isActive = false;
// };

class MenuAction {
public:
    String name;
    void (*action)();  // Function pointer for action
    bool isLoop = false;
    bool* isActive = nullptr;
    bool isAutomatic = false;
    bool hasMenu = false;
    void (*render)();

    // Constructor for actions with an action function and render function
    MenuAction(String name, void (*action)() = nullptr, bool isLoop = false, bool hasMenu = false, void (*render)() = nullptr, bool* isActive = nullptr)
        : name(name), action(action), isLoop(isLoop), hasMenu(hasMenu), render(render) {
            if (isActive == nullptr) // If no isActive pointer is provided, create a new bool variable
                this->isActive = new bool(false);
        	else {
                this->isActive = isActive;
				this->isAutomatic = true;
			}
        }

    // Constructor for actions with no action function but with a render function
    MenuAction(String name, bool hasMenu, void (*render)(), bool* isActive = nullptr)
        : name(name), action(nullptr), isLoop(false), hasMenu(hasMenu), render(render) {
            if (isActive == nullptr) 
                this->isActive = new bool(false);
        	else {
                this->isActive = isActive;
				this->isAutomatic = true;
			}
        }

	// Constructor for actions with no action or render function
    MenuAction(String name, bool hasMenu)
        : name(name), action(nullptr), isLoop(false), hasMenu(hasMenu), render(nullptr) {}
};

struct MenuItem
{
	String name;
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

	void topBar() {
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
		} else if (currentSubMenu) {
			// Render the submenu
			int yOffset = 20;
			M5.Lcd.setCursor(10, yOffset);
			M5.Lcd.setTextSize(2);
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print(menuOptions[currentMenuOption].name);
			M5.Lcd.setTextColor(WHITE);
			yOffset += 20;

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
				String opt_name = currentSubMenu[currentSubMenuOption].name;
				bool is_loop = currentSubMenu[currentSubMenuOption].isLoop;
				bool is_active = currentSubMenu[currentSubMenuOption].isActive != nullptr ? *(currentSubMenu[currentSubMenuOption].isActive) : false;
				bool has_sub = currentSubMenu[currentSubMenuOption].hasMenu;
				
				if (is_active)
					M5.Lcd.setTextColor(GREEN);
				else if (is_loop) 
					M5.Lcd.setTextColor(DARKCYAN); // setTextColor(uint16_t color, [uint16_t backgroundcolor])
				else 
					M5.Lcd.setTextColor(ORANGE);
				
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.setTextSize(3);
				M5.Lcd.print("> ");
				M5.Lcd.setTextSize(opt_name.length() >= 10 ? 2 : 3); // To better fit each feature
				M5.Lcd.print(opt_name);
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

	/**
	 * @brief Secret screen to fake the device as a crypto wallet
	 * To exit this screen the user will have to restart the device.
	 */
	void render_hww() {
		this->topBar();
		M5.Lcd.fillScreen(BLACK);

		M5.Lcd.setTextSize(2);
		M5.Lcd.setTextColor(BLUE);
		M5.Lcd.setCursor(SCREEN_WIDTH / 4, 30);
		M5.Lcd.print("M5 | Wallet");
		
		M5.Lcd.setTextColor(WHITE);
		M5.Lcd.setCursor(10, 60);
		M5.Lcd.print("Type:");
		M5.Lcd.setCursor(80, 60);
		M5.Lcd.setTextColor(ORANGE);
		M5.Lcd.print("BTC");
		M5.Lcd.setTextColor(WHITE);

		M5.Lcd.setCursor(10, 80);
		M5.Lcd.setTextSize(2);
		M5.Lcd.print("Amt: 0.027");

		M5.Lcd.qrcode("bitcoin:1LbcFS7sAHTD9CglQo3STiCkV8LK4ZnXl0s", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 2);

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 15);
		M5.Lcd.print("Version: " + String(VERSION));
	}

	void render_feature() {
		if (!currentSubMenu)
			return;

		bool is_active = currentSubMenu[currentSubMenuOption].isActive != nullptr ? *(currentSubMenu[currentSubMenuOption].isActive) : false;
		bool has_sub = currentSubMenu[currentSubMenuOption].hasMenu;

		if (is_active && has_sub) {
			if (!prevFeatureState)
				M5.Lcd.fillScreen(BLACK); // Reset the screen, so we dont have to do it inside the sub render

			currentSubMenu[currentSubMenuOption].render();
		}

		if (prevFeatureState && !is_active)
			this->render(true); // Render the menu again so we exit the feature screen

		prevFeatureState = is_active;
	}

	void select() {
		// l.log(Logger::INFO, "Menu::select() called, user is in a " + String(currentSubMenu ? "submenu" : "menu"));
		if (currentSubMenu) {
			String name = currentSubMenu[currentSubMenuOption].name;
			l.log(Logger::INFO, "Menu::select() subaction name: " + name);
			if (name == "Back")
				return exitSubMenu();

			if (!currentSubMenu[currentSubMenuOption].isAutomatic) // I know its not clean, but i cant think about anything 
				if (currentSubMenu[currentSubMenuOption].isActive != nullptr)
					*(currentSubMenu[currentSubMenuOption].isActive) = !*(currentSubMenu[currentSubMenuOption].isActive);

			// if (*(currentSubMenu[currentSubMenuOption].isActive))
				if (currentSubMenu[currentSubMenuOption].action != nullptr) 
					currentSubMenu[currentSubMenuOption].action();
		} else {
			auto subMenu = menuOptions[currentMenuOption].actions;
			l.log(Logger::INFO, "Menu::select() submenu name: " + menuOptions[currentMenuOption].name);
			if (subMenu)
				enterSubMenu(subMenu);
		}
	}

	void nextOption() {
		if (currentSubMenu)
			currentSubMenuOption = (currentSubMenuOption + 1) % numSubMenuOptions;
		else
			currentMenuOption = (currentMenuOption + 1) % numMenuOptions;
	}

	void previousOption() {
		if (currentSubMenu)
			currentSubMenuOption = (currentSubMenuOption - 1 + numSubMenuOptions) % numSubMenuOptions;
		else
			currentMenuOption = (currentMenuOption - 1 + numMenuOptions) % numMenuOptions;
	}

	void enterSubMenu(MenuAction *submenu) {
		this->currentSubMenu = submenu;
		this->currentSubMenuOption = 0;
		this->numSubMenuOptions = menuOptions[currentMenuOption].numOptions;
		l.log(Logger::INFO, "Entered submenu: " + menuOptions[currentMenuOption].name + " (" + String(menuOptions[currentMenuOption].numOptions) + " options)");
	}

	void exitSubMenu() {
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
	bool prevFeatureState = false;
};
