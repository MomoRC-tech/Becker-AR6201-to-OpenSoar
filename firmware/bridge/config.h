/*********************************************************************************
 * config.h: build-time settings of the Wi-Fi bridge firmware
 *
 * Pins, baud rates, ports, Wi-Fi mode and features. Passwords are in
 * credentials.h. Lines marked "Project:" are this project's settings.
 * Project: with STORED_SETTINGS, baud rates, frames, ports, the network and
 * the hotspot set here are defaults; valid values stored on the ESP32
 * override them (settings.h, MODE_STA_AP only).
*********************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

//#define DEBUG  // comment to suppress system serial messages
                 // Project: off, UART0 serves the Zander (wiring UART plan)

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#define OTA_HANDLER          // uncomment to enable OTA programming

// Project: Wi-Fi and OTA passwords live in credentials.h, ignored by Git.
// It has the same format as the OTA loader's; copy credentials.example.h.
#if __has_include("credentials.h")
#include "credentials.h"
#else
#error "Copy credentials.example.h to credentials.h and fill in your values."
#endif

// Project: tools/build_bridge.py fallback-test writes bridge_variant.h (ignored by Git).
// FALLBACK_TEST joins a network that does not exist, so the hotspot opens; without an
// OTA upload the bridge boots the previous app slot after FALLBACK_REVERT_MS.
#if __has_include("bridge_variant.h")
#include "bridge_variant.h"
#endif
#ifdef FALLBACK_TEST
#undef WIFI_SSID
#define WIFI_SSID "bridgetom-no-such-network"
#define FALLBACK_REVERT_MS (10UL * 60UL * 1000UL)
#endif

#define SSID     WIFI_SSID      // SSID to join (or broadcast)
#define PASSWD   WIFI_PASSWORD  // wiFi password
#define HOSTNAME "wifi-bridgetom"  // hostname for mDNS and OTA (Project: as the OTA loader)

#define HOTSPOT_SSID "WiFi-BridgeTom"  // Project: MODE_STA_AP fallback hotspot (ADR-05)
#define JOIN_TIMEOUT_MS 15000         // Project: MODE_STA_AP network join time

#define BUFFERSIZE 1024
#define UART_RX_BUFFER 4096  // Project: UART receive buffer per port (ESP32), holds
                             // incoming bytes while the loop waits on a UART write

#ifdef ESP32
#define VERSION "0.5-ESP32"       // Project: own version, 0.<plan step> until bench acceptance; 0.4.1 = device profiles (WP9), .2 = web log (step 3c), .3/.4 = log clock time; 0.5 = apps never hold up the bridge
#elif defined(ESP8266)
#define VERSION "0.3b.1-ESP8266"
#endif

#define MODE_STA_AP            // MODE_STA, MODE_AP or MODE_STA_AP (Project: network, then hotspot; ESP32 only)
#define PROTOCOL_TCP           // uncomment to enable TCP server
#define MAX_NMEA_CLIENTS 4     // max TCP clients
#define APP_STALL_CLOSE_MS 60000UL  // Project: close a Zander/FLARM app that took no data for this long; until then it skips data (step 5)
// Project: UDP (upstream PROTOCOL_UDP) is removed; the bridge serves TCP only
#define STATUS_PAGE            // Project: read-only status page on port 80 and /api/status (ESP32 only)
#define STORED_SETTINGS        // Project: settings stored on the ESP32 override the defaults here (step 3a, ESP32 only)
#define WEB_LOG                // Project: event log on /log, kept across a restart (step 3c, weblog.h, needs STATUS_PAGE, ESP32 only)
#define PORT_NAMES {"Zander", "FLARM", "Becker"}  // Project: COM 0, 1, 2 on the status page
#define PORT_PROFILES {"zs1", "flarm", "ar6201"}  // Project: default device profile per COM (settings.h); baud and frame must match UART_BAUDn and SERIAL_PARAMn
//#define BLUETOOTH 0          // uncomment to create a bluetooth serial bridge on the indicated serial port (ESP32 only)
//#define BATTERY_SAVER        // uncomment to reduce wifi power

#define STATIC_IP IPAddress( 10,  10,  10, 10)  // static IP for MODE_AP (Project: and the MODE_STA_AP hotspot)
#define NETMASK   IPAddress(255, 255, 255, 0)  // netmask for MODE_AP

#ifdef ESP32
#define NUM_COM   3    // 3 available on ESP32
#elif defined(ESP8266)
#define NUM_COM   2    // we only use 2 on ESP8266
#endif

/**************************  COM Port 0 *******************************/
#define UART_BAUD0 19200          // Baudrate UART0: Zander ZS1, candidate (S22), installed setting unknown
#define SERIAL_PARAM0 SERIAL_8N1  // Data/Parity/Stop UART0 (use SWSERIAL_* for ESP8266)
#define SERIAL0_TXPIN 33          // transmit Pin UART0
#define SERIAL0_RXPIN 32          // receive Pin UART0
#define SERIAL0_TCP_PORT 10110    // TCP Port UART0
/*************************  COM Port 1 *******************************/
#define UART_BAUD1 19200            // Baudrate UART1: FLARM, candidate (S23), installed setting unknown
#define SERIAL_PARAM1 SERIAL_8N1    // Data/Parity/Stop UART1 (use SWSERIAL_* for ESP8266)
#define SERIAL1_TXPIN 27            // transmit Pin UART1
#define SERIAL1_RXPIN 26            // receive Pin UART1
#define SERIAL1_TCP_PORT 2000       // TCP Port UART1
/*************************  COM Port 2 *******************************/
#define UART_BAUD2 9600             // Baudrate UART2         (ESP32 only): Becker, best lead (S108), installed setting unknown
#define SERIAL_PARAM2 SERIAL_8N1    // Data/Parity/Stop UART2 (ESP32 only)
#define SERIAL2_TXPIN 17            // transmit Pin UART2     (ESP32 only)
#define SERIAL2_RXPIN 16            // receive Pin UART2      (ESP32 only)
#define SERIAL2_TCP_PORT 4352       // TCP Port UART2         (ESP32 only)

/**************************  DO NOT EDIT BELOW HERE **************************/
// Project: build-time checks for the Wi-Fi mode and the radio port rules
#if defined(MODE_STA) + defined(MODE_AP) + defined(MODE_STA_AP) != 1
#error "Select exactly one of MODE_STA, MODE_AP and MODE_STA_AP."
#endif
#if defined(MODE_STA_AP) && !defined(ESP32)
#error "MODE_STA_AP is ESP32 only."
#endif
#define RADIO_COM 2  // Becker radio on UART2
#if defined(BLUETOOTH) && (BLUETOOTH == RADIO_COM)
#error "Bluetooth must not target the radio port."
#endif
#ifdef PROTOCOL_UDP
#error "UDP is removed from this firmware; the bridge serves TCP only."
#endif
#if defined(WEB_LOG) && !(defined(ESP32) && defined(STATUS_PAGE))
#error "WEB_LOG needs STATUS_PAGE and an ESP32."
#endif
#if defined(WEB_LOG) && defined(DEBUG)
#error "DEBUG prints on UART0, which serves the Zander; WEB_LOG shows the same lines on /log."
#endif

// perhaps a clever way to include/exclude serial debug messages...
#ifdef DEBUG
    #define debug Serial
#elif defined(WEB_LOG)
    // Project: `debug` writes Debug lines to the web log (weblog.h)
#else
    class Debug: public Print {
        virtual size_t write(byte b) { return 0; }
    };
    Debug debug;
#endif

#endif  // CONFIG_H
