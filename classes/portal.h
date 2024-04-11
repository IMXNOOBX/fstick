#include "globals.h"

/**
 * @brief Docs and other useful information
 * @details WebServer https://github.com/khoih-prog/WiFiWebServer
 * @details DNSServer 
 * 
 */
extern Logger logger;
extern Notify notify;

class CPortal
{
public:
	CPortal(): gateway(172, 0, 0, 1), mask(255, 255, 255, 0) { }

	void run() {
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(gateway, gateway, mask);
		WiFi.softAP("TEST");

		webServer = new WebServer(http_port);
		dnsServer.start(dns_port, "*", gateway);

		// webServer->on("/", [this]() {
		// 	webServer->send(this->http_ok, "text/html", "<a href='/rick'>Click me</a>");
		// });
		webServer->on("/rick", [this]() {
			webServer->sendHeader("Location", "https://www.youtube.com/watch?v=dQw4w9WgXcQ", true);
			webServer->send(302, "text/plain", "");

			this->stop();
		});
		webServer->onNotFound([this]() {
			String test = "<!DOCTYPE html><html><head><title>Largest companies by market cap — US Stock Market</title><meta charset=\"UTF-8\"></head><body><h1>Apple : 2037 Billion</h1><h2>Microsoft : 1624 Billion</h2><h3>Amazon : 1611 Billion</h3><h4>Google : 1058 Billion</h4><h5>Alibaba : 826 Billion</h5><b>This data is as of 21 Sep 2020.</b></body></html>";

			webServer->send(
				this->http_ok, 
				"text/html", 
				test
			);
		});

		webServer->begin();

		is_running = true;
	}

	void stop() {
		is_running = false;
		dnsServer.stop();
		webServer->stop();
		// WiFi.disconnect();
		WiFi.mode(WIFI_OFF);
	}

	void loop() {
		if (!is_running)
			return;

		dnsServer.processNextRequest();
		webServer->handleClient();
	}

private:
	IPAddress gateway;
	IPAddress mask;

	int8_t dns_port = 53;
	int8_t http_port = 80;

	int8_t http_ok = 200;
	int8_t http_redirect = 302;
	int8_t http_not_found = 404;

	DNSServer dnsServer;
	WebServer* webServer;

	bool is_running = false;
};
