#include "../classes/globals.h"

extern Logger l;

enum ActionType {
	LOOP,
	TOGGLE,
	ACTION
};

class MenuAction {
public:
    String name;
    void (*action)();  // Function pointer for action
    // bool isLoop = false;
	ActionType type = ActionType::ACTION;
    bool* isActive = nullptr;
    // bool isAutomatic = true;
    bool hasMenu = false;
    void (*render)();

    // Constructor for actions with an action function and render function
    MenuAction(String name, void (*action)() = nullptr, ActionType acType = ActionType::ACTION, bool hasMenu = false, void (*render)() = nullptr, bool* isActive = nullptr)
        : name(name), action(action), type(acType), hasMenu(hasMenu), render(render) {
            if (isActive == nullptr) // If no isActive pointer is provided, create a new bool variable
                this->isActive = new bool(false);
        	else 
                this->isActive = isActive;
        }

    // Constructor for actions with no action function but with a render function
    MenuAction(String name, bool hasMenu, void (*render)(), bool* isActive = nullptr)
        : name(name), action(nullptr), type(ActionType::ACTION), hasMenu(hasMenu), render(render) {
            if (isActive == nullptr) 
                this->isActive = new bool(false);
        	else 
                this->isActive = isActive;
        }

	// Constructor for actions with no action or render function
    MenuAction(String name, bool hasMenu)
        : name(name), action(nullptr), type(ActionType::ACTION), hasMenu(hasMenu), render(nullptr) {}
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

		#ifdef DEV
		{
			l.log(Logger::INFO, "MenuRenderer initialized");
			l.log(Logger::INFO, "Menu title: " + String(title));
			l.log(Logger::INFO, "Menu options: " + String(numMenuOptions));
		}
		#endif
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
		} else if (currentSubMenu) { // Render the submenu
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
				ActionType ac_type = currentSubMenu[currentSubMenuOption].type;
				// bool is_auto = currentSubMenu[currentSubMenuOption].isAutomatic;
				bool is_active = *(currentSubMenu[currentSubMenuOption].isActive);
				bool has_sub = currentSubMenu[currentSubMenuOption].hasMenu;

				l.log(Logger::INFO, "Option Name: " + opt_name);
				// l.log(Logger::INFO, "Is Loop: " + String(is_loop));
				// l.log(Logger::INFO, "Is Automatic: " + String(is_auto));
				l.log(Logger::INFO, "Action Type: " + String(ac_type));
				l.log(Logger::INFO, "Is Active: " + String(is_active));
				l.log(Logger::INFO, "Has Submenu: " + String(has_sub));	

				if (is_active && (ac_type == ActionType::LOOP || ac_type == ActionType::TOGGLE))
					M5.Lcd.setTextColor(GREEN);
				else if (ac_type == ActionType::LOOP) 
					M5.Lcd.setTextColor(DARKCYAN);
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
		M5.Lcd.fillScreen(BLACK);
		this->topBar();

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

		M5.Lcd.qrcode("https://youtu.be/dQw4w9WgXcQ", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5); // Ctrl + Click the URL

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 15);
		M5.Lcd.print("Version: " + String(VERSION));
	}

	void render_feature() {
		if (!currentSubMenu)
			return;

		bool is_active = *(currentSubMenu[currentSubMenuOption].isActive);
		ActionType ac_type = currentSubMenu[currentSubMenuOption].type;
		bool has_sub = currentSubMenu[currentSubMenuOption].hasMenu;

		if ((is_active/* || ac_type == ActionType::ACTION*/) && has_sub) {
			if (!prevFeatureState)
				M5.Lcd.fillScreen(BLACK); // Reset the screen, so we dont have to do it inside the sub render

			this->topBar();
			currentSubMenu[currentSubMenuOption].render();
		}

		if (prevFeatureState && !is_active)
			this->render(true); // Render the menu again so we exit the feature screen

		prevFeatureState = is_active;
	}

	void select() {
		if (currentSubMenu) {
			String name = currentSubMenu[currentSubMenuOption].name;
			ActionType ac_type = currentSubMenu[currentSubMenuOption].type;
			bool* is_active = currentSubMenu[currentSubMenuOption].isActive;
			bool has_sub = currentSubMenu[currentSubMenuOption].hasMenu;
			l.log(Logger::INFO, "Menu::select() subaction name: " + name);
			if (name == "Back")
				return exitSubMenu();

			/**
			 * @brief This should invert the state of the boolean pointer if the item is an action
			 * and if it has sub menu so we can get in and out of it
			 */
			if (ac_type == ActionType::ACTION && has_sub)
				*(is_active) = !*(is_active);

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
