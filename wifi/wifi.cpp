#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include <string>
#include "net_objects.cpp"

#define UPTIME() to_ms_since_boot(get_absolute_time())

extern cyw43_t cyw43_state;
using std::string;

namespace WiFiStatus {
	const int NOT_INIT 			= -255;
	const int OFFLINE 	 		= CYW43_LINK_DOWN;
	const int JOINED			= CYW43_LINK_JOIN;
	const int NO_INTERNET 		= CYW43_LINK_NOIP;
	const int CONNECTED			= CYW43_LINK_UP;
	const int CONNECT_FAILED	= CYW43_LINK_FAIL;
	const int NOTFOUND			= CYW43_LINK_NONET;
	const int AUTH_FAILED		= CYW43_LINK_BADAUTH;
}

namespace WiFiAuth {
	const uint32_t OPEN = CYW43_AUTH_OPEN;
	const uint32_t WPA1 = CYW43_AUTH_WPA_TKIP_PSK;
	const uint32_t WPA2 = CYW43_AUTH_WPA2_AES_PSK;
	const uint32_t WPA  = CYW43_AUTH_WPA2_MIXED_PSK;
}

class WiFi {
	public:
		WiFi();
		bool init();
		void deinit();
		int status();
		string statusAsString();
		void setLED(bool s);
		bool connect(const char* ssid, const char* psswd, uint32_t auth, uint32_t timeout);
		bool disconnect(bool block);
		IPAddress localIP();
	private:
		int _init;
};

WiFi::WiFi(){
	_init = cyw43_arch_init();
	if(_init == 0) cyw43_arch_enable_sta_mode();
}

bool WiFi::init(){
	return _init == 0;
}

void WiFi::deinit(){
	cyw43_arch_deinit();
	_init = 0;
}

int WiFi::status(){
	if(!init()) return WiFiStatus::NOT_INIT;
	return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
}

string WiFi::statusAsString(){
	int state = status();
	switch(state){
		case WiFiStatus::NOT_INIT:
			return "NOT_INIT";
		case WiFiStatus::OFFLINE:
			return "OFFLINE";
		case WiFiStatus::JOINED:
			return "JOINED";
		case WiFiStatus::NO_INTERNET:
			return "NO_INTERNET";
		case WiFiStatus::CONNECTED:
			return "CONNECTED";
		case WiFiStatus::CONNECT_FAILED:
			return "CONNECT_FAILED";
		case WiFiStatus::NOTFOUND:
			return "NOTFOUND";
		case WiFiStatus::AUTH_FAILED:
			return "AUTH_FAILED";
		default:
			return "UNKNOWN";
	}
}

void WiFi::setLED(bool s){
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (int)s);
}

bool WiFi::connect(const char* ssid, const char* psswd, uint32_t auth, uint32_t timeout){
	if(!init() || status() != WiFiStatus::OFFLINE) return false;
	if(cyw43_arch_wifi_connect_async(ssid, psswd, auth) != 0) return false;
	uint32_t start = UPTIME();

	while((status() != WiFiStatus::CONNECTED) && (UPTIME() - start <= timeout)){
		sleep_ms(200);
	}

	return status() == WiFiStatus::CONNECTED;
}

bool WiFi::disconnect(bool block){
	if(!init() || status() != WiFiStatus::CONNECTED) return false;
	int error = cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
	if(!block){
		return error == 0;
	} else {
		while(status() == WiFiStatus::CONNECTED){
			sleep_ms(200);
		}
		return true;
	}
}

IPAddress WiFi::localIP(){
	if(!init() || status() == WiFiStatus::OFFLINE) return IPAddress(0, 0, 0, 0);
	const ip4_addr_t* ip_stat = netif_ip4_addr(netif_list);
	return IPAddress(ip_stat);
}