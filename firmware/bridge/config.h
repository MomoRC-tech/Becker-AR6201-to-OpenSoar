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

#define DEBUG  // comment to suppress system serial messages

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#define OTA_HANDLER          // uncomment to enable OTA programming

#define SSID     "ssid"      // SSID to join (or broadcast)
#define PASSWD   "password"  // wiFi password
#define HOSTNAME "esp32"     // hostname for STA mode mDNS

#define BUFFERSIZE 1024

#ifdef ESP32
#define VERSION "2.0-ESP32"
#elif defined(ESP8266)
#define VERSION "2.0-ESP8266"
#endif

#define MODE_STA               // MODE_STA or MODE_AP
#define PROTOCOL_TCP           // uncomment to enable TCP server
#define MAX_NMEA_CLIENTS 4     // max TCP clients
#define PROTOCOL_UDP           // uncomment to enable UDP broadcast (ESP32 only)
//#define BLUETOOTH 0          // uncomment to create a bluetooth serial bridge on the indicated serial port (ESP32 only)
//#define BATTERY_SAVER        // uncomment to reduce wifi power

#define STATIC_IP IPAddress( 10,  10,  10, 10)  // static IP for MODE_AP
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
#define SERIAL2_UDP_PORT 14552      // UDP Port UART2         (ESP32 only)

/**************************  DO NOT EDIT BELOW HERE **************************/
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
