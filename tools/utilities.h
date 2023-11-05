namespace utilities
{
	String get_time_str()
	{
		M5.Rtc.GetBm8563Time();
		String time = String(M5.Rtc.Hour) + ":" + String(M5.Rtc.Minute) + ":" + String(M5.Rtc.Second);
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
}
