# picow-oop-wifidriver
An object-oriented WiFi driver/wrapper for the Raspberry Pi Pico W written in C++.

# Basic example:
```c++
#include <stdio.h>
#include "pico/stdlib.h"
#include "wifi/wifi.cpp"

int main(){
    stdio_init_all();
    
    WiFi wireless; //Initialization is done automatically
    wireless.init(); //Returns true if the initialization succeeded (should be used in an if)
    
    wireless.connect(SSID, PASSWORD, AUTH, WIFI_TIMEOUT);
    printf("WiFi: Local IP - %s\n", wireless.localIP().toString().c_str());
    
    wireless.disconnect(true); //Blocking disconnect

    wireless.deinit();
    return 0;
}
```
