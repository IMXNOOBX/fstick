#ifndef BATTERY_H
#define BATTERY_H

class Battery
{
public:
	Battery() {}

	void brightness(int ammount) {
		current_brightness = ammount;
		M5.Axp.ScreenBreath(ammount);
	}

	int getBrightness() {
		return current_brightness;
	}

	void restoreBrightness() {
		if (battery_saver)
			brightness(this->get());
		else
			brightness(100);

		Serial.println("Restored brightness to: " + String(getBrightness()) + "%");
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
	
	int getLI() { // Last Interaction
		return last_interaction;
	}

	void loop() {
		/**
		 * @brief Automatic Brightness
		 * This is not clean at all, but works 
		 */
		if (last_interaction + last_interation_timeout < millis()) {  // If last interaction was more than 20 seconds ago, adjust screen brightness
			int amount = getBrightness();

			if (amount > 75) {
				brightness(75);
			} else if (amount > 50) {
				brightness(45);
			} else if (amount > 25) {
				brightness(25);
			} else if (amount > 15) {
				brightness(15);
				last_interation_timeout += 20000; // Add some more delay before shutting it down
			} else if (battery_saver) {
				#ifndef DEV
					Serial.println("Shutting down due to inactivity...");
					M5.Axp.PowerOff(); // if there hasnt been any action by the user in 2m power off
				#endif
			}

			if (amount > 15)
				Serial.println("Adjusted brightness to: " + String(getBrightness()) + "%");

			last_interation_timeout += 20000;
		}
	}

private:
	bool battery_saver = true;

	int current_brightness = 100;

	int last_interaction = millis();
	int last_interation_timeout = 20000;
};

#endif