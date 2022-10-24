#include "wifi.hpp"

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

int WiFi::_scanCallback(void* self, const cyw43_ev_scan_result_t* result){
    WiFi* _self = (WiFi*)self;
    printf("[WiFi Scan] Got beacon from: %s\n", result->ssid);
    
    if(result){
        cyw43_ev_scan_result_t network;
        memcpy(&network, result, sizeof(network));
        _self->scanResult.push_back(network);
    }
    return 1;
}

bool WiFi::scanCleanHelper(uint8_t bssid[8]){
    int count = 0;
    for(cyw43_ev_scan_result_t network : scanResult){
        if(memcmp(network.bssid, bssid, 8) == 0){
            count++;
        }
    }
    return count > 1;
}

void WiFi::cleanupScan(){
    WiFiScanResults::iterator it = scanResult.begin();
    while(it != scanResult.end()){
        bool duplicate = scanCleanHelper(it->bssid);
        if(duplicate){
            it = scanResult.erase(it);
        } else it++;
    }
}

WiFiScanResults WiFi::scan(){
    if(!init() || status() == WiFiStatus::CONNECTED) return {};

    cyw43_wifi_scan_options_t scan_options;
    memset(&scan_options, 0, sizeof(scan_options));
    scanResult.clear();
    
    int error = cyw43_wifi_scan(&cyw43_state, &scan_options, this, _scanCallback);
    if(error == 0){
        while(cyw43_wifi_scan_active(&cyw43_state)) sleep_ms(10);
    } else return {};

    //Remove duplicates
    cleanupScan();

    return scanResult;
}

string WiFi::macToString(const uint8_t bssid[6]){
    char buf[18 /* AB:CD:EF:GH:IJ:KL0 */] = {0};
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
        bssid[0], bssid[1],
        bssid[2], bssid[3],
        bssid[4], bssid[5]
    );
    return string(buf);
}