// Project: saved log (firmware plan step 3g). ESP32 only.
//
// Errors, warnings and one line per start, kept through power cuts in a file on
// the flash file system (LittleFS on the 128 kB "spiffs" partition). The web log
// keeps everything in RAM until power off; this file keeps what matters after
// switch-off. At most every 10 s the status task copies new Error and Warning
// lines from the web log and appends them in one write. Lines written just
// before a crash or restart are still in RAM at the next start and are saved then.
//
// File /saved.log, one line each, tab-separated: start number (lifetime record),
// clock time in Unix ms (0 = not known), ms since that start, level, source,
// message. Above SAVED_MAX_BYTES it becomes /saved.old and the older one is
// deleted, so the two files stay below about 64 kB.
//
// Flash writes pause code from flash on both cores (S133): only the status task
// writes, and at most once every 10 s.
//
//   GET  /saved              the page
//   GET  /api/saved          both files as text, oldest line first
//   POST /api/saved/clear    delete both (settings_page.h)
//
// Included by status_page.h after lifetime.h, before settings_page.h.

#pragma once

#ifdef SAVED_LOG

#include <LittleFS.h>
#include <esp_attr.h>
#include <esp_log.h>
#include <esp_system.h>

#define SAVED_FILE "/saved.log"
#define SAVED_OLD "/saved.old"
#define SAVED_MAX_BYTES 32768
#define SAVED_EVERY_MS 10000UL
#define SAVED_MAGIC 0x53564C31u  // "SVL1"

struct SavedMark {  // .noinit: what was saved, kept through a restart (not a power cut)
    uint32_t magic;
    uint32_t seq;  // the last web log line looked at
};
static __NOINIT_ATTR SavedMark savedMark;
static bool savedReady = false;    // file system mounted
static bool savedStarted = false;  // this start's line written

static uint32_t savedStartOf(uint8_t boot) {  // lifetime start number of a web log start
    const uint8_t back = (uint8_t)(logStore.boot - boot);
    return life.starts > back ? life.starts - back : 0;
}

static void savedRotate() {
    File f = LittleFS.open(SAVED_FILE, "r");
    const size_t size = f ? f.size() : 0;
    if (f) f.close();
    if (size < SAVED_MAX_BYTES) return;
    LittleFS.remove(SAVED_OLD);
    LittleFS.rename(SAVED_FILE, SAVED_OLD);
}

static void savedPutLine(File& f, uint32_t start, uint64_t unixMs, uint32_t ms, char level, uint8_t src, const char* text,
                         size_t len) {
    char head[64];
    const int n = snprintf(head, sizeof(head), "%lu\t%llu\t%lu\t%c\t%s\t", (unsigned long)start, (unsigned long long)unixMs,
                           (unsigned long)ms, level, kLogSource[src]);
    f.write((const uint8_t*)head, n);
    for (size_t i = 0; i < len; i++) {
        const char c = text[i];
        f.write((uint8_t)(c == '\t' || c == '\n' || c == '\r' ? ' ' : c));
    }
    f.write((uint8_t)'\n');
}

