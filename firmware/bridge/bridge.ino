/*********************************************************************************
 * Wi-Fi bridge firmware: Zander ZS1, FLARM and Becker AR6201 to OpenSoar
 * https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar (firmware/bridge)
 *
 * Forwards three UARTs to TCP over Wi-Fi (no UDP): Zander on UART0, FLARM on
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
#include <driver/uart.h>
#include <esp_wifi.h>
#include <esp_wifi_ap_get_sta_list.h>
#include <lwip/sockets.h>
#include <cerrno>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>  // use SoftwareSerial for ESP8266
#endif

#include "config.h"
#include "weblog.h"  // Project: web log (step 3c); before every module that logs

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

// Project: the upstream UDP broadcast (PROTOCOL_UDP) is removed; TCP only.
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

#if defined(ESP32) && defined(PROTOCOL_TCP)
// The Wi-Fi event task only marks a departed owner. The bridge loop owns sockets.
static portMUX_TYPE radioStationLock = portMUX_INITIALIZER_UNLOCKED;
static uint8_t radioStationMac[6];
static bool radioStationTracked = false;
static bool radioStationLeft = false;

static void radioStationDisconnected(WiFiEvent_t, WiFiEventInfo_t info) {
    portENTER_CRITICAL(&radioStationLock);
    if (radioStationTracked && memcmp(radioStationMac, info.wifi_ap_stadisconnected.mac, sizeof(radioStationMac)) == 0)
        radioStationLeft = true;
    portEXIT_CRITICAL(&radioStationLock);
}

// Project (step 5): apps are sent to without waiting. The core's write() waits up to 10 s
// for an app that does not read and holds up every port meanwhile (S119). Each app has a
// small queue for data its socket cannot take yet (send buffer or segment queue full while
// the phone's acknowledgements are on their way); the loop sends it on the next passes.
#define APP_QUEUE 2048  // bytes per app, about 1 s at 19200 baud
struct AppQueue {
    uint16_t len;
    uint32_t progressAt;  // last time the app took data, or its queue was empty
    uint8_t data[APP_QUEUE];
};
// About 24 kB, taken from the heap at first use: static RAM has no room left for it.
// Returns void*, since the sketch's generated prototypes come before the struct.
static void* appQueueMemory() {
    static void* const all = calloc(NUM_COM * MAX_NMEA_CLIENTS, sizeof(AppQueue));
    configASSERT(all != nullptr);
    return all;
}
#define queueOf(num, cln) (static_cast<AppQueue*>(appQueueMemory())[(num) * MAX_NMEA_CLIENTS + (cln)])
static const char* const appPortName[NUM_COM] = PORT_NAMES;

static void resetAppQueue(int num, int cln) {
    queueOf(num, cln).len = 0;
    queueOf(num, cln).progressAt = millis();
}

// Adds data to the app's queue. Returns the number of bytes that did not fit.
static size_t queueForApp(int num, int cln, const uint8_t* data, size_t size) {
    AppQueue& q = queueOf(num, cln);
    if (q.len == 0) q.progressAt = millis();
    const size_t room = APP_QUEUE - q.len;
    const size_t n = size < room ? size : room;
    memcpy(q.data + q.len, data, n);
    q.len += n;
    return size - n;
}

// Sends as much of the queue as the app's socket takes now. False when the connection
// is lost; errorOut then holds the reason.
static bool flushAppQueue(WiFiClient& client, int num, int cln, int& errorOut) {
    AppQueue& q = queueOf(num, cln);
    if (q.len == 0) return true;
    const int fd = client.fd();
    if (fd < 0) {
        errorOut = 0;
        return false;
    }
    const int sent = send(fd, q.data, q.len, MSG_DONTWAIT);
    if (sent > 0) {
        memmove(q.data, q.data + sent, q.len - sent);
        q.len -= sent;
        q.progressAt = millis();
        return true;
    }
    errorOut = errno;
    return sent == 0 || errorOut == EAGAIN || errorOut == EWOULDBLOCK || errorOut == EINTR || errorOut == ENOMEM;
}

// Zander and FLARM: an app that falls behind skips the data that does not fit its queue,
// so the other apps and ports keep their data and its connection stays open. It is
// closed only after it took no data at all for APP_STALL_CLOSE_MS, as a dead connection.
static void serviceNavApps(int num) {
    for (int cln = 0; cln < MAX_NMEA_CLIENTS; cln++) {
        WiFiClient& client = TCPClient[num][cln];
        if (!client) continue;
        int error = 0;
        if (!flushAppQueue(client, num, cln, error)) {
            LOGI(LOG_PORTS, "%s: app %s connection lost (error %d), closed", appPortName[num],
                 client.remoteIP().toString().c_str(), error);
            client.stop();
            resetAppQueue(num, cln);
        } else if (queueOf(num, cln).len > 0 && millis() - queueOf(num, cln).progressAt >= APP_STALL_CLOSE_MS) {
            LOGW(LOG_PORTS, "%s: app %s took no data for %lu s, closed", appPortName[num],
                 client.remoteIP().toString().c_str(), (unsigned long)(APP_STALL_CLOSE_MS / 1000));
            client.stop();
            resetAppQueue(num, cln);
        }
    }
}

static void trackRadioStation(const WiFiClient& owner) {
    wifi_sta_list_t stations;
    wifi_sta_mac_ip_list_t addresses;
    if (!(WiFi.getMode() & WIFI_AP) || owner.localIP() != WiFi.softAPIP() ||
        esp_wifi_ap_get_sta_list(&stations) != ESP_OK ||
        esp_wifi_ap_get_sta_list_with_ip(&stations, &addresses) != ESP_OK)
        return;

    const IPAddress peer = owner.remoteIP();
    for (int i = 0; i < addresses.num; ++i) {
        if (IPAddress(addresses.sta[i].ip.addr) != peer) continue;
        portENTER_CRITICAL(&radioStationLock);
        memcpy(radioStationMac, addresses.sta[i].mac, sizeof(radioStationMac));
        radioStationTracked = true;
        portEXIT_CRITICAL(&radioStationLock);
        break;
    }
}

static void closeRadioOwner(WiFiClient& owner) {
    owner.stop();
    resetAppQueue(RADIO_COM, 0);
    portENTER_CRITICAL(&radioStationLock);
    radioStationTracked = false;
    radioStationLeft = false;
    portEXIT_CRITICAL(&radioStationLock);
}

// Why the owner is gone (for the web log), or nullptr while it is still there.
static const char* radioOwnerGone(WiFiClient& owner) {
    portENTER_CRITICAL(&radioStationLock);
    const bool left = radioStationLeft;
    portEXIT_CRITICAL(&radioStationLock);
    if (left) return "the phone left the hotspot";
    if (owner.available() > 0) return nullptr;
    if (owner.fd() < 0) return "connection closed";

    // Core 3.3.12 connected() treats a zero-length recv (TCP FIN) as connected.
    uint8_t byte;
    const int result = recv(owner.fd(), &byte, sizeof(byte), MSG_DONTWAIT | MSG_PEEK);
    if (result == 0) return "closed by the app";
    if (result < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) return "connection lost";
    return nullptr;
}

static void serviceRadioPort() {
    WiFiClient& owner = TCPClient[RADIO_COM][0];
    HardwareSerial& radio = *COM[RADIO_COM];
    if (owner.fd() >= 0) {
        if (const char* why = radioOwnerGone(owner)) {
            LOGI(LOG_RADIO, "App released the radio port: %s", why);
            closeRadioOwner(owner);
        }
    }

    if (server[RADIO_COM]->hasClient()) {
        WiFiClient candidate = server[RADIO_COM]->accept();
        // About 14 seconds (5 s idle + three 3 s probe intervals).
        const int idle = 5, interval = 3, probes = 3;
        if (candidate.fd() >= 0 && owner.fd() < 0 &&
            candidate.setSocketOption(IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) == 0 &&
            candidate.setSocketOption(IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) == 0 &&
            candidate.setSocketOption(IPPROTO_TCP, TCP_KEEPCNT, &probes, sizeof(probes)) == 0) {
            // Do not pass previously buffered UART bytes to the new owner.
            uart_flush_input(UART_NUM_2);
            closeRadioOwner(owner);  // Clear tracking if the core closed the previous socket itself.
            owner = candidate;
            trackRadioStation(owner);
            LOGI(LOG_RADIO, "App %s took the radio port", owner.remoteIP().toString().c_str());
        } else {
            // One line per 30 s at most: an app may retry every second.
            static uint32_t refusedAt = 0, refusedMore = 0;
            if (refusedAt == 0 || millis() - refusedAt >= 30000) {
                LOGW(LOG_RADIO, "App %s refused: %s (%lu more since the last line)", candidate.remoteIP().toString().c_str(),
                     candidate.fd() < 0 ? "connection failed" : owner.fd() >= 0 ? "another app owns the radio port" : "socket setup failed",
                     (unsigned long)refusedMore);
                refusedAt = millis() | 1u;
                refusedMore = 0;
            } else {
                refusedMore++;
            }
            candidate.stop();
        }
    }

    if (owner.available() > 0) {
        const int readCount = owner.read(buf1[RADIO_COM], BUFFERSIZE);
        if (readCount > 0) {
            const size_t sent = radio.write(buf1[RADIO_COM], readCount);
            STAT(RADIO_COM, toDevice, sent);
            MON(RADIO_COM, 1, buf1[RADIO_COM], sent);
            if (sent != (size_t)readCount) {
                LOGW(LOG_RADIO, "Radio took %u of %d bytes, session closed", (unsigned)sent, readCount);
                closeRadioOwner(owner);
            }
        }
    }

    if (radio.available()) {
        const size_t readCount = radio.read(buf2[RADIO_COM], BUFFERSIZE);
        if (readCount > 0) {
            STAT(RADIO_COM, fromDevice, readCount);
            STAT_RX(RADIO_COM);
            MON(RADIO_COM, 0, buf2[RADIO_COM], readCount);
            // Project (step 5): queued, never waited for; the session closes rather than lose bytes.
            if (owner.fd() >= 0 && queueForApp(RADIO_COM, 0, buf2[RADIO_COM], readCount) > 0) {
                STAT(RADIO_COM, shortWrites, 1);
                LOGW(LOG_RADIO, "App did not take %u radio bytes, session closed", (unsigned)readCount);
                closeRadioOwner(owner);
            }
        }
    }
    int error = 0;
    if (owner.fd() >= 0 && !flushAppQueue(owner, RADIO_COM, 0, error)) {
        LOGI(LOG_RADIO, "App released the radio port: connection lost (error %d)", error);
        closeRadioOwner(owner);
    }
    STAT_CLIENTS(RADIO_COM, owner.fd() >= 0 ? 1 : 0);
}
#endif

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
        logStart();  // Project: web log (step 3c), before anything logs
        delay(500);

#if defined(ESP32) && defined(PROTOCOL_TCP)
        WiFi.onEvent(radioStationDisconnected, ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
#endif

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
        WiFi.persistent(false);  // Project: the core keeps no second copy of the network password (step 3a)
        WiFi.setHostname(HOSTNAME);
        WiFi.mode(WIFI_STA);
        WiFi.begin(settings.netSsid, settings.netPass);  // Project: stored settings (step 3a)
        LOGI(LOG_WIFI, "Joining network %s for up to %u s", settings.netSsid, settings.joinSeconds);
        {
            const unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < settings.joinSeconds * 1000UL) {
                delay(250);
            }
        }
        if (WiFi.status() != WL_CONNECTED) {  // connected: logWifiEvent logs the address; reconnects on its own after a drop
            LOGW(LOG_WIFI, "Network %s not reached, opening the hotspot", settings.netSsid);
            WiFi.disconnect(true);
            WiFi.mode(WIFI_AP);
            if (!WiFi.softAP(settings.hotspotSsid, settings.hotspotPass)) {
                LOGE(LOG_WIFI, "Hotspot failed, restarting in 5 s");
                delay(5000);
                ESP.restart();
            }
            delay(2000);  // as upstream MODE_AP before softAPConfig
            const IPAddress hotspotIp(settings.hotspotIp);
            WiFi.softAPConfig(hotspotIp, hotspotIp, NETMASK);
            LOGI(LOG_WIFI, "Hotspot %s open at %s", settings.hotspotSsid, WiFi.softAPIP().toString().c_str());
        }
        if (MDNS.begin(HOSTNAME)) {
            MDNS.addService("_telnet", "_tcp", settings.port[0].tcpPort);
        } else {
            LOGW(LOG_WIFI, "Name %s.local not available (mDNS)", HOSTNAME);
        }
#endif

#ifdef OTA_HANDLER
        // Project: password and hostname (R13); messages go to the web log (step 3c)
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
            LOGI(LOG_SYSTEM, "Firmware update started (%s)", type.c_str());
        });
        ArduinoOTA.onEnd([]() { LOGI(LOG_SYSTEM, "Firmware update received, restarting"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            static unsigned int reported = 0;  // quarters reported
            const unsigned int quarter = total ? (unsigned int)(4ULL * progress / total) : 0;
            if (quarter == 0) reported = 0;
            else if (quarter > reported) LOGD(LOG_SYSTEM, "Firmware update %u %%", 25 * (reported = quarter));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            LOGE(LOG_SYSTEM, "Firmware update failed: %s (%u)",
                 error == OTA_AUTH_ERROR      ? "wrong password"
                 : error == OTA_BEGIN_ERROR   ? "could not begin"
                 : error == OTA_CONNECT_ERROR ? "could not connect"
                 : error == OTA_RECEIVE_ERROR ? "receive failed"
                 : error == OTA_END_ERROR     ? "could not finish"
                                              : "unknown",
                 (unsigned)error);
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
#ifdef ESP32
            if (num == RADIO_COM) continue;
#endif
            if (server[num]->hasClient()) {
                for (byte i = 0; i < MAX_NMEA_CLIENTS; i++) {
                    // find free/disconnected spot
                    if (!TCPClient[num][i] || !TCPClient[num][i].connected()) {
                        if (TCPClient[num][i]) TCPClient[num][i].stop();
                        TCPClient[num][i] = server[num]->available();
#ifdef ESP32
                        resetAppQueue(num, i);  // Project (step 5): a new app starts with an empty queue
#endif
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
#if defined(ESP32) && defined(PROTOCOL_TCP)
            if (num == RADIO_COM) {
                serviceRadioPort();
                continue;
            }
#endif
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
#ifdef ESP32
                serviceNavApps(num);  // Project (step 5): send queued data, close dead apps
#endif
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
#ifdef ESP32
                            if (queueForApp(num, cln, buf2[num], readCount) > 0) STAT(num, shortWrites, 1);  // Project (step 5): skipped
#else
                            if (TCPClient[num][cln].write(buf2[num], readCount) != (size_t)readCount)
                                STAT(num, shortWrites, 1);  // Project: count short writes
#endif
                    }
#endif

#ifdef ESP32
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
