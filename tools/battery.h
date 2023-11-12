#ifndef BATTERY_H
#define BATTERY_H

class Battery {
public:
    Battery() {}

    void brightness(int ammount) {
		current_brightness = ammount;
		M5.Axp.ScreenBreath(ammount);
	}

    int getBrightness() {
		return current_brightness;
	}

	int get() {
		float b = M5.Axp.GetVbatData() * 1.1 / 1000;
		int battery = ((b - 3.0) / 1.2) * 100;
		return battery;
	}

	String get_str() {
		float b = M5.Axp.GetVbatData() * 1.1 / 1000;
		int battery = ((b - 3.0) / 1.2) * 100;
		return String(battery) + "%";
	}

	void setBT(bool state) {
		battery_saver = state;
	}

	void setLI(int time) { // Last Interaction
		last_interaction = time;
	}

	void loop() {
        if (last_battery_loop < millis()) {
			if (battery_saver) { // hehe (100 * -1) iq xd
				brightness(this->get());
			}

			if (last_interaction + 10000 > millis())
				return; // If last interaction was less than 10 seconds ago, don't do anything

			switch (getBrightness()) {
			case 100:
				brightness(75);
				break;
			case 75:
				brightness(50);
				break;
			case 50:
				brightness(25);
				break;
			case 25:	
				brightness(10);
				break;
			}
        }

        if (last_battery_loop < millis())
			last_battery_loop = millis() + 10000; // 10s default, i will make a setting later
	}
private:
	bool battery_saver = true;

	int current_brightness = 100;
	
	int last_interaction = 0;
	int last_battery_loop = 0;

};

#endif