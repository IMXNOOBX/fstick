#ifndef BATTERY_H
#define BATTERY_H

class Battery
{
private:
	int last_interaction = millis();
	bool battery_saver = false;
	int brightness = 12;

public:
	void setLI(int time) {
		last_interaction = time;
	}

	void setBrightness(int amount) {
		brightness = amount;
		#if defined(PLUS2)
			StickCP2.Display.setBrightness(amount);
		#else
			M5.Axp.ScreenBreath(amount);
		#endif
	}

	void restoreBrightness() {
		setBrightness(brightness);
	}

	int getBattery() {
		float b;
		#if defined(PLUS2)
			b = StickCP2.Power.getBatteryVoltage() * 1.1 / 1000;
		#else
			b = M5.Axp.GetVbatData() * 1.1 / 1000;
		#endif

		int battery = ((b - 3.0) / 1.2) * 100;
		
		if (battery > 100 || battery < 0)
			return -1;

		return battery;
	}

	void loop() {
		if (battery_saver) {
			if (millis() - last_interaction > 10000) { // if there hasnt been any action by the user in 10s dim the screen
				setBrightness(8);
			}

			if (millis() - last_interaction > 120000) { // if there hasnt been any action by the user in 2m power off
				#if defined(PLUS2)
					StickCP2.Power.powerOff();
				#else
					M5.Axp.PowerOff();
				#endif
			}
		}
	}
};

#endif