namespace utilities
{
	int get_batery()
	{
		float b = M5.Axp.GetVbatData() * 1.1 / 1000;
		int battery = ((b - 3.0) / 1.2) * 100;
		return battery;
	}
}
