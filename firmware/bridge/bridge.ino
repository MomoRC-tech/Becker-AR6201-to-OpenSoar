/*********************************************************************************
 * Wi-Fi bridge firmware: Zander ZS1, FLARM and Becker AR6201 to OpenSoar
 * https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar (firmware/bridge)
 *
 * Forwards three UARTs to TCP and UDP over Wi-Fi: Zander on UART0, FLARM on
 * UART1, the Becker radio (RS-422 through a MAX490) on UART2. Settings are in
 * config.h, Wi-Fi and OTA passwords in credentials.h, the status page in
 * status_page.h.
 *
 * Based on ESP-Serial-Bridge by yuri-rage, itself forked from AlphaLima's
 * ESP32-Serial-Bridge (MIT license, see LICENSE). Lines marked "Project:"
 * are this project's changes.
 *
 * Note: the ESP8266 branches are kept but not tested here.
 *
 * Disclaimer: Don't use for life support systems or any other situation
 * where system failure may affect user or environmental safety.
 *********************************************************************************/

#include <Arduino.h>

#ifdef ESP32
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>  // use SoftwareSerial for ESP8266
#endif

#include "config.h"

#ifdef FALLBACK_TEST  // Project: temporary hotspot fallback test build
#include <esp_ota_ops.h>
static bool otaStarted = false;
#endif

#ifdef OTA_HANDLER
#include <ArduinoOTA.h>
static_assert(sizeof(OTA_PASSWORD) - 1 >= 8, "OTA_PASSWORD needs at least 8 characters.");  // Project
#endif
#ifdef MODE_STA_AP
static_assert(sizeof(HOTSPOT_PASSWORD) - 1 >= 8, "HOTSPOT_PASSWORD needs at least 8 characters (WPA2).");  // Project
#endif

#ifdef ESP32

#ifdef PROTOCOL_UDP
#include <AsyncUDP.h>

AsyncUDP udp0, udp1, udp2;
AsyncUDP *udp[NUM_COM] = {&udp0, &udp1, &udp2};
uint16_t udp_port[NUM_COM] = {SERIAL0_UDP_PORT, SERIAL1_UDP_PORT,
                              SERIAL2_UDP_PORT};
#endif

#ifdef BLUETOOTH  // inside the ESP32 condition, since ESP8266 doesn't have BT
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
#endif

HardwareSerial Serial_one(1);
HardwareSerial Serial_two(2);
HardwareSerial* COM[NUM_COM] = {&Serial, &Serial_one, &Serial_two};
#elif defined(ESP8266)
SoftwareSerial Serial_zero(SERIAL0_RXPIN, SERIAL0_TXPIN);
SoftwareSerial Serial_one(SERIAL1_RXPIN, SERIAL1_TXPIN);
SoftwareSerial* COM[NUM_COM] = {&Serial_zero, &Serial_one};
#endif

#ifdef PROTOCOL_TCP
#include <WiFiClient.h>
WiFiServer server_0(SERIAL0_TCP_PORT);
WiFiServer server_1(SERIAL1_TCP_PORT);

#ifdef ESP32
WiFiServer server_2(SERIAL2_TCP_PORT);
WiFiServer* server[NUM_COM] = {&server_0, &server_1, &server_2};
#elif defined(ESP8266)
WiFiServer* server[NUM_COM] = {&server_0, &server_1};
#endif
WiFiClient TCPClient[NUM_COM][MAX_NMEA_CLIENTS];
#endif

uint8_t buf1[NUM_COM][BUFFERSIZE];
uint8_t buf2[NUM_COM][BUFFERSIZE];

#ifdef ESP32
uint16_t i1[NUM_COM] = {0, 0, 0};
#elif defined(ESP8266)
uint16_t i1[NUM_COM] = {0, 0};
#endif

uint8_t BTbuf[BUFFERSIZE];
uint16_t iBT = 0;

#include "settings.h"     // Project: stored settings (step 3a)
#include "status_page.h"  // Project: status page and counters (step 3)

#ifdef MODE_STA
#if defined(ESP32)
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
#endif
#if defined(ESP8266)
    WiFiEventHandler stationDisconnectedHandler;
    void WiFiStationDisconnected(
        const WiFiEventSoftAPModeStationDisconnected& evt) {
#endif
        debug.print("WiFi disconnected: ");
#ifdef ESP32
        debug.println(info.wifi_sta_disconnected.reason);
#endif
        debug.println("Trying to reconnect..");
        WiFi.begin(SSID, PASSWD);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            debug.print(".");
        }
        debug.println("connected");
        debug.print("IP address: ");
        debug.println(WiFi.localIP());
    }
