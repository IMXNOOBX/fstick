#include "globals.h"

extern Logger l;
extern Led led;

class WifiManager
{
public:
	WifiManager() {}

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
		for (int i = 0; i < 10; i++)
		{
			createAccessPoint("FS | " + generateRandomString(10), "fsstick")
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
					(ssid.c_str(), numPackets);
			}
		}
	}

private:

	void createAccessPoint(const char *apName, const char *apPassword)
	{
		WiFi.softAP(apName, apPassword);
	}

	const char* generateRandomString(int length) {
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
				// sendDeauthPacket(targetMAC.c_str());
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

/*
	void sendDeauthPacket(const char *targetMAC)
	{
		l.log(Logger::INFO, "Sending deauth packet to " + String(targetMAC));

		uint8_t packet[] = {
			0xC0, 0x00,           // Type/Subtype: Deauthentication
			0x00, 0x00,           // Duration: 0
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Receiver address (Broadcast)
			{0},                  // Sender address (Randomly generated)
			{0},                  // BSSID (same as sender address for broadcast)
			0x00, 0x00,           // Sequence control: 0
			0x01, 0x00,           // Reason code: 1 (Unspecified reason)
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
	    // esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), 0);
	}
	*/
};