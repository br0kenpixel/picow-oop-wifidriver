#include <stdio.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "wifi/wifi.cpp"

const char SSID[] = "YOUR_SSID";
const char PSSW[] = "YOUR_PASSWORD";
const uint64_t AUTH = CYW43_AUTH_WPA2_AES_PSK;

#define WIFI_TIMEOUT 15000

int main() {
    stdio_init_all();
    sleep_ms(5000);

    WiFi wireless;
    if(!wireless.init()){
        printf("WiFi failed to initialize!\n");
        return 1;
    } else printf("WiFi initialized successfully!\n");
    
    printf("WiFi: Link status - %s\n", wireless.statusAsString().c_str());
    printf("WiFi: Local IP - %s\n", wireless.localIP().toString().c_str());

    printf("Attempting to connect...\n");
    if(!wireless.connect(SSID, PSSW, AUTH, WIFI_TIMEOUT)){
        printf("Connection attempt failed! - %s\n", wireless.statusAsString().c_str());
        goto end;
    } else printf("WiFi Connected!\n");

    printf("WiFi: Link status - %s\n", wireless.statusAsString().c_str());
    printf("WiFi: Local IP - %s\n", wireless.localIP().toString().c_str());

    printf("Sleeping...\n");
    sleep_ms(2000);
    if(wireless.status() != WiFiStatus::CONNECTED){
        printf("Connection lost (%s)!\n", wireless.statusAsString().c_str());
        goto end;
    } else printf("WiFi still connected!\n");

    printf("WiFi: Link status - %s\n", wireless.statusAsString().c_str());
    printf("WiFi: Local IP - %s\n", wireless.localIP().toString().c_str());

    printf("Disconnecting...\n");
    if(!wireless.disconnect(true)){
        printf("Failed to disconnect!\n");
        goto end;
    } else printf("WiFi Disconnected!\n");

    printf("WiFi: Link status - %s\n", wireless.statusAsString().c_str());
    printf("WiFi: Local IP - %s\n", wireless.localIP().toString().c_str());

    printf("Done. Bye!\n");

    end:
        wireless.deinit();
        return 0;
}
