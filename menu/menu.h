#include "../classes/globals.h"

extern Logger logger;

enum ActionType {
	LOOP,
	TOGGLE,
	ACTION,
	ACTION_MENU
};

class MenuAction {
public:
    String name;
    void (*action)();  // Function pointer for action
	ActionType type = ActionType::ACTION;
    bool* is_active = nullptr;
    bool has_menu = false;

    void (*render)();
	void (*next)();
	void (*prev)();
	void (*select)();

    // Constructor for actions with an action function and render function
    MenuAction(String name, void (*action)() = nullptr, ActionType acType = ActionType::ACTION, bool has_menu = false, void (*render)() = nullptr, bool* is_active = nullptr) {
			this->name = name;
			this->action = action;
			this->type = acType;
			this->has_menu = has_menu;
			this->render = render;

			// If no is_active pointer is provided, create a new bool variable
            if (is_active == nullptr) this->is_active = new bool(false);
			else this->is_active = is_active;
        }

	MenuAction(String name, ActionType acType = ActionType::ACTION_MENU, bool* is_active = nullptr, void (*render)() = nullptr, void (*next)() = nullptr, void (*prev)() = nullptr, void (*select)() = nullptr) {
			this->name = name;
			this->type = acType;
			this->has_menu = true;
			this->render = render;
			this->next = next;
			this->prev = prev;
			this->select = select;

            if (is_active == nullptr) this->is_active = new bool(false);
			else this->is_active = is_active;
        }

    // Constructor for actions with no action function but with a render function
    MenuAction(String name, bool has_menu, void (*render)(), bool* is_active = nullptr) {
			this->name = name;
			this->action = nullptr;
			this->type = ActionType::ACTION;
			this->has_menu = has_menu;
			this->render = render;

            if (is_active == nullptr) this->is_active = new bool(false);
			else this->is_active = is_active;
        }

	// Constructor for actions with no action or render function
    MenuAction(String name, bool has_menu)
        : name(name), action(nullptr), type(ActionType::ACTION), has_menu(has_menu), render(nullptr) {}
};

struct MenuItem
{
	String name;
	MenuItem* submenu;
	int numOptions;
	MenuAction* actions; // Array of actions
};

class MenuRenderer
{
public:
	MenuRenderer(const char *title, MenuItem *options, int menu_size)
	{
		this->title = title;
		this->menu_options = options;
		this->menu_size = menu_size;
		this->menu_selected = 0;
		this->subm_actions = nullptr;

		#ifdef DEV
			logger.log(Logger::INFO, "MenuRenderer initialized");
			logger.log(Logger::INFO, "Menu title: " + String(title));
			logger.log(Logger::INFO, "Menu options: " + String(menu_size));
		#endif
	}

