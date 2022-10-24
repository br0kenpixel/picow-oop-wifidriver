#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"    // WiFi driver
#include "pico/time.h"          // For UPTIME()

#include <string>               // C++ string type
#include <string.h>             // C string functions
#include <vector>               // C++ vector type

#include "net_objects.cpp"      // For now, only the IPAddress class

extern cyw43_t cyw43_state;     // Defined in the WiFi driver

using std::string;
using std::vector;

// Return uptime of the MCU in milliseconds
#define UPTIME() to_ms_since_boot(get_absolute_time())

// Type for storing WiFi network scan results
typedef vector<cyw43_ev_scan_result_t> WiFiScanResults;

/*
    Describes various possible statuses of the CYW43 chip.
*/
namespace WiFiStatus {
    const int NOT_INIT        = -255;                 /* Driver not initialized.                    */
    const int OFFLINE         = CYW43_LINK_DOWN;      /* Initial state after driver initialization. */
    const int JOINED          = CYW43_LINK_JOIN;      /* Connected to a network.                    */
    const int NO_INTERNET     = CYW43_LINK_NOIP;      /* Connected to a network, but no IP address. */
    const int CONNECTED       = CYW43_LINK_UP;        /* Connected to a network with an IP address. */
    const int CONNECT_FAILED  = CYW43_LINK_FAIL;      /* Failed to connect to a network.            */
    const int NOTFOUND        = CYW43_LINK_NONET;     /* Network not found (out of range)           */
    const int AUTH_FAILED     = CYW43_LINK_BADAUTH;   /* Failed to authenticate (bad password)      */
}

/*
    Describes supported authentication methods.
*/
namespace WiFiAuth {
    const uint32_t OPEN = CYW43_AUTH_OPEN;              /* Open network (not password protected) */
    const uint32_t WPA1 = CYW43_AUTH_WPA_TKIP_PSK;      /* Password-protected WPA1 with TKIP     */
    const uint32_t WPA2 = CYW43_AUTH_WPA2_AES_PSK;      /* Password-protected WPA2 with AES      */
    const uint32_t WPA  = CYW43_AUTH_WPA2_MIXED_PSK;    /* Password-protected WPA1/2 mix         */
}

/*
    WiFi object.
*/
class WiFi {
    public:
        WiFi();

        /**
         * Returns whether the WiFi driver is initialized and ready to be used.
         * An application shall use this to check if it's okay to use the WiFi
         * features.
         * 
         * If the function returns false, the application should NOT proceed
         * using the WiFi driver.
         *
         * @return true if initialized, false otherwise.
         */
        bool init();

        /**
         * Deinitializes the WiFi driver. Should be used in main() before a return
         * statement.
         * 
         * An application shall NOT use the WiFi driver after running this function.
         */
        void deinit();

        /**
         * Returns the current status code of the CYW43 WiFi chip.
         * If the driver is not initialized, WiFiStatus::NOT_INIT is returned.
         * 
         * @return true if initialized, false otherwise.
         */
        int status();

        /**
         * Returns the current status code of the CYW43 WiFi chip as a string.
         * Strings are the same as the status code constants defined in the
         * WiFiStatus namespace.
         * 
         * @return description of the current status.
         */
        string statusAsString();

        /**
         * Set the state of the on-board LED of a Raspberry Pi Pico W board.
         * Strings are the same as the status code constants defined in the
         * 
         * @param logic status.
         */
        void setLED(bool s);

        /**
         * Connect to a WiFi network. While connecting, the function will block.
         * If the connection fails within the specified timeout (in milliseconds),
         * the blocking is aborted.
         * 
         * If the WiFi driver is not initialized or the current WiFi status is not
         * WiFiStatus::OFFLINE or the async connect request fails, false is returned.
         * 
         * @param logic status.
         * @return whether the current status is WiFiStatus::CONNECTED.
         */
        bool connect(const char* ssid, const char* psswd, uint32_t auth, uint32_t timeout);

        /**
         * Disconnect from the network.
         * Blocking can be optionally enabled (recommended).
         * 
         * If the WiFi driver is not initialized or the current WiFi status is not
         * WiFiStatus::CONNECTED or the leave request fails, false is returned.
         * 
         * @param whether to block until the chip reports WiFiStatus::OFFLINE.
         * @return true if blocking is enbaled or boolean whether the leave request succeeded.
         */
        bool disconnect(bool block);

        /**
         * Returns the local IP address of the device.
         * 
         * If the WiFi driver is not initialized or the current WiFi status is not
         * WiFiStatus::CONNECTED, a zero-IP address is returned IPAddress(0, 0, 0, 0).
         * 
         * @return local IP address as IPAddress object.
         */
        IPAddress localIP();

        /**
         * Perform a scan for nearby WiFi networks.
         * The function will block until the scan is complete.
         * 
         * If the WiFi driver is not initialized or the current WiFi status is not
         * WiFiStatus::CONNECTED or the scan request fails, false is returned.
         * 
         * @return scan results as WiFiScanResults object (vector of cyw43_ev_scan_result_t).
         */
        WiFiScanResults scan();

        /**
         * Convert a MAC/BSSID address to a string.
         * The function will block until the scan is complete.
         * 
         * This function is intended for users to help them with such conversions.
         * 
         * @param uint8_t[6] array containing all 6 octets.
         * @return address as a C++ std::string.
         */
        static string macToString(const uint8_t bssid[6]);
    
    private:
        // Used to the whether the WiFi driver initialization was successfull
        int _init;

        // Storage for scan results
        WiFiScanResults scanResult;

        // Callback to handle found networks during a scan
        static int _scanCallback(void* self, const cyw43_ev_scan_result_t* network);

        /**
         * Clean up scan results by removing duplicates.
         * Uses scanCleanHelper() to check for duplicates.
         * 
         * This function should only be called internally (thus it's private).
         * 
         */
        void cleanupScan();

        /**
         * Check if a network exists in the scan results multiple times.
         * Used by cleanupScan().
         * Duplicates are detected by looking for a network with the same BSSID,
         * SSID's are not used since they can be the same for multiple AP's in certain
         * setups (like multi-point/roaming networks).
         * 
         * This function should only be called internally (thus it's private).
         * 
         * @param bssid of the network to look for.
         * @return true if duplicate(s) were found, false otherwise.
         */
        bool scanCleanHelper(uint8_t bssid[8]);
};