// Append the Error and Warning lines after savedMark.seq. oldOnly: only lines of
// earlier starts, before this start's own line is written.
static void savedFlush(bool oldOnly) {
    static LogLine batch[12];  // only the status task uses these
    uint32_t last, dropped;
    uint8_t boot;
    portENTER_CRITICAL(&logLock);
    last = logStore.seq;
    dropped = logStore.ev.dropped;
    boot = logStore.boot;
    portEXIT_CRITICAL(&logLock);
    if (savedMark.seq > last) savedMark.seq = 0;
    if (savedMark.seq == last) return;

    File f;
    uint32_t cursor = savedMark.seq;
    if (dropped > cursor) {  // lines were overwritten in RAM before this could save them
        savedRotate();
        f = LittleFS.open(SAVED_FILE, "a");
        if (!f) return;
        const char* t = "Some lines were overwritten before they were saved";
        savedPutLine(f, life.starts, 0, millis(), 'W', LOG_SYSTEM, t, strlen(t));
    }
    for (;;) {
        size_t count = 0;
        portENTER_CRITICAL(&logLock);
        while (count < sizeof(batch) / sizeof(batch[0])) {
            const LogLine* best = nullptr;
            for (const LogLine& l : logStore.ev.line)
                if (l.seq > cursor && l.seq <= last && (!best || l.seq < best->seq)) best = &l;
            if (!best || (oldOnly && best->boot == boot)) break;
            batch[count++] = *best;
            cursor = best->seq;
        }
        portEXIT_CRITICAL(&logLock);
        for (size_t i = 0; i < count; i++) {
            const LogLine& l = batch[i];
            if (l.level != 'E' && l.level != 'W') continue;
            if (!f) {
                savedRotate();
                f = LittleFS.open(SAVED_FILE, "a");
                if (!f) return;  // try again next time
            }
            const uint64_t unixMs = l.sec ? (uint64_t)l.sec * 1000ULL + l.milli : 0;
            savedPutLine(f, savedStartOf(l.boot), unixMs, l.ms, l.level, l.src, l.text, l.len);
        }
        if (count < sizeof(batch) / sizeof(batch[0])) break;
    }
    if (f) f.close();
    savedMark.seq = cursor;
}

// Once, in the status task after lifetimeBegin(): mount, save what the last start
// left in RAM, then this start's line.
static void savedLogBegin() {
    if (esp_reset_reason() == ESP_RST_POWERON || savedMark.magic != SAVED_MAGIC) {
        savedMark.magic = SAVED_MAGIC;
        savedMark.seq = 0;
    }
    esp_log_level_set("esp_littlefs", ESP_LOG_NONE);  // an unformatted partition is expected on first use
    bool ok = LittleFS.begin(false, "/littlefs", 4, "spiffs");
    esp_log_level_set("esp_littlefs", ESP_LOG_ERROR);
    if (!ok) {
        LOGI(LOG_SYSTEM, "Saved log: formatting the file system (first use)");
        ok = LittleFS.format() && LittleFS.begin(false, "/littlefs", 4, "spiffs");
    }
    if (!ok) {
        LOGW(LOG_SYSTEM, "Saved log: file system not available, nothing is kept through a power cut");
        return;
    }
    savedReady = true;
    savedFlush(true);
    const esp_reset_reason_t r = esp_reset_reason();
    const bool bad = r == ESP_RST_PANIC || r == ESP_RST_INT_WDT || r == ESP_RST_TASK_WDT || r == ESP_RST_WDT ||
                     r == ESP_RST_BROWNOUT;
    char t[LOG_TEXT];
    const int n = snprintf(t, sizeof(t), "Start, firmware %s, reset: %s", VERSION, resetText(r));
    savedRotate();
    File f = LittleFS.open(SAVED_FILE, "a");
    if (f) {
        const time_t now = time(nullptr);
        const uint64_t unixMs = now >= (time_t)LOG_CLOCK_VALID ? (uint64_t)now * 1000ULL : 0;
        savedPutLine(f, life.starts, unixMs, millis(), bad ? 'W' : 'I', LOG_SYSTEM, t, n);
        f.close();
    }
    savedStarted = true;
}

// 0.5.10: every append goes through here, so flash writes (which stall the UART receive
// interrupt, S133) stay at most one per SAVED_EVERY_MS, whatever a client requests.
static void savedFlushIfDue() {
    static uint32_t at = 0;
    if (millis() - at < SAVED_EVERY_MS) return;
    at = millis();
    savedFlush(false);
}

// Called from the status task loop every few milliseconds.
static void savedLogTick() {
    if (!savedReady) return;
    savedFlushIfDue();
}

static void savedClear() {
    if (!savedReady) return;
    LittleFS.remove(SAVED_OLD);
    LittleFS.remove(SAVED_FILE);
    LOGI(LOG_SETTINGS, "Saved log cleared");
}