#endif

    void setup() {
        delay(500);

#ifdef ESP32
        settingsLoad();  // Project: defaults from config.h, overridden by valid stored values (step 3a)
        for (int num = 0; num < NUM_COM; num++)  // Project: before begin()
            COM[num]->setRxBufferSize(UART_RX_BUFFER), statusAttachUart(num);
        COM[0]->begin(settings.port[0].baud, settingsFrameCode(0), SERIAL0_RXPIN, SERIAL0_TXPIN);  // Project: stored settings
        COM[1]->begin(settings.port[1].baud, settingsFrameCode(1), SERIAL1_RXPIN, SERIAL1_TXPIN);
        COM[2]->begin(settings.port[2].baud, settingsFrameCode(2), SERIAL2_RXPIN, SERIAL2_TXPIN);
#elif defined(ESP8266)
    COM[0]->begin(UART_BAUD0);
    COM[1]->begin(UART_BAUD1);
    debug.begin(UART_BAUD0);
#endif

        debug.print("\n\nWiFi serial bridge ");
        debug.println(VERSION);

#ifdef MODE_AP
        debug.println("Open ESP Access Point Mode");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(SSID, PASSWD);  // configure SSID and password for softAP
        delay(2000);            // VERY IMPORTANT
        WiFi.softAPConfig(STATIC_IP, STATIC_IP,
                          NETMASK);  // configure ip address for softAP
#endif

#ifdef MODE_STA
        debug.println("Open ESP Station Mode");
        WiFi.mode(WIFI_STA);
#ifdef ESP32
        WiFi.onEvent(WiFiStationDisconnected,
                     ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
#elif defined(ESP8266)
    stationDisconnectedHandler =
        WiFi.onSoftAPModeStationDisconnected(&WiFiStationDisconnected);
#endif
        WiFi.begin(SSID, PASSWD);
        debug.print("Connecting to: ");
        debug.print(SSID);
        debug.print("..");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            debug.print(".");
        }
        debug.println("connected");
        debug.print("IP address: ");
        debug.println(WiFi.localIP());

        if (!MDNS.begin(HOSTNAME)) {
            debug.println("Error starting mDNS");
        } else {
            debug.print("Started mDNS, discoverable as: ");
            debug.println(HOSTNAME);
            MDNS.addService("_telnet", "_tcp", SERIAL0_TCP_PORT);
        }
#endif

#ifdef MODE_STA_AP  // Project: join the network for JOIN_TIMEOUT_MS, else open the hotspot (ADR-05, R12)
        debug.println("Open ESP Station Mode, hotspot fallback");
        WiFi.persistent(false);  // Project: the core keeps no second copy of the network password (step 3a)
        WiFi.setHostname(HOSTNAME);
        WiFi.mode(WIFI_STA);
        WiFi.begin(settings.netSsid, settings.netPass);  // Project: stored settings (step 3a)
        debug.printf("Joining %s for up to %u s\n", settings.netSsid, settings.joinSeconds);
        {
            const unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < settings.joinSeconds * 1000UL) {
                delay(250);
            }
        }
        if (WiFi.status() == WL_CONNECTED) {
            debug.print("connected, IP address: ");
            debug.println(WiFi.localIP());  // reconnects on its own after a drop
        } else {
            debug.println("Network not reached, opening hotspot");
            WiFi.disconnect(true);
            WiFi.mode(WIFI_AP);
            if (!WiFi.softAP(settings.hotspotSsid, settings.hotspotPass)) {
                debug.println("Hotspot failed, restarting in 5 s");
                delay(5000);
                ESP.restart();
            }
            delay(2000);  // as upstream MODE_AP before softAPConfig
            const IPAddress hotspotIp(settings.hotspotIp);
            WiFi.softAPConfig(hotspotIp, hotspotIp, NETMASK);
            debug.print("Hotspot ");
            debug.print(settings.hotspotSsid);
            debug.print(", IP address: ");
            debug.println(WiFi.softAPIP());
        }
        if (MDNS.begin(HOSTNAME)) {
            MDNS.addService("_telnet", "_tcp", settings.port[0].tcpPort);
        }
#endif

#ifdef OTA_HANDLER
        // Project: password and hostname (R13); messages follow the DEBUG switch
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setPassword(OTA_PASSWORD);
        ArduinoOTA.onStart([]() {
#ifdef FALLBACK_TEST
            otaStarted = true;
#endif
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else  // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount
            // SPIFFS using SPIFFS.end()
            debug.println("Start updating " + type);
        });
        ArduinoOTA.onEnd([]() { debug.println("\nEnd"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            debug.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            debug.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                debug.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                debug.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                debug.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                debug.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                debug.println("End Failed");
        });
        // if DNSServer is started with "*" for domain name, it will reply with
        // provided IP to all DNS request

        ArduinoOTA.begin();
#endif

#ifdef PROTOCOL_TCP
        for (int num = 0; num < NUM_COM; num++) {
            debug.print("Starting TCP Server ");
            debug.println(num + 1);
#ifdef ESP32
            server[num]->begin(settings.port[num].tcpPort);  // Project: stored settings (step 3a)
#else
            server[num]->begin();  // start TCP server
#endif
            server[num]->setNoDelay(true);
        }
#endif

#ifdef ESP32
#ifdef PROTOCOL_UDP
        for (uint16_t num = 0; num < NUM_COM; num++) {
            udp_port[num] = settings.port[num].udpPort;  // Project: stored settings (step 3a)
            if (udp_port[num] == 0) continue;  // Project: no UDP on this port
            if (udp[num]->listen(udp_port[num])) {
                debug.printf("Listening on UDP port %d\n", udp_port[num]);
                udp[num]->onPacket([num](AsyncUDPPacket packet) {
                    COM[num]->write(packet.data(), packet.length());
                    STAT(num, toDevice, packet.length());  // Project: count
                    MON(num, 1, packet.data(), packet.length()); });  // Project: data monitor (step 3b)
                    }
                }
#endif
#ifdef BLUETOOTH
        debug.printf("Bluetooth discoverable at %s\n", SSID);
        SerialBT.begin(SSID);  // Bluetooth device name
#endif
#endif

#ifdef BATTERY_SAVER
#ifdef ESP32
        esp_wifi_set_max_tx_power(50);  // lower WiFi Power (Project: now called; upstream declared a variable)
#elif defined(ESP8266)
    WiFi.setOutputPower(15);
#endif
#endif
        statusBegin();  // Project: status page task (step 3)
    }

