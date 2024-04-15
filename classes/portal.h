#include "globals.h"

/**
 * @brief Docs and other useful information
 * @details WebServer https://github.com/khoih-prog/WiFiWebServer
 * @details DNSServer
 *
 */
extern Logger logger;
extern Notify notify;
CWiFi cwifi;

class CPortal
{
public:
	CPortal() : gateway(192, 168, 0, 1), mask(255, 255, 255, 0) {}

	void run()
	{
		if (menu_is_running)
			return this->stop();

		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(gateway, gateway, mask);
		WiFi.softAP("TEST");

		esp_wifi_get_mac(WIFI_IF_AP, u8t_mac_ap);

		logger.log(Logger::INFO, "Creating Captive Portal on " + String(WiFi.localIP()) + " with gateway " + String(gateway) + " and mask " + String(mask));

		webServer = new WebServer(http_port);
		dnsServer.start(dns_port, "*", gateway);

		webServer->on("/goto", [this]() { 
			logger.log(Logger::INFO, "Received request in /goto endpoint.");
			webServer->send(this->http_ok, "text/html", "<a href='/rick'>Click me</a>"); 
		});
		webServer->on("/rick", [this]() {
			logger.log(Logger::INFO, "Received request in /rick endpoint.");
			
			String htmlResponse = "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"0;URL='https://www.youtube.com/watch?v=dQw4w9WgXcQ'\" /></head><body></body></html>";
			webServer->send(this->http_ok, "text/html", htmlResponse);
			
			logger.log(Logger::INFO, "Redirecting to Rick Roll & Deauthing client.");
			
			// Deauth for a quick disconnect
			for (int i = 0; i < 3; i++)
				cwifi.deauth(u8t_mac_ap); 
			
			this->stop(); // Disable the portal after redirecting, cant find any other way to disconnect the client
		});
		webServer->onNotFound([this]() {
			logger.log(Logger::INFO, "Received request in not found endpoint.");

			String test = "<!DOCTYPE html><html><head><title>TEST Captive Portal</title><meta charset=\"UTF-8\"></head><body><h1>TEST Captive Portal</h1><a href='/rick' target='_blank'>Click me</a></body></html>";

			webServer->send(
				this->http_ok, 
				"text/html; charset=utf-8", 
				test
			); 
		});

		webServer->begin();

		is_running = true;
		menu_is_running = is_running;
	}

	void stop()
	{
		is_running = false;
		dnsServer.stop();
		webServer->stop();
		// WiFi.disconnect();
		WiFi.mode(WIFI_OFF);

		menu_is_running = is_running;
		logger.log(Logger::INFO, "Stopped Captive Portal.");
	}

	void loop()
	{
		if (!is_running)
			return;

		dnsServer.processNextRequest();
		webServer->handleClient();
	}
public:
	bool menu_is_running = false;
private:
	IPAddress gateway;
	IPAddress mask;

	int8_t dns_port = 53;
	int8_t http_port = 80;

	int16_t http_ok = 200;
	int16_t http_redirect = 302;
	int16_t http_not_found = 404;

	DNSServer dnsServer;
	WebServer *webServer;

	uint8_t u8t_mac_ap[6];

	bool is_running = false;
};