// Part of /api/status.
static void savedJson(String& out) { out += savedReady ? ",\"saved_log\":{\"ready\":true}" : ",\"saved_log\":{\"ready\":false}"; }

static void savedSendFile(const char* path, char* buf, size_t cap) {
    File f = LittleFS.open(path, "r");
    if (!f) return;
    size_t n;
    while ((n = f.read((uint8_t*)buf, cap)) > 0) statusServer.sendContent(buf, n);
    f.close();
}

// /api/saved: "# used <bytes> of <bytes>" first, then the lines, oldest first.
static void savedSend() {
    statusServer.sendHeader("Cache-Control", "no-store");
    if (!savedReady) {
        statusServer.send(503, "text/plain", "# not available\n");
        return;
    }
    savedFlushIfDue();  // lines from the last few seconds may follow on the next request
    static char buf[1024];
    statusServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    statusServer.send(200, "text/plain; charset=utf-8", "");
    const int n = snprintf(buf, sizeof(buf), "# used %u of %u bytes\n", (unsigned)LittleFS.usedBytes(),
                           (unsigned)LittleFS.totalBytes());
    statusServer.sendContent(buf, n);
    savedSendFile(SAVED_OLD, buf, sizeof(buf));
    savedSendFile(SAVED_FILE, buf, sizeof(buf));
    statusServer.sendContent("");
}