#ifdef FALLBACK_TEST
    // Project: boot the other app slot (the previous image) if no OTA upload
    // arrived, so the test build never stays installed by accident.
    static void revertIfIdle() {
        if (otaStarted || millis() < FALLBACK_REVERT_MS) return;
        const esp_partition_t* previous = esp_ota_get_next_update_partition(nullptr);
        if (previous != nullptr) esp_ota_set_boot_partition(previous);
        delay(500);
        ESP.restart();
    }
#endif

    void loop() {
#ifdef FALLBACK_TEST
        revertIfIdle();
#endif
#ifdef OTA_HANDLER
        ArduinoOTA.handle();
#endif

#ifdef ESP32
#ifdef BLUETOOTH
        if (SerialBT.hasClient()) {
            while (SerialBT.available()) {
                BTbuf[iBT] = SerialBT.read();
                iBT++;
                if (iBT == BUFFERSIZE - 1) break;
            }
            COM[BLUETOOTH]->write(BTbuf, iBT);
            iBT = 0;
        }
#endif
#endif

#ifdef PROTOCOL_TCP
        for (int num = 0; num < NUM_COM; num++) {
            if (server[num]->hasClient()) {
                for (byte i = 0; i < MAX_NMEA_CLIENTS; i++) {
                    // find free/disconnected spot
                    if (!TCPClient[num][i] || !TCPClient[num][i].connected()) {
                        if (TCPClient[num][i]) TCPClient[num][i].stop();
                        TCPClient[num][i] = server[num]->available();
                        debug.print("New client for COM");
                        debug.print(num);
                        debug.print(" #");
                        debug.println(i);
                        continue;
                    }
                }
                // no free/disconnected spot so reject
                WiFiClient TmpserverClient = server[num]->available();
                TmpserverClient.stop();
            }
        }
#endif

        for (int num = 0; num < NUM_COM; num++) {
            if (COM[num] != NULL) {
#ifdef PROTOCOL_TCP
                uint32_t clients = 0;  // Project: count for the status page
                for (byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++) {
                    if (TCPClient[num][cln]) {
                        clients++;  // Project
                        while (TCPClient[num][cln].available()) {
                            buf1[num][i1[num]] =
                                TCPClient[num][cln]
                                    .read();  // read char from client
                            i1[num]++;
                            if (i1[num] == BUFFERSIZE - 1) break;
                        }

                        COM[num]->write(buf1[num],
                                        i1[num]);  // now send to UART(num):
                        STAT(num, toDevice, i1[num]);  // Project: count
                        MON(num, 1, buf1[num], i1[num]);  // Project: data monitor (step 3b)
                        i1[num] = 0;
                    }
                }
                STAT_CLIENTS(num, clients);  // Project
#endif

                if (COM[num]->available()) {
                    int readCount = COM[num]->read(&buf2[num][0], BUFFERSIZE); // Read up to BUFFERSIZE bytes from the serial device.
                    if (readCount == 0)
                        continue;
                    STAT(num, fromDevice, readCount);  // Project: count
                    STAT_RX(num);
                    MON(num, 0, buf2[num], readCount);  // Project: data monitor (step 3b)
#ifdef PROTOCOL_TCP
                    for (byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++) {
                        if (TCPClient[num][cln])
                            if (TCPClient[num][cln].write(buf2[num], readCount) != (size_t)readCount)
                                STAT(num, shortWrites, 1);  // Project: count short writes
                    }
#endif

#ifdef ESP32
#ifdef PROTOCOL_UDP
                    if (udp_port[num] != 0)  // Project: no UDP on this port
                        if (udp[num]->broadcastTo(buf2[num], readCount, udp_port[num]) != (size_t)readCount)
                            STAT(num, udpDrops, 1);  // Project: count UDP sends the network stack refused
#endif
#ifdef BLUETOOTH
                    // now send to Bluetooth:
                    if (num == BLUETOOTH && SerialBT.hasClient())
                        SerialBT.write(buf2[num], readCount);
#endif
#endif
                }
            }
        }
    }
