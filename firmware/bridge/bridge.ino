/*********************************************************************************
 * Wi-Fi bridge firmware: Zander ZS1, FLARM and Becker AR6201 to OpenSoar
 * https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar (firmware/bridge)
 *
 * Forwards three UARTs to TCP over Wi-Fi (no UDP): Zander on UART0, FLARM on
 * UART1, the Becker radio (RS-422 through a MAX490) on UART2. Settings are in
 * config.h (with the public default logins), the status page in
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
#include <atomic>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>  // use SoftwareSerial for ESP8266
#endif

#include "config.h"
#include "weblog.h"  // Project: web log (step 3c); before every module that logs

#if defined(FALLBACK_TEST) || defined(BENCH_OPEN)  // Project: temporary test builds
#include <esp_ota_ops.h>
static bool otaStarted = false;
#endif
#include "bench.h"  // Project: bench build without OTA password (BENCH_OPEN only)

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

#ifdef ESP32
// Planned restart (0.5.18): every task stops accepting and closes the app sockets it owns,
// so the apps see the close and no connection accepted meanwhile survives into the reboot
// (S198). The loop task owns Zander, FLARM and Becker, the status task the voltage port.
static volatile bool appsClosing = false;       // a restart follows: stop accepting, close apps
static volatile bool navAppsClosed = false;     // loop task done
static volatile bool supplyAppsClosed = false;  // status task done
static volatile int navAppsClosedN = 0, supplyAppsClosedN = 0;

// Accept what is pending (hasClient() may hold an accepted socket that end() would leave
// open), close it, stop listening, then send a FIN on every app socket of this server.
static int closeServerApps(WiFiServer& srv, WiFiClient* clients, int count) {
    int closed = 0;
    for (int i = 0; i < 8 && srv.hasClient(); i++) {
        WiFiClient c = srv.accept();
        if (c.fd() >= 0 && shutdown(c.fd(), SHUT_RDWR) == 0) closed++;
        c.stop();
    }
    srv.end();
    for (int i = 0; i < count; i++) {
        const int fd = clients[i].fd();
        if (fd >= 0 && shutdown(fd, SHUT_RDWR) == 0) closed++;
    }
    return closed;
}

#ifdef PROTOCOL_TCP
// Project (step 4a, 0.5.20): the bridge also dials the app's "TCP port" on each phone
// (dialAppsTick below). One entry per phone and port; a connected session sits in an
// ordinary app slot of TCPClient and is closed with the others before a restart.
#define DIAL_TARGETS MAX_NMEA_CLIENTS  // phones per port, within the app limit
struct DialTarget {
    uint32_t ip;          // phone address as in ip4_addr (network order); 0 = entry unused
    int fd;               // connect in progress, -1 = none
    int8_t slot;          // app slot of the dialed session, -1 = none
    uint32_t attemptAt;   // start of the last attempt
    uint32_t failures;    // attempts without a session since the last one
    uint8_t silent;       // attempts in a row without any answer (no SYN-ACK, no reset)
};
#define DIAL_SUPPLY NUM_COM        // the voltage port, dialed from the status task
#define DIAL_PORTS (NUM_COM + 1)
static DialTarget dialTargets[DIAL_PORTS][DIAL_TARGETS];

// The port's task owns both the socket slots and their dial targets. Detach the
// old target before replacing a slot, even if the new socket has the same fd.
static void assignAppSocket(int port, int slot, WiFiClient& client, const WiFiClient& next) {
    for (DialTarget& target : dialTargets[port]) {
        if (!target.ip || target.slot != slot) continue;
        target.slot = -1;
        target.attemptAt = millis();
    }
    client = next;
}

// 0.5.22: one connect in progress at a time across both tasks (the socket budget reserves one).
static std::atomic<int> dialInProgress{0};
static void dialDropPending(int p, int k) {  // indices: sketch prototypes come before the struct
    DialTarget& t = dialTargets[p][k];
    if (t.fd < 0) return;
    close(t.fd);
    t.fd = -1;
    dialInProgress.store(0);
}

// Abandon connects in progress for ports first..last (no session exists for them yet).
// Each task for its own ports.
static void closeDialPending(int first, int last) {
    for (int num = first; num <= last; num++)
        for (int k = 0; k < DIAL_TARGETS; k++)
            if (dialTargets[num][k].ip) dialDropPending(num, k);
}
#endif

// Loop task only.
static void closeNavApps() {
    if (navAppsClosed) return;
    int closed = 0;
#ifdef PROTOCOL_TCP
    closeDialPending(0, NUM_COM - 1);
#endif
    for (int num = 0; num < NUM_COM; num++) closed += closeServerApps(*server[num], TCPClient[num], MAX_NMEA_CLIENTS);
    navAppsClosedN = closed;
    navAppsClosed = true;
}
#endif

// Project (0.5.21): one transfer buffer instead of two per port (6 kB). Only the loop task
// uses it, and each use ends (data written to the UART or queued for the apps) before the next.
uint8_t ioBuf[BUFFERSIZE];

#ifdef ESP32
uint16_t i1[NUM_COM] = {0, 0, 0};
#elif defined(ESP8266)
uint16_t i1[NUM_COM] = {0, 0};
#endif

uint8_t BTbuf[BUFFERSIZE];
uint16_t iBT = 0;

#include "settings.h"     // Project: stored settings (step 3a)
#if defined(ESP32) && defined(STATUS_PAGE) && defined(STORED_SETTINGS)
#define NAV_DETECTION
#include "nav_scan.h"
#endif
#ifdef ESP32
// Every app connection: TCP keepalive after 5 s idle, three probes 3 s apart, so the
// connection of a phone that left (Wi-Fi off, out of range) is closed after about 14 s
// even on a quiet port. The core turns keepalive on at accept, with lwIP's 2 h default.
static bool appKeepalive(WiFiClient& c) {
    const int idle = 5, interval = 3, probes = 3;
    return c.setSocketOption(IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) == 0 &&
           c.setSocketOption(IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) == 0 &&
           c.setSocketOption(IPPROTO_TCP, TCP_KEEPCNT, &probes, sizeof(probes)) == 0;
}
#endif

#if defined(ESP32) && defined(PROTOCOL_TCP)
// 0.5.21: socket budget. lwIP has 16 sockets in all (fixed in the core libraries): 5 listening
// ports, OTA, the web connection, the network self-check, one dial attempt and one socket to
// refuse a surplus connection leave 6 for apps. With 13 app connections (the port limits) the
// web server and the self-check found no socket and the self-check restarted the bridge.
// Dialed sessions count; the one connect in progress has its own reserve. Owner decision (0.5.22).
#define APP_SOCKET_BUDGET 6
static std::atomic<int> navAppSockets{0};     // loop task: Zander, FLARM, Becker
static std::atomic<int> supplyAppSockets{0};  // status task: voltage port
static bool appBudgetFree() { return navAppSockets.load() + supplyAppSockets.load() < APP_SOCKET_BUDGET; }

// Loop task only.
static void countNavAppSockets() {
    int n = 0;
    for (int num = 0; num < NUM_COM; num++) {
        for (int i = 0; i < MAX_NMEA_CLIENTS; i++) n += TCPClient[num][i].fd() >= 0;
    }
    navAppSockets.store(n);
}

// A surplus connection: accepted and closed at once, one log line per 30 s at most.
static void refuseOverBudget(const char* port, WiFiClient c) {
    static uint32_t loggedAt = 0, more = 0;
    if (loggedAt == 0 || (int32_t)(millis() - loggedAt) >= 30000) {
        LOGW(LOG_PORTS, "%s: app %s refused, %d app connections in use (limit; %lu more since the last line)", port,
             c.remoteIP().toString().c_str(), APP_SOCKET_BUDGET, (unsigned long)more);
        loggedAt = millis() | 1u;
        more = 0;
    } else {
        more++;
    }
    c.stop();
}

static void lastPhonePersistTick();  // status task
#endif

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
#define NAV_RESYNC_MAX 512  // skip at most this many bytes looking for a line end (NMEA sentences are up to 82)
struct AppQueue {
    uint16_t len;
    uint16_t skipped;     // Zander and FLARM: bytes dropped since data stopped fitting; 0 = in step
    uint32_t progressAt;  // last time the app took data, or its queue was empty
    uint8_t data[APP_QUEUE];
};
// 0.5.15: a queue is taken from the heap only while its app is connected (a phone uses
// four, about 8 kB), not for all twelve slots (24 kB). The heap ran out under load (S186).
// void*, since the sketch's generated prototypes come before the struct.
static void* appQueues[NUM_COM][MAX_NMEA_CLIENTS];
#define queueOf(num, cln) (*static_cast<AppQueue*>(appQueues[num][cln]))
static const char* const appPortName[NUM_COM] = PORT_NAMES;

// Gives a newly accepted app an empty queue. False when the heap has no room for one.
static bool openAppQueue(int num, int cln) {
    if (!appQueues[num][cln]) appQueues[num][cln] = malloc(sizeof(AppQueue));
    if (!appQueues[num][cln]) return false;
    queueOf(num, cln).len = 0;
    queueOf(num, cln).skipped = 0;
    queueOf(num, cln).progressAt = millis();
    return true;
}

static void freeAppQueue(int num, int cln) {
    free(appQueues[num][cln]);
    appQueues[num][cln] = nullptr;
}

// Below this much free heap, apps that took no data for APP_GUARD_IDLE_MS get nothing
// more sent until it recovers: their data waits in (and is skipped from) the fixed queue
// instead of filling network buffers that Wi-Fi joins, the web pages and the saved log need.
#define APP_MEMORY_GUARD (16 * 1024)
#define APP_GUARD_IDLE_MS 1000

// Adds data to the app's queue. Returns the number of bytes that did not fit.
static size_t queueForApp(int num, int cln, const uint8_t* data, size_t size) {
    if (!appQueues[num][cln]) return size;
    AppQueue& q = queueOf(num, cln);
    if (q.len == 0) q.progressAt = millis();
    const size_t room = APP_QUEUE - q.len;
    const size_t n = size < room ? size : room;
    memcpy(q.data + q.len, data, n);
    q.len += n;
    return size - n;
}

static const uint8_t* lastLineEnd(const uint8_t* data, size_t size) {
    for (size_t i = size; i > 0; i--)
        if (data[i - 1] == '\n') return data + i;
    return nullptr;
}

// Zander and FLARM (0.5.10): like queueForApp, but an app that falls behind skips whole
// lines, so it never gets the start of one sentence joined to the end of another. When
// data stops fitting, the queue is cut back to its last line end and incoming data is
// dropped up to the next line end. Data without line ends (no NMEA) is cut as before.
// Returns the number of bytes dropped.
static size_t queueNavForApp(int num, int cln, const uint8_t* data, size_t size) {
    if (!appQueues[num][cln]) return size;
    AppQueue& q = queueOf(num, cln);
    size_t dropped = 0;
    if (q.skipped > 0) {
        const uint8_t* end = static_cast<const uint8_t*>(memchr(data, '\n', size));
        if (!end && q.skipped + size < NAV_RESYNC_MAX) {
            q.skipped += size;
            return size;
        }
        const size_t skip = end ? (size_t)(end + 1 - data) : 0;  // no line end in sight: give up waiting
        q.skipped = 0;
        data += skip;
        size -= skip;
        dropped = skip;
    }
    if (size == 0) return dropped;
    const size_t rest = queueForApp(num, cln, data, size);
    if (rest == 0) return dropped;
    const uint8_t* keep = lastLineEnd(q.data, q.len);
    if (!keep) return dropped + rest;  // no line structure: plain cut
    const size_t cut = q.len - (size_t)(keep - q.data);
    q.len -= cut;
    q.skipped = 1;
    return dropped + rest + cut;
}

// Sends as much of the queue as the app's socket takes now. False when the connection
// is lost; errorOut then holds the reason.
static bool flushAppQueue(WiFiClient& client, int num, int cln, int& errorOut, bool memoryLow = false) {
    if (!appQueues[num][cln]) return true;
    AppQueue& q = queueOf(num, cln);
    if (q.len == 0) return true;
    if (memoryLow && millis() - q.progressAt >= APP_GUARD_IDLE_MS) return true;  // lagging app: wait for memory
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
    const size_t heapFree = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    const bool memoryLow = heapFree < APP_MEMORY_GUARD;
    if (memoryLow) {  // one line per minute at most
        static uint32_t lowAt = 0;
        if (lowAt == 0 || (int32_t)(millis() - lowAt) >= 60000) {  // signed: lowAt may be 1 ms ahead (| 1u)
            LOGW(LOG_SYSTEM, "Memory low (%u kB free): apps that are behind get no new data until it recovers",
                 (unsigned)(heapFree / 1024));
            lowAt = millis() | 1u;
        }
    }
    for (int cln = 0; cln < MAX_NMEA_CLIENTS; cln++) {
        WiFiClient& client = TCPClient[num][cln];
        if (!client) {
            if (appQueues[num][cln]) freeAppQueue(num, cln);  // the core closed it: give the memory back
            continue;
        }
        if (!appQueues[num][cln]) continue;
        int error = 0;
        if (!flushAppQueue(client, num, cln, error, memoryLow)) {
            LOGI(LOG_PORTS, "%s: app %s connection lost (error %d), closed", appPortName[num],
                 client.remoteIP().toString().c_str(), error);
            client.stop();
            freeAppQueue(num, cln);
        } else if (queueOf(num, cln).len > 0 && millis() - queueOf(num, cln).progressAt >= APP_STALL_CLOSE_MS) {
            LOGW(LOG_PORTS, "%s: app %s took no data for %lu s, closed", appPortName[num],
                 client.remoteIP().toString().c_str(), (unsigned long)(APP_STALL_CLOSE_MS / 1000));
            client.stop();
            freeAppQueue(num, cln);
        }
    }
}

// Project (step 4a, 0.5.20/0.5.21): the bridge also dials the app. OpenSoar's "TCP port"
// entry listens on the phone and takes each new connection in place of the old one, so a
// session left stale by a bridge power cut or a hotspot rejoin is replaced as soon as the
// bridge dials again (S204, S206). With mobile data on, connections the phone opens go out
// over mobile data; connections from the bridge reach it over Wi-Fi (S207, S209).
// Hotspot mode: every phone on the hotspot, on all four ports (increment 2). Connects never
// wait: a socket in progress is checked on each pass. A connected session takes a free app
// slot of its port and from then on runs like an accepted app (queue, keepalive, closes).
// Zander, FLARM and Becker are dialed from the loop task, the voltage port from the status
// task (each task owns its sockets). Becker: dialed only while no app owns the radio.
#define DIAL_RETRY_MS 3000    // one attempt per phone and port every 3 s, never given up
#define DIAL_TIMEOUT_MS 1000  // a connect without an answer is dropped after this (a phone on the hotspot answers in ms)
#define DIAL_LIST_MS 1000     // hotspot station list read once a second
// A station that drops connection attempts without any answer (a laptop firewall; a phone
// answers at once, with a connection or a reset) would hold the one connect in progress
// most of the time and delay the dials to the phones: after DIAL_SILENT_LIMIT silent attempts
// in a row it is tried every DIAL_SILENT_RETRY_MS only (S211 follow-up; owner decision).
#define DIAL_SILENT_LIMIT 3
#define DIAL_SILENT_RETRY_MS 30000

static void closeRadioOwner(WiFiClient& owner);
static void trackRadioStation(const WiFiClient& owner);
static int supplyFreeSlot();  // supply.h, status task
static void supplyAdopt(int slot, WiFiClient& session);
static int supplySlotFd(int slot);
static void supplyEndSlot(int slot, const char* why);

static int freeAppSlot(int num) {
    for (int i = 0; i < MAX_NMEA_CLIENTS; i++)
        if (!TCPClient[num][i]) return i;  // connected() also drops a slot the core found closed
    return -1;
}

static const char* dialPortName(int p) { return p == DIAL_SUPPLY ? "Supply voltage" : appPortName[p]; }
static uint16_t dialTcpPort(int p) { return p == DIAL_SUPPLY ? settings.supplyTcp : settings.port[p].tcpPort; }

static int dialFreeSlot(int p) {
    if (p == DIAL_SUPPLY) return supplyFreeSlot();
    if (p == RADIO_COM) return TCPClient[RADIO_COM][0].fd() < 0 ? 0 : -1;  // one owner
    return freeAppSlot(p);
}

static int dialSlotFd(int p, int slot) { return p == DIAL_SUPPLY ? supplySlotFd(slot) : TCPClient[p][slot].fd(); }

static bool dialSessionAlive(int p, int k) {
    const DialTarget& t = dialTargets[p][k];
    return t.slot >= 0 && dialSlotFd(p, t.slot) >= 0;
}

static void endDialSession(int p, int k, const char* why) {
    DialTarget& t = dialTargets[p][k];
    if (dialSessionAlive(p, k)) {
        if (p == DIAL_SUPPLY) {
            supplyEndSlot(t.slot, why);  // logs itself
        } else {
            LOGI(LOG_PORTS, "%s: dialed app %s closed: %s", dialPortName(p), IPAddress(t.ip).toString().c_str(), why);
            if (p == RADIO_COM) {
                closeRadioOwner(TCPClient[RADIO_COM][0]);
            } else {
                TCPClient[p][t.slot].stop();
                freeAppQueue(p, t.slot);
            }
        }
    }
    t.slot = -1;
}

static void dialFailed(int p, int k, int error) {
    DialTarget& t = dialTargets[p][k];
    dialDropPending(p, k);
    if (error != ETIMEDOUT) t.silent = 0;
    else if (++t.silent == DIAL_SILENT_LIMIT)
        LOGI(LOG_PORTS, "%s: %s does not answer at all, trying every %u s", dialPortName(p), IPAddress(t.ip).toString().c_str(),
             (unsigned)(DIAL_SILENT_RETRY_MS / 1000));
    if (t.failures++ == 0)  // first failure only; the next attempts are counted
        LOGI(LOG_PORTS, "%s: no app answers at %s:%u (%s), trying every %u s", dialPortName(p),
             IPAddress(t.ip).toString().c_str(), (unsigned)dialTcpPort(p),
             error == ECONNREFUSED || error == ECONNRESET ? "refused" : error == ETIMEDOUT ? "no answer" : strerror(error),
             (unsigned)(DIAL_RETRY_MS / 1000));
}

static void dialStart(int p, int k) {
    DialTarget& t = dialTargets[p][k];
    if (dialFreeSlot(p) < 0) return;  // port full (Becker: owned): checked again on the next pass
    if (p != DIAL_SUPPLY) countNavAppSockets();
    if (!appBudgetFree()) return;  // socket budget full: try again later
    int idle = 0;
    if (!dialInProgress.compare_exchange_strong(idle, 1)) return;  // another connect in progress: next pass
    t.attemptAt = millis();  // only a real attempt starts the retry time
    const int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        dialInProgress.store(0);
        dialFailed(p, k, errno);  // no socket left
        return;
    }
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dialTcpPort(p));
    addr.sin_addr.s_addr = t.ip;
    t.fd = fd;
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0 && errno != EINPROGRESS)
        dialFailed(p, k, errno);
}

// A connect in progress: done, failed or still waiting (never waits itself).
static void dialCheck(int p, int k) {
    DialTarget& t = dialTargets[p][k];
    fd_set writable, failed;
    FD_ZERO(&writable);
    FD_ZERO(&failed);
    FD_SET(t.fd, &writable);
    FD_SET(t.fd, &failed);
    timeval now = {0, 0};
    const int ready = select(t.fd + 1, nullptr, &writable, &failed, &now);
    if (ready == 0) {
        if (millis() - t.attemptAt >= DIAL_TIMEOUT_MS) dialFailed(p, k, ETIMEDOUT);
        return;
    }
    int error = 0;
    socklen_t len = sizeof(error);
    if (ready < 0 || getsockopt(t.fd, SOL_SOCKET, SO_ERROR, &error, &len) != 0 || error != 0) {
        dialFailed(p, k, error ? error : errno);
        return;
    }
    const int slot = dialFreeSlot(p);
    if (slot < 0) {  // an accepted app took the last slot (Becker: the radio) meanwhile: no failure
        dialDropPending(p, k);
        return;
    }
    fcntl(t.fd, F_SETFL, fcntl(t.fd, F_GETFL, 0) & ~O_NONBLOCK);  // as accepted sockets; sends use MSG_DONTWAIT
    WiFiClient session(t.fd);  // owns the socket from here
    t.fd = -1;
    dialInProgress.store(0);
    session.setNoDelay(true);
    if (!appKeepalive(session) || (p != DIAL_SUPPLY && !openAppQueue(p, slot))) {
        session.stop();
        dialFailed(p, k, ENOMEM);
        return;
    }
    if (p == DIAL_SUPPLY) {
        supplyAdopt(slot, session);
    } else if (p == RADIO_COM) {
        uart_flush_input(UART_NUM_2);  // no earlier radio bytes for the new owner
        assignAppSocket(p, slot, TCPClient[p][slot], session);
        trackRadioStation(session);
    } else {
        assignAppSocket(p, slot, TCPClient[p][slot], session);
    }
    t.slot = slot;
    LOGI(LOG_PORTS, "%s: dialed app %s connected (attempt %lu)", dialPortName(p), IPAddress(t.ip).toString().c_str(),
         (unsigned long)t.failures + 1);
    t.failures = 0;
    t.silent = 0;
}

// Network mode dials the configured address, the latest accepted instrument client,
// or the gateway. The loop task learns the address and the status task saves it.
#define DIAL_NVS "bridge-dial"
#define DIAL_NVS_LAST "last"
static constexpr uint32_t PHONE_SAVE_INTERVAL_MS = 5000;
static std::atomic<uint32_t> dialNetTarget{0};  // as in ip4_addr; 0 = none (not on a network)
static std::atomic<uint32_t> lastPhoneIp{0};    // loop task writes, status task reads
static uint32_t savedPhoneIp = 0;              // status task after setup

static void lastPhoneLoad() {
    nvs_handle_t nvs;
    if (nvs_open(DIAL_NVS, NVS_READONLY, &nvs) != ESP_OK) return;  // never saved yet
    uint32_t ip = 0;
    if (nvs_get_u32(nvs, DIAL_NVS_LAST, &ip) == ESP_OK && validIp(ip)) {
        savedPhoneIp = ip;
        lastPhoneIp.store(ip);
    }
    nvs_close(nvs);
}

// Called only when an inbound instrument connection takes a slot.
static void rememberPhone(const WiFiClient& client) {
    if (settings.phoneIp || (WiFi.getMode() & WIFI_AP) || WiFi.status() != WL_CONNECTED) return;
    const uint32_t ip = (uint32_t)client.remoteIP();
    if (ip == lastPhoneIp.load() || ip == (uint32_t)WiFi.localIP() || !validIp(ip)) return;
    lastPhoneIp.store(ip);
}

static void lastPhonePersistTick() {
    const uint32_t ip = lastPhoneIp.load();
    if (!ip || ip == savedPhoneIp) return;

    // Coalesce rapid changes and pace retries after a flash error.
    static uint32_t attemptedAt = 0u - PHONE_SAVE_INTERVAL_MS;
    const uint32_t now = millis();
    if (now - attemptedAt < PHONE_SAVE_INTERVAL_MS) return;
    attemptedAt = now;

    nvs_handle_t nvs;
    esp_err_t result = nvs_open(DIAL_NVS, NVS_READWRITE, &nvs);
    if (result == ESP_OK) {
        result = nvs_set_u32(nvs, DIAL_NVS_LAST, ip);
        if (result == ESP_OK) result = nvs_commit(nvs);
        nvs_close(nvs);
    }
    // A failed commit leaves the stored value uncertain until a write succeeds.
    savedPhoneIp = result == ESP_OK ? ip : 0;
    if (result != ESP_OK) LOGW(LOG_PORTS, "Could not save the phone address (0x%x)", unsigned(result));
}

// Loop task, network mode only.
static void dialNetTargetTick() {
    static uint32_t at = 0;
    const uint32_t now = millis();
    if ((WiFi.getMode() & WIFI_AP) || (at && now - at < DIAL_LIST_MS)) return;
    at = now;
    if (WiFi.status() != WL_CONNECTED) {
        dialNetTarget.store(0);
        return;
    }
    const uint32_t own = (uint32_t)WiFi.localIP();
    const uint32_t learned = lastPhoneIp.load();
    const char* source = "the settings page";
    uint32_t target = settings.phoneIp;
    if (!target && learned) target = learned, source = "the last phone seen";
    if (!target) target = (uint32_t)WiFi.gatewayIP(), source = "the gateway";
    if (target == own || !validIp(target)) target = 0;
    if (target == dialNetTarget.load()) return;
    if (target) LOGI(LOG_PORTS, "Network mode: dialing the app at %s (%s)", IPAddress(target).toString().c_str(), source);
    dialNetTarget.store(target);
}

// Keeps one dialed session per phone on the hotspot (network mode: per target) for ports
// first..last. Loop task: Zander, FLARM, Becker; status task: DIAL_SUPPLY.
static void dialAppsTick(int first, int last) {
    const bool hotspot = WiFi.getMode() & WIFI_AP;
    if (!hotspot) {  // network mode: entry 0 follows the target
        const uint32_t target = dialNetTarget.load();
        for (int p = first; p <= last; p++) {
            DialTarget& t = dialTargets[p][0];
            if (t.ip == target) continue;
            if (t.ip) {
                dialDropPending(p, 0);
                endDialSession(p, 0, target ? "the phone address changed" : "the network connection was lost");
            }
            t = {target, -1, -1, millis() - DIAL_RETRY_MS, 0, 0};  // dial at once
        }
    }
    static uint32_t listAt[DIAL_PORTS];
    if (hotspot && (listAt[first] == 0 || millis() - listAt[first] >= DIAL_LIST_MS)) {
        listAt[first] = millis() | 1u;
        wifi_sta_list_t stations;
        wifi_sta_mac_ip_list_t addresses;
        addresses.num = 0;
        if (esp_wifi_ap_get_sta_list(&stations) == ESP_OK)
            esp_wifi_ap_get_sta_list_with_ip(&stations, &addresses);
        for (int p = first; p <= last; p++) {
            for (int k = 0; k < DIAL_TARGETS; k++) {  // phones that left: end their session
                DialTarget& t = dialTargets[p][k];
                if (!t.ip) continue;
                bool present = false;
                for (int i = 0; i < addresses.num; i++) present |= addresses.sta[i].ip.addr == t.ip;
                if (present) continue;
                dialDropPending(p, k);
                endDialSession(p, k, "the phone left the hotspot");
                t.ip = 0;
            }
            for (int i = 0; i < addresses.num; i++) {  // new phones (no address yet: next time)
                const uint32_t ip = addresses.sta[i].ip.addr;
                if (!ip) continue;
                int freeK = -1, known = -1;
                for (int k = 0; k < DIAL_TARGETS; k++) {
                    if (dialTargets[p][k].ip == ip) known = k;
                    else if (!dialTargets[p][k].ip && freeK < 0) freeK = k;
                }
                if (known >= 0 || freeK < 0) continue;
                dialTargets[p][freeK] = {ip, -1, -1, millis() - DIAL_RETRY_MS, 0, 0};  // dial at once
            }
        }
    }
    for (int p = first; p <= last; p++) {
        for (int k = 0; k < DIAL_TARGETS; k++) {
            DialTarget& t = dialTargets[p][k];
            if (!t.ip) continue;
            if (t.fd >= 0) {
                dialCheck(p, k);
                continue;
            }
            if (t.slot >= 0) {
                if (!dialSessionAlive(p, k)) {  // closed elsewhere (send error, stall, keepalive, app left)
                    t.slot = -1;
                    t.attemptAt = millis();  // next attempt in DIAL_RETRY_MS
                    continue;
                }
                if (p == RADIO_COM || p == DIAL_SUPPLY) continue;  // their own code sees a close by the app
                // Core 3.3.12 connected() keeps a socket with a FIN; the app closed or replaced it.
                uint8_t byte;
                WiFiClient& c = TCPClient[p][t.slot];
                if (c.available() == 0 && recv(c.fd(), &byte, 1, MSG_DONTWAIT | MSG_PEEK) == 0) {
                    endDialSession(p, k, "the app closed it");
                    t.attemptAt = millis();
                }
                continue;
            }
            if (millis() - t.attemptAt >= (t.silent >= DIAL_SILENT_LIMIT ? DIAL_SILENT_RETRY_MS : DIAL_RETRY_MS)) dialStart(p, k);
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
    freeAppQueue(RADIO_COM, 0);
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
#ifdef STATUS_PAGE
    // 0.5.10: a receive error on the radio line (buffer full, FIFO overflow, framing) means
    // bytes from the radio were lost or damaged; close the session rather than pass a gap on.
    static uint32_t radioErrorsSeen = 0;
    const uint32_t radioErrors = portStats[RADIO_COM].uartErrors.load(std::memory_order_relaxed);
    if (radioErrors != radioErrorsSeen) {
        radioErrorsSeen = radioErrors;
        if (owner.fd() >= 0) {
            LOGW(LOG_RADIO, "Radio receive error, session closed so no damaged reply reaches the app");
            closeRadioOwner(owner);
        }
    }
#endif
    if (owner.fd() >= 0) {
        if (const char* why = radioOwnerGone(owner)) {
            LOGI(LOG_RADIO, "App released the radio port: %s", why);
            closeRadioOwner(owner);
        }
    }

    if (server[RADIO_COM]->hasClient()) {
        WiFiClient candidate = server[RADIO_COM]->accept();
        countNavAppSockets();
        if (candidate.fd() >= 0 && owner.fd() < 0 && !appBudgetFree()) {
            refuseOverBudget(appPortName[RADIO_COM], candidate);
        } else if (candidate.fd() >= 0 && owner.fd() < 0 && appKeepalive(candidate)) {
            // Do not pass previously buffered UART bytes to the new owner.
            uart_flush_input(UART_NUM_2);
            closeRadioOwner(owner);  // Clear tracking if the core closed the previous socket itself.
            if (!openAppQueue(RADIO_COM, 0)) {
                LOGW(LOG_RADIO, "App %s refused: no memory for its queue", candidate.remoteIP().toString().c_str());
                candidate.stop();
            } else {
                assignAppSocket(RADIO_COM, 0, owner, candidate);
                trackRadioStation(owner);
                rememberPhone(owner);
                LOGI(LOG_RADIO, "App %s took the radio port", owner.remoteIP().toString().c_str());
            }
        } else {
            // One line per 30 s at most: an app may retry every second.
            static uint32_t refusedAt = 0, refusedMore = 0;
            if (refusedAt == 0 || (int32_t)(millis() - refusedAt) >= 30000) {  // signed, as above
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
        const int readCount = owner.read(ioBuf, BUFFERSIZE);
        if (readCount > 0) {
            const size_t sent = radio.write(ioBuf, readCount);
            STAT(RADIO_COM, toDevice, sent);
            MON(RADIO_COM, 1, ioBuf, sent);
            if (sent != (size_t)readCount) {
                LOGW(LOG_RADIO, "Radio took %u of %d bytes, session closed", (unsigned)sent, readCount);
                closeRadioOwner(owner);
            }
        }
    }

    if (radio.available()) {
        const size_t readCount = radio.read(ioBuf, BUFFERSIZE);
        if (readCount > 0) {
            STAT(RADIO_COM, fromDevice, readCount);
            STAT_RX(RADIO_COM);
            MON(RADIO_COM, 0, ioBuf, readCount);
            // Project (step 5): queued, never waited for; the session closes rather than lose bytes.
            if (owner.fd() >= 0 && queueForApp(RADIO_COM, 0, ioBuf, readCount) > 0) {
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

#if defined(ESP32) && defined(MODE_STA_AP)
// Project (0.5.21): at most 16 Wi-Fi receive and 16 transmit buffers instead of the core's
// 32 each (about 1.6 kB per buffer). Bursts of network traffic took the free heap to 0
// (S199); with the cap the radio drops packets instead, which TCP resends.
// The core only sets its counts when it starts Wi-Fi, and esp_wifi_init() does nothing
// when Wi-Fi is already initialized (ESP-IDF 5.5.5), so starting it here first wins.
// Called once, before the first WiFi.mode(); the switch to the hotspot keeps Wi-Fi initialized.
#define WIFI_BUFFERS 16
static void wifiInitCapped() {
    if (!Network.begin()) return;  // the core retries and logs
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.static_tx_buf_num = 0;  // as the core: dynamic buffers only
    cfg.dynamic_tx_buf_num = WIFI_BUFFERS;
    cfg.tx_buf_type = 1;
    cfg.cache_tx_buf_num = 4;
    cfg.static_rx_buf_num = 4;
    cfg.dynamic_rx_buf_num = WIFI_BUFFERS;
    const esp_err_t err = esp_wifi_init(&cfg);
    if (err != ESP_OK) LOGW(LOG_WIFI, "Wi-Fi buffer limit not set (%s); core defaults apply", esp_err_to_name(err));
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
#ifdef CHIP_TEMP
        chipTempCaptureBoot();
#endif
        logStart();  // Project: web log (step 3c), before anything logs
#ifdef CHIP_TEMP
        chipTempStartBootSeries();
#endif
        delay(500);

#if defined(ESP32) && defined(PROTOCOL_TCP)
        WiFi.onEvent(radioStationDisconnected, ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
#endif

#ifdef ESP32
        settingsLoad();  // Project: defaults from config.h, overridden by valid stored values (step 3a)
#ifdef PROTOCOL_TCP
        lastPhoneLoad();  // increment 3: the phone to dial in network mode
#endif
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
        wifiInitCapped();  // 0.5.21
        WiFi.mode(WIFI_STA);
        if (settings.netSsid[0]) {  // Project: no network set (the default): hotspot at once
            WiFi.begin(settings.netSsid, settings.netPass);  // Project: stored settings (step 3a)
            LOGI(LOG_WIFI, "Joining network %s for up to %u s", settings.netSsid, settings.joinSeconds);
            const unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < settings.joinSeconds * 1000UL) {
                delay(250);
            }
        }
        if (WiFi.status() != WL_CONNECTED) {  // connected: logWifiEvent logs the address; reconnects on its own after a drop
            if (settings.netSsid[0]) LOGW(LOG_WIFI, "Network %s not reached, opening the hotspot", settings.netSsid);
            else LOGI(LOG_WIFI, "No network set, opening the hotspot");
            WiFi.disconnect();  // 0.5.21: Wi-Fi stays initialized, so the buffer limit of wifiInitCapped()
                                // holds; disconnect(true) shut it down, and a second init failed
                                // (wifi_init_default: netstack cb reg failed with 12308)
            WiFi.mode(WIFI_AP);
            if (!WiFi.softAP(settings.hotspotSsid, settings.hotspotPass, HOTSPOT_CHANNEL)) {
                LOGE(LOG_WIFI, "Hotspot failed, restarting in 5 s");
                delay(5000);
                ESP.restart();
            }
            delay(2000);  // as upstream MODE_AP before softAPConfig
            const IPAddress hotspotIp(settings.hotspotIp);
            WiFi.softAPConfig(hotspotIp, hotspotIp, NETMASK);
            {  // 0.5.21: the DHCP answers carry no router (default gateway). The hotspot has no
               // internet, so a phone must not send its internet traffic here, whatever its user
               // answers to "no internet, stay connected?"; data reaches the app by the dial (S208).
                esp_netif_t* ap = WiFi.AP.netif();
                uint8_t offerRouter = 0;
                const bool ok = ap && esp_netif_dhcps_stop(ap) != ESP_FAIL &&
                                esp_netif_dhcps_option(ap, ESP_NETIF_OP_SET, ESP_NETIF_ROUTER_SOLICITATION_ADDRESS,
                                                       &offerRouter, sizeof(offerRouter)) == ESP_OK &&
                                esp_netif_dhcps_start(ap) == ESP_OK;
                if (!ok) LOGW(LOG_WIFI, "Hotspot: could not turn off the gateway in DHCP answers");
            }
            // 0.5.21: a phone that joined before softAPConfig got an address from the core's default
            // range (192.168.4.x) and cannot reach the bridge (S209); send it off once to rejoin.
            if (const int early = WiFi.softAPgetStationNum()) {
                esp_wifi_deauth_sta(0);
                LOGI(LOG_WIFI, "Hotspot: %d early device(s) asked to rejoin for a 10.10.10.x address", early);
            }
            LOGI(LOG_WIFI, "Hotspot %s open at %s", settings.hotspotSsid, WiFi.softAPIP().toString().c_str());
        }
        // 0.5.21: no mDNS (owner decision); the bridge is reached by its address
#endif

#ifdef OTA_HANDLER
        // Project: password and hostname (R13); messages go to the web log (step 3c)
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setMdnsEnabled(false);  // 0.5.21: no mDNS
#ifndef BENCH_OPEN  // bench build: OTA without password (bench.h)
        ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
        ArduinoOTA.onStart([]() {
#if defined(FALLBACK_TEST) || defined(BENCH_OPEN)
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
        ArduinoOTA.onEnd([]() {
            LOGI(LOG_SYSTEM, "Firmware update received, restarting");
#ifdef STORED_SETTINGS
            closeAppsBeforeRestart(true);  // loop task; apps reconnect to the new firmware by themselves
#endif
        });
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
#ifdef BENCH_OPEN
        benchBegin();
#else
        benchForget();  // Project: a later bench build may revert to this image
#endif
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

#if defined(ESP32) && defined(STATUS_PAGE)
    // 0.5.21: loop-task watch on the status task (self-healing rule). A stuck status task also
    // stops the network self-check, so this check lives here. Normal build: planned restart.
    // Bench build: deliberate abort, so /api/coredump shows where the status task hung.
#define STATUS_STALL_MS 60000
    static void statusWatchTick() {
        static uint32_t checkedAt = 0;
        if (millis() - checkedAt < 1000) return;
        checkedAt = millis();
        const uint32_t beat = statusBeat.load(std::memory_order_relaxed);
        if (beat == 0 || (int32_t)(millis() - beat) < (int32_t)STATUS_STALL_MS) return;  // signed: beat may be 1 ms ahead
        LOGE(LOG_SYSTEM, "Status task stuck for %lu s: restarting the bridge", (unsigned long)((millis() - beat) / 1000));
#ifdef BENCH_OPEN
        delay(200);
        abort();  // core dump with every task's stack
#else
        closeAppsBeforeRestart(true);
        ESP.restart();
#endif
    }
#endif

    void loop() {
#ifdef ESP32
        if (appsClosing) {  // a planned restart from the status task: no more accepts or forwarding
            closeNavApps();
            delay(10);
            return;
        }
#endif
#ifdef FALLBACK_TEST
        revertIfIdle();
#endif
#ifdef BENCH_OPEN
        benchLoop(otaStarted);
#endif
#if defined(ESP32) && defined(STATUS_PAGE)
        statusWatchTick();
#endif
#ifdef OTA_HANDLER
        ArduinoOTA.handle();
#endif

#ifdef ESP32
#ifdef BLUETOOTH
        if (SerialBT.hasClient()
#ifdef NAV_DETECTION
            && navPort.load() != BLUETOOTH
#endif
        ) {
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
#ifdef ESP32
            countNavAppSockets();
            if (server[num]->hasClient() && !appBudgetFree()) {
                refuseOverBudget(appPortName[num], server[num]->accept());
            } else
#endif
            if (server[num]->hasClient()) {
                for (byte i = 0; i < MAX_NMEA_CLIENTS; i++) {
                    // find free/disconnected spot
                    if (!TCPClient[num][i] || !TCPClient[num][i].connected()) {
                        if (TCPClient[num][i]) TCPClient[num][i].stop();
#ifdef ESP32
                        assignAppSocket(num, i, TCPClient[num][i], server[num]->available());
                        if (TCPClient[num][i]) appKeepalive(TCPClient[num][i]);  // 0.5.17: dead apps closed on quiet ports too
                        if (TCPClient[num][i]) {
                            if (openAppQueue(num, i)) {
                                rememberPhone(TCPClient[num][i]);
                            } else {
                                LOGW(LOG_PORTS, "%s: app refused, no memory for its queue", appPortName[num]);
                                TCPClient[num][i].stop();
                            }
                        }
#else
                        TCPClient[num][i] = server[num]->available();
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
#ifdef ESP32
        dialNetTargetTick();           // increment 3: the phone to dial in network mode
        dialAppsTick(0, NUM_COM - 1);  // Project (step 4a): the bridge also dials the app
#endif
#endif

        for (int num = 0; num < NUM_COM; num++) {
#ifdef NAV_DETECTION
            if (navPort.load() == num) {
                navScan.tick(num);
                continue;
            }
#endif
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
                            ioBuf[i1[num]] =
                                TCPClient[num][cln]
                                    .read();  // read char from client
                            i1[num]++;
                            if (i1[num] == BUFFERSIZE - 1) break;
                        }

                        COM[num]->write(ioBuf,
                                        i1[num]);  // now send to UART(num):
                        STAT(num, toDevice, i1[num]);  // Project: count
                        MON(num, 1, ioBuf, i1[num]);  // Project: data monitor (step 3b)
                        i1[num] = 0;
                    }
                }
                STAT_CLIENTS(num, clients);  // Project
#ifdef ESP32
                serviceNavApps(num);  // Project (step 5): send queued data, close dead apps
#endif
#endif

                if (COM[num]->available()) {
                    int readCount = COM[num]->read(ioBuf, BUFFERSIZE); // Read up to BUFFERSIZE bytes from the serial device.
                    if (readCount == 0)
                        continue;
                    STAT(num, fromDevice, readCount);  // Project: count
                    STAT_RX(num);
                    MON(num, 0, ioBuf, readCount);  // Project: data monitor (step 3b)
#ifdef PROTOCOL_TCP
                    for (byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++) {
                        if (TCPClient[num][cln])
#ifdef ESP32
                            if (queueNavForApp(num, cln, ioBuf, readCount) > 0) STAT(num, shortWrites, 1);  // Project (step 5): skipped
#else
                            if (TCPClient[num][cln].write(ioBuf, readCount) != (size_t)readCount)
                                STAT(num, shortWrites, 1);  // Project: count short writes
#endif
                    }
#endif

#ifdef ESP32
#ifdef BLUETOOTH
                    // now send to Bluetooth:
                    if (num == BLUETOOTH && SerialBT.hasClient())
                        SerialBT.write(ioBuf, readCount);
#endif
#endif
                }
            }
        }
    }