static const char kSavedPage[] = R"HTML(<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light dark"><title>WIFI Bridge saved log</title><link rel="icon" href="/device.webp">
<style>
:root{--ink:#262a27;--paper:#f4f6f3;--rule:#d5ddd6;--mute:#5d665f;--amber:#b26b00;--red:#b3261e;--green:#2f6b3b;--field:#fff}
@media (prefers-color-scheme:dark){:root{--paper:#141a16;--ink:#e6ebe6;--rule:#2c3630;--mute:#9aa59c;--amber:#f0a73a;--red:#ff8a80;--green:#7cc487;--field:#1c241f}}
*{box-sizing:border-box}
body{margin:0;background:var(--paper);color:var(--ink);font:16px/1.45 system-ui,"Segoe UI",Roboto,sans-serif}
main{max-width:900px;margin:0 auto;padding:16px 16px 40px}
header{display:flex;gap:14px;align-items:center;padding-bottom:12px}
header img{width:52px;height:auto;border-radius:5px}
h1{margin:0;font:800 26px/1.1 Arial,Helvetica,sans-serif;color:var(--green)}
header a{margin-left:auto;font-size:15px}a{color:inherit}
.bar{display:flex;flex-wrap:wrap;gap:8px;align-items:center;margin-bottom:10px}
button{font:inherit;font-weight:600;border-radius:10px;padding:8px 14px;cursor:pointer;border:1px solid var(--rule);background:none;color:var(--ink)}
button:focus-visible{outline:2px solid var(--green);outline-offset:1px}
#st{color:var(--mute);font-size:14px;margin:0 0 8px}
#log{background:var(--field);border:1px solid var(--rule);border-radius:10px;padding:10px 12px;height:62vh;overflow:auto;font:13px/1.5 ui-monospace,Consolas,"Roboto Mono",monospace;white-space:pre-wrap;word-break:break-word;margin:0}
.ln{display:block;padding-left:20ch;text-indent:-20ch}
.t,.sep{color:var(--mute)}.lE{color:var(--red)}.lW{color:var(--amber)}.st{font-weight:700}
.key{color:var(--mute);font-size:13px;margin-top:8px}
</style>
<main>
<header><img src="/device.webp" width="240" height="317" alt=""><h1>Saved log</h1><a href="/log">Log</a><a href="/">Status page</a></header>
<div class="bar"><button onclick="load()">Reload</button><button onclick="save()">Save</button></div>
<p id="st">Loading the saved log</p>
<pre id="log" aria-live="off"></pre>
<p class="key">Errors, warnings and each start, kept through power cuts. Start numbers count from the last reset of the lifetime record. <b>+95.1s</b> means seconds after a start whose time was not known when the line was saved. New lines are saved within 10 seconds. Clear it on the <a href="/settings">settings page</a>.</p>
</main>
<script>
const $=i=>document.getElementById(i),SRC={wifi:'Wi-Fi',ports:'Ports',radio:'Radio',settings:'Settings',system:'System'},LV={E:'Error',W:'Warning',I:'Info'};
const two=n=>String(n).padStart(2,'0'),esc=x=>x.replace(/[&<>]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;'})[c]);
function day(ms){const d=new Date(ms);return d.getFullYear()+'-'+two(d.getMonth()+1)+'-'+two(d.getDate())}
function hms(ms){const d=new Date(ms);return two(d.getHours())+':'+two(d.getMinutes())+':'+two(d.getSeconds())}
function after(t){const s=t/1000;return '+'+(s<1000?s.toFixed(1):Math.round(s))+'s'}
let raw='',lines=[];
function parse(t){raw=t;lines=[];let used='';for(const r of t.split('\n')){if(!r)continue;if(r[0]==='#'){used=r.slice(2);continue}
const f=r.split('\t');if(f.length<6)continue;lines.push({n:+f[0],c:+f[1],t:+f[2],l:f[3],s:f[4],m:f.slice(5).join(' ')})}return used}
function draw(used){const L=$('log');let h=[],pday='',pn=-1;
for(const l of lines){const start=/^Start, firmware/.test(l.m);if(start||l.n!==pn){h.push('<span class="ln sep">── Start '+l.n+' ──</span>');pn=l.n}
if(l.c&&day(l.c)!==pday){pday=day(l.c);h.push('<span class="ln sep">── '+pday+' ──</span>')}
h.push('<span class="ln"><span class="t">'+(l.c?hms(l.c):after(l.t).padStart(8))+'</span> <span class="l'+l.l+(start?' st':'')+'">'+l.l+' '+(SRC[l.s]||l.s).padEnd(8)+' '+esc(l.m)+'</span></span>')}
L.innerHTML=h.join('');L.scrollTop=L.scrollHeight;
const w=lines.filter(l=>l.l!=='I').length;$('st').textContent=(lines.length?lines.length+' lines, '+w+' errors or warnings':'No lines saved yet')+(used?'. Storage: '+used:'')}
function load(){fetch('/api/saved',{cache:'no-store'}).then(r=>r.ok?r.text():Promise.reject()).then(t=>draw(parse(t))).catch(()=>{$('st').textContent='The saved log is not available.'})}
function save(){const d=new Date(),a=document.createElement('a'),s=day(d.getTime())+'-'+hms(d.getTime()).replace(/:/g,'-');
const t=['# WIFI Bridge saved log, saved '+day(d.getTime())+' '+hms(d.getTime())+' (phone clock)','# Columns: date and time (- not known), seconds since start, start number, level, source, message']
.concat(lines.map(l=>[l.c?day(l.c)+' '+hms(l.c):'-',(l.t/1000).toFixed(3),l.n,LV[l.l]||l.l,SRC[l.s]||l.s,l.m].join('\t'))).join('\n')+'\n';
a.href=URL.createObjectURL(new Blob([t],{type:'text/plain'}));a.download='bridge-saved-log-'+s+'.txt';a.click();setTimeout(()=>URL.revokeObjectURL(a.href),5000)}
load();
</script>
)HTML";

static void savedRoutes() {
    statusServer.on("/saved", HTTP_GET, []() { statusServer.send(200, "text/html", kSavedPage); });
    statusServer.on("/api/saved", HTTP_GET, savedSend);
}

#else

static inline void savedLogBegin() {}
static inline void savedLogTick() {}
static inline void savedRoutes() {}
static inline void savedJson(String&) {}

#endif
