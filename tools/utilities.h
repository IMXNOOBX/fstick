namespace utilities
{
	String get_time_str()
	{
		M5.Rtc.GetBm8563Time();
		String time = String(M5.Rtc.Hour) + ":" + String(M5.Rtc.Minute) + ":" + String(M5.Rtc.Second);
		if (time.length() < 8)
			time += "  ";
		return time;
	}

	int get_battery()
	{
		float b = M5.Axp.GetVbatData() * 1.1 / 1000;
		int battery = ((b - 3.0) / 1.2) * 100;
		return battery;
	}

	String get_battery_str()
	{
		float b = M5.Axp.GetVbatData() * 1.1 / 1000;
		int battery = ((b - 3.0) / 1.2) * 100;
		return String(battery) + "%";
	}
	
	String gen_random_str(int length) {
		const char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		const int charactersLength = sizeof(characters) - 1;
		String randomString = "";

		for (int i = 0; i < length; i++) {
			int randomIndex = random(charactersLength);
			randomString += characters[randomIndex];
		}

		return randomString;
	}

	uint8_t* rand_mac() {
		static uint8_t macAddr[6];
		randomSeed(micros());
		for (int i = 0; i < 6; i++) {
			macAddr[i] = random(0, 256);
		}

		return macAddr;
	}
}
