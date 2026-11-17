// Project: read-only status page on port 80 and /api/status as JSON
// (firmware plan step 3, R10, R14). ESP32 only.
//
// The forwarding loop only adds to atomic counters (STAT_* macros). A web
// server in its own low-priority task on core 0 reads them; the Arduino loop
// runs on core 1, so serving the page cannot hold up forwarding.
//
// Included by bridge.ino after its globals (COM, NUM_COM, udp_port) and settings.h.

#pragma once

#if defined(ESP32) && defined(STATUS_PAGE)

#include <WebServer.h>
#include <atomic>
#include <esp_timer.h>
#include <esp_wifi.h>

struct PortStats {
    std::atomic<uint32_t> fromDevice{0};   // bytes UART -> network
    std::atomic<uint32_t> toDevice{0};     // bytes network -> UART (TCP and UDP)
    std::atomic<uint32_t> lastRxMs{0};     // millis() of the last byte from the device, 0 = none yet
    std::atomic<uint32_t> tcpClients{0};   // connected TCP clients
    std::atomic<uint32_t> uartErrors{0};   // receive buffer full, FIFO overflow, framing, parity, break
    std::atomic<uint32_t> shortWrites{0};  // TCP writes that took fewer bytes than offered
};
static PortStats portStats[NUM_COM];

#define STAT(num, field, n) portStats[num].field.fetch_add((n), std::memory_order_relaxed)
#define STAT_RX(num) portStats[num].lastRxMs.store(millis() | 1u, std::memory_order_relaxed)
#define STAT_CLIENTS(num, n) portStats[num].tcpClients.store((n), std::memory_order_relaxed)

static const char* const kPortName[NUM_COM] = PORT_NAMES;

static WebServer statusServer(80);

static const char kStatusPage[] = R"HTML(<!doctype html>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Wi-Fi bridge</title>
<style>
body{font:15px system-ui,sans-serif;margin:12px;color:#222;background:#fff}
table{border-collapse:collapse;margin-top:8px}th,td{padding:4px 8px;border-bottom:1px solid #ccc;text-align:right}
th:first-child,td:first-child{text-align:left}#s{line-height:1.6}.bad{color:#b00}
</style>
<h3>Wi-Fi bridge</h3><div id="s">Loading ...</div><table id="t"></table>
<p style="color:#666">Read-only. Updates every 2 s. JSON: <a href="/api/status">/api/status</a></p>
<script>
const s=document.getElementById('s'),t=document.getElementById('t');
function age(ms){return ms===null?'never':(ms/1000).toFixed(1)+' s'}
function f(){fetch('/api/status',{cache:'no-store'}).then(r=>r.json()).then(d=>{
let w=d.wifi,sig=w.mode==='hotspot'?('phones: '+(w.stations.length?w.stations.map(x=>x+' dBm').join(', '):'none')):('bridge to router: '+w.rssi_dbm+' dBm');
s.innerHTML='Firmware '+d.firmware+'<br>Uptime '+d.uptime_s+' s<br>Wi-Fi '+w.mode+', '+w.ip+'<br>Signal '+sig;
let h='<tr><th>Device</th><th>Baud</th><th>From device</th><th>To device</th><th>Last data</th><th>Clients</th><th>UART errors</th><th>Short writes</th></tr>';
for(const p of d.ports){h+='<tr><td>'+p.name+'</td><td>'+p.baud+'</td><td>'+p.bytes_from_device+'</td><td>'+p.bytes_to_device+'</td><td>'+age(p.last_data_ms_ago)+'</td><td>'+p.tcp_clients+'</td><td'+(p.uart_errors?' class="bad"':'')+'>'+p.uart_errors+'</td><td'+(p.short_writes?' class="bad"':'')+'>'+p.short_writes+'</td></tr>'}
t.innerHTML=h}).catch(()=>{s.textContent='No connection to the bridge'})}
f();setInterval(f,2000);
</script>
)HTML";

static String statusJson() {
    char part[256];
    String out;
    out.reserve(1024);
    snprintf(part, sizeof(part), "{\"firmware\":\"Wi-Fi bridge %s\",\"uptime_s\":%llu,", VERSION,
             (unsigned long long)(esp_timer_get_time() / 1000000));
    out += part;

    if (WiFi.getMode() & WIFI_MODE_AP) {
        out += "\"wifi\":{\"mode\":\"hotspot\",\"ip\":\"" + WiFi.softAPIP().toString() + "\",\"stations\":[";
        wifi_sta_list_t list = {};
        if (esp_wifi_ap_get_sta_list(&list) == ESP_OK) {
            for (int i = 0; i < list.num; i++) {
                if (i) out += ",";
                out += String(list.sta[i].rssi);  // signal only; device addresses are not shown
            }
        }
        out += "]},";
    } else {
        snprintf(part, sizeof(part), "\"wifi\":{\"mode\":\"network\",\"ip\":\"%s\",\"rssi_dbm\":%d},",
                 WiFi.localIP().toString().c_str(), (int)WiFi.RSSI());
        out += part;
    }

    out += "\"ports\":[";
    const uint32_t now = millis();
    for (int num = 0; num < NUM_COM; num++) {
        PortStats& p = portStats[num];
        const uint32_t last = p.lastRxMs.load(std::memory_order_relaxed);
        char age[16];
        if (last) snprintf(age, sizeof(age), "%lu", (unsigned long)(now - last));
        else snprintf(age, sizeof(age), "null");
#ifdef PROTOCOL_UDP
        const unsigned udpPort = udp_port[num];
#else
        const unsigned udpPort = 0;
#endif
        snprintf(part, sizeof(part),
                 "%s{\"name\":\"%s\",\"uart\":%d,\"baud\":%lu,\"tcp_port\":%u,\"udp_port\":%u,"
                 "\"bytes_from_device\":%lu,\"bytes_to_device\":%lu,\"last_data_ms_ago\":%s,"
                 "\"tcp_clients\":%lu,\"uart_errors\":%lu,\"short_writes\":%lu}",
                 num ? "," : "", kPortName[num], num, (unsigned long)settings.port[num].baud, settings.port[num].tcpPort, udpPort,
                 (unsigned long)p.fromDevice.load(), (unsigned long)p.toDevice.load(), age,
                 (unsigned long)p.tcpClients.load(), (unsigned long)p.uartErrors.load(),
                 (unsigned long)p.shortWrites.load());
        out += part;
    }
    out += "]}";
    return out;
}

static void statusTask(void*) {
    statusServer.on("/", HTTP_GET, []() { statusServer.send(200, "text/html", kStatusPage); });
    statusServer.on("/api/status", HTTP_GET, []() {
        statusServer.sendHeader("Cache-Control", "no-store");
        statusServer.send(200, "application/json", statusJson());
    });
    statusServer.onNotFound([]() { statusServer.send(404, "text/plain", "Not found"); });
    statusServer.begin();
    for (;;) {
        statusServer.handleClient();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Before COM[num]->begin(): count UART receive errors.
static void statusAttachUart(int num) {
    COM[num]->onReceiveError([num](hardwareSerial_error_t) { STAT(num, uartErrors, 1); });
}

// At the end of setup(), once Wi-Fi is up.
static void statusBegin() {
    xTaskCreatePinnedToCore(statusTask, "status", 6144, nullptr, 1, nullptr, 0);
    MDNS.addService("http", "tcp", 80);
}

#else

#define STAT(num, field, n) ((void)0)
#define STAT_RX(num) ((void)0)
#define STAT_CLIENTS(num, n) ((void)0)
static inline void statusAttachUart(int) {}
static inline void statusBegin() {}

#endif
