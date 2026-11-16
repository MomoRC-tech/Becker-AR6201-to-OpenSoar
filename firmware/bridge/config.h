/*********************************************************************************
 * config.h
 * 
 * Added compatibility for ESP8266  -- Yuri - Aug 2021
 * Incremented  version to 2.0      -- Yuri - Apr 2023
 * Implemented UDP + bug fixes      -- Yuri - Apr 2023
 * 
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
#define VERSION "2.0-ESP32"
#elif defined(ESP8266)
#define VERSION "2.0-ESP8266"
#endif

#define MODE_STA_AP            // MODE_STA, MODE_AP or MODE_STA_AP (Project: network, then hotspot; ESP32 only)
#define PROTOCOL_TCP           // uncomment to enable TCP server
#define MAX_NMEA_CLIENTS 4     // max TCP clients
#define PROTOCOL_UDP           // uncomment to enable UDP broadcast (ESP32 only)
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
#define SERIAL0_UDP_PORT 14550    // UDP Port UART0           (ESP32 only)
/*************************  COM Port 1 *******************************/
#define UART_BAUD1 19200            // Baudrate UART1: FLARM, candidate (S23), installed setting unknown
#define SERIAL_PARAM1 SERIAL_8N1    // Data/Parity/Stop UART1 (use SWSERIAL_* for ESP8266)
#define SERIAL1_TXPIN 27            // transmit Pin UART1
#define SERIAL1_RXPIN 26            // receive Pin UART1
#define SERIAL1_TCP_PORT 2000       // TCP Port UART1
#define SERIAL1_UDP_PORT 14551      // UDP Port UART1         (ESP32 only)
/*************************  COM Port 2 *******************************/
#define UART_BAUD2 115200           // Baudrate UART2         (ESP32 only): Becker, placeholder, setting unknown
#define SERIAL_PARAM2 SERIAL_8N1    // Data/Parity/Stop UART2 (ESP32 only)
#define SERIAL2_TXPIN 17            // transmit Pin UART2     (ESP32 only)
#define SERIAL2_RXPIN 16            // receive Pin UART2      (ESP32 only)
#define SERIAL2_TCP_PORT 4352       // TCP Port UART2         (ESP32 only)
#define SERIAL2_UDP_PORT 0          // UDP Port UART2         (ESP32 only) Project: 0 = no UDP; radio port is TCP only

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
#if defined(ESP32) && defined(PROTOCOL_UDP) && (SERIAL2_UDP_PORT != 0)
#error "No UDP on the radio port: set SERIAL2_UDP_PORT to 0."
#endif

// perhaps a clever way to include/exclude serial debug messages...
#ifdef DEBUG
    #define debug Serial
#else
    class Debug: public Print {
        virtual size_t write(byte b) { return 0; }
    };
    Debug debug;
#endif

#endif  // CONFIG_H