	void topBar() {
		M5.Lcd.setTextColor(WHITE, BLACK);
		M5.Lcd.setCursor(10, 10);
		M5.Lcd.setTextSize(1);
		M5.Lcd.print(utilities::get_time_str());
		M5.Lcd.setCursor(SCREEN_WIDTH / 2 - 5, 10);
		#ifdef DEV
			M5.Lcd.setTextColor(GREEN);
			M5.Lcd.print("DEV");
		#elif
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print("FS");
		#endif


		/**
		 * @todo Clean this part :D
		 * Someday it will be done, I promise
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

		if (this->subm_actions == nullptr && this->subm_options == nullptr)
		{
			M5.Lcd.setTextSize(2);
			M5.Lcd.setCursor(10, 20);
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print(title);
			M5.Lcd.setTextColor(WHITE);

			M5.Lcd.setTextSize(menu_options[menu_selected].name.length() >= 8 ? 3 : 4);
			M5.Lcd.setCursor(20, 60);
			M5.Lcd.print(menu_options[menu_selected].name);

			M5.Lcd.setTextSize(1);
			M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
			M5.Lcd.print(
				"<" + String(menu_selected + 1) + "/" + String(menu_size) +
				"> Navigate" +
				String(
					menu_options[menu_selected].submenu != nullptr
						? " to submenu"
						: ""));
		} else if (this->subm_actions) { // Render the submenu
			int yOffset = 20;
			M5.Lcd.setCursor(10, yOffset);
			M5.Lcd.setTextSize(2);
			M5.Lcd.setTextColor(BLUE);
			M5.Lcd.print(menu_options[menu_selected].name);
			M5.Lcd.setTextColor(WHITE);
			yOffset += 20;

			int backOpt = this->subm_actions_selected - 1;
			int nextOpt = this->subm_actions_selected + 1;

			#ifdef DEV
				logger.log(Logger::INFO, "Rendering submenu (" + String(this->subm_actions_selected + 1) + "/" + String(this->subm_actions_size) + ") Back: " + String(backOpt) + " Next: " + String(nextOpt));
			#endif

			if (backOpt >= 0)
			{
				M5.Lcd.setTextColor(*(subm_actions[backOpt].is_active) ? GREEN : WHITE);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print(subm_actions[backOpt].name);
				yOffset += 20;
			}

			if (this->subm_actions_selected >= 0)
			{
				String opt_name = subm_actions[subm_actions_selected].name;
				ActionType ac_type = subm_actions[subm_actions_selected].type;
				// bool is_auto = subm_actions[subm_actions_selected].isAutomatic;
				bool is_active = *(subm_actions[subm_actions_selected].is_active);
				bool has_sub = subm_actions[subm_actions_selected].has_menu;

				#ifdef DEV
					logger.log(Logger::INFO, "Option Name: " + opt_name);
					// logger.log(Logger::INFO, "Is Loop: " + String(is_loop));
					// logger.log(Logger::INFO, "Is Automatic: " + String(is_auto));
					logger.log(Logger::INFO, "Action Type: " + String(ac_type));
					logger.log(Logger::INFO, "Is Active: " + String(is_active));
					logger.log(Logger::INFO, "Has Submenu: " + String(has_sub));	
				#endif

				if (is_active && (ac_type == ActionType::LOOP || ac_type == ActionType::TOGGLE))
					M5.Lcd.setTextColor(GREEN);
				else if (ac_type == ActionType::LOOP) 
					M5.Lcd.setTextColor(DARKCYAN);
				else 
					M5.Lcd.setTextColor(ORANGE);
				
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.setTextSize(3);
				M5.Lcd.print(ac_type == ActionType::LOOP ? "o " : "> ");
				{
					bool text_oversize = opt_name.length() > 8;
					bool text_toooversize = opt_name.length() >= 12;

					if (text_oversize)
						M5.Lcd.setCursor(50, yOffset + 5);

					M5.Lcd.setTextSize(text_oversize ? 2 : 3); // To better fit each feature
					M5.Lcd.print(text_oversize ? (text_toooversize ? opt_name.substring(0, 12) : opt_name) : opt_name);

					if (text_toooversize) {
						// M5.Lcd.setCursor(50 + 12 * 10, yOffset + 10);
						M5.Lcd.setTextSize(1);
						M5.Lcd.print("...");
					}
				}
				yOffset += 30;
			}

			if (nextOpt < subm_actions_size)
			{
				M5.Lcd.setTextColor(*(subm_actions[nextOpt].is_active) ? GREEN : WHITE);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print(subm_actions[nextOpt].name);
				yOffset += 20;
			}

			if (nextOpt < subm_actions_size - 1)
			{
				M5.Lcd.setTextColor(WHITE);
				M5.Lcd.setTextSize(1);
				M5.Lcd.setCursor(20, yOffset);
				M5.Lcd.print("...");
			}

			M5.Lcd.setTextColor(WHITE); // Reset text color

			M5.Lcd.setTextSize(1);
			M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
			M5.Lcd.print(String(subm_actions_selected + 1) + "/" + String(subm_actions_size) + " Navigate");
		} else if (this->subm_options) {
				M5.Lcd.setTextSize(1);
				M5.Lcd.setCursor(10, 20);
				M5.Lcd.setTextColor(BLUE);
				M5.Lcd.print(String(title) + " > " + menu_options[menu_selected].name);
				M5.Lcd.setTextColor(WHITE);

				M5.Lcd.setTextSize(subm_options[subm_options_selected].name.length() >= 8 ? 3 : 4);
				M5.Lcd.setCursor(20, 60);
				M5.Lcd.print(subm_options[subm_options_selected].name);

				M5.Lcd.setTextSize(1);
				M5.Lcd.setCursor(10, SCREEN_HEIGHT - 20);
				M5.Lcd.print(
					"<" + String(subm_options_selected + 1) + "/" + String(subm_options_size) +
					"> Navigate" +
					String(
						menu_options[menu_selected].submenu != nullptr
							? " to submenu"
							: ""));
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

		// 				Ctrl + Click the URL
		M5.Lcd.qrcode("https://youtu.be/dQw4w9WgXcQ", SCREEN_WIDTH - 85, SCREEN_HEIGHT - 85, 80, 5);

		M5.Lcd.setTextSize(1);
		M5.Lcd.setCursor(10, SCREEN_HEIGHT - 15);
		M5.Lcd.print("Version: " + String(VERSION));
	}

	void render_feature(bool refresh = false) {
		if (!subm_actions)
			return;

		bool is_active = *(subm_actions[subm_actions_selected].is_active);
		ActionType ac_type = subm_actions[subm_actions_selected].type;
		bool has_sub = subm_actions[subm_actions_selected].has_menu;

		if ((is_active/* || ac_type == ActionType::ACTION*/) && has_sub) {
			if (!prev_feature_state)
				M5.Lcd.fillScreen(BLACK); // Reset the screen, so we dont have to do it inside the sub render

			this->topBar();

			if ((ac_type == ActionType::ACTION_MENU && !refresh) || ac_type != ActionType::ACTION_MENU) {
				subm_actions[subm_actions_selected].render();
				#ifdef DEV
					logger.log(Logger::INFO, "Feature menu is been rerendered...");
				#endif
			}
		}

		if (prev_feature_state && !is_active)
			this->render(true); // Render the menu again so we exit the feature screen

		prev_feature_state = is_active;
	}

	void select() {
		if (this->subm_actions) {
			String name = subm_actions[subm_actions_selected].name;
			ActionType ac_type = subm_actions[subm_actions_selected].type;
			bool* is_active = subm_actions[subm_actions_selected].is_active;
			bool has_sub = subm_actions[subm_actions_selected].has_menu;
			#ifdef DEV
				logger.log(Logger::INFO, "Menu::select() on action -> sub action name: " + name);
			#endif
			if (name == "Back") {
				exit_action_menu();
				goto skip;
			}

			/**
			 * @brief This should invert the state of the boolean pointer if the item is an action
			 * and if it has sub menu so we can get in and out of it
			 */
			if (ac_type == ActionType::ACTION && has_sub)
				*(is_active) = !*(is_active);

			if (ac_type == ActionType::ACTION_MENU) {
				subm_actions[subm_actions_selected].select(); // We perform the action
				return render_feature(); // We rerender for to show new changes
			}

			if (subm_actions[subm_actions_selected].action != nullptr) 
				subm_actions[subm_actions_selected].action();

			// this->render_feature();
		} else if (this->subm_options) {
			String name = subm_options[subm_options_selected].name;
			auto sub_actions = subm_options[subm_options_selected].actions;
			auto sub_menu = subm_options[subm_options_selected].submenu;
			#ifdef DEV
				logger.log(Logger::INFO, "Menu::select() on submenu -> sub option name: " + name);
			#endif
			if (name == "Back") {
				exit_sub_menu();
				goto skip;
			}
			
			if (sub_actions)
				enter_action_menu(sub_actions);
			else if (sub_menu)
				enter_sub_menu(sub_menu);
		} else {
			#ifdef DEV
				logger.log(Logger::INFO, "Menu::select() on menu -> option name: " + menu_options[menu_selected].name);
			#endif

			auto sub_actions = menu_options[menu_selected].actions;
			auto sub_menu = menu_options[menu_selected].submenu;
			
			if (sub_actions)
				enter_action_menu(sub_actions);
			else if (sub_menu) 
				enter_sub_menu(sub_menu);
		}
		
		skip:
			this->render(true);
	}

	void next_option() {
		if (subm_actions) {
			if (subm_actions[subm_actions_selected].type == ActionType::ACTION_MENU && *(subm_actions[subm_actions_selected].is_active)){
				this->render_feature();
				return subm_actions[subm_actions_selected].next();
			}

			subm_actions_selected = (subm_actions_selected + 1) % subm_actions_size;
		}
		else if (subm_options)
			subm_options_selected = (subm_options_selected + 1) % subm_options_size;
		else 
			menu_selected = (menu_selected + 1) % menu_size;

		this->render(true);
	}

	void previous_option() {
		if (subm_actions) {
			if (subm_actions[subm_actions_selected].type == ActionType::ACTION_MENU && *(subm_actions[subm_actions_selected].is_active)) {
				this->render_feature();
				return subm_actions[subm_actions_selected].prev();
			}
			subm_actions_selected = (subm_actions_selected - 1 + subm_actions_size) % subm_actions_size;
		}
		else if (subm_options)
			subm_options_selected = (subm_options_selected - 1 + subm_options_size) % subm_options_size;
		else
			menu_selected = (menu_selected - 1 + menu_size) % menu_size;
		
		this->render(true);
	}

	void enter_action_menu(MenuAction* sub_action) {
		this->subm_actions = sub_action;
		this->subm_actions_selected = 0;
		this->subm_actions_size = menu_options[menu_selected].numOptions;
		#ifdef DEV
			logger.log(Logger::INFO, "Entered action menu: " + menu_options[menu_selected].name + " (" + String(menu_options[menu_selected].numOptions) + " options)");
		#endif
	}

	void exit_action_menu() {
		this->subm_actions = nullptr;
		this->subm_actions_selected = 0;
		this->subm_actions_size = 0;
	}

	void enter_sub_menu(MenuItem* sub_menu) {
		this->subm_options = sub_menu;
		this->subm_options_selected = 0;
		this->subm_options_size = menu_options[menu_selected].numOptions;
		#ifdef DEV
			logger.log(Logger::INFO, "Entered submenu: " + menu_options[menu_selected].name + " (" + String(menu_options[menu_selected].numOptions) + " options)");
		#endif
	}

	void exit_sub_menu() {
		this->subm_options = nullptr;
		this->subm_options_selected = 0;
		this->subm_options_size = 0;
	}

	void exit_all_sub() {
		this->subm_actions = nullptr;
		this->subm_options = nullptr;

		this->subm_actions_selected = 0;
		this->subm_options_selected = 0;

		this->subm_actions_size = 0;
		this->subm_options_size = 0;
	}

private:
	const char* title;
	
	MenuItem* menu_options;
	int menu_selected;
	int menu_size;

	MenuItem* subm_options;
	int subm_options_selected;
	int subm_options_size;

	MenuAction* subm_actions;
	int subm_actions_selected;
	int subm_actions_size;

	bool prev_feature_state = false;
};
