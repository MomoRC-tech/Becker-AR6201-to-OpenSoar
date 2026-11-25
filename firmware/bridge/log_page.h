// Project: web log page (firmware plan step 3c, R17). ESP32 only.
//
// GET /log                  the page
// GET /api/log?since=<seq>  newer lines as JSON, oldest first
//
// weblog.h keeps the lines. logTick() runs once per second in the status task
// and turns counters into lines (apps, data arriving, receive errors, short
// writes, memory), so the forwarding loop never formats text for them.
//
// Included by status_page.h inside its ESP32 and STATUS_PAGE block.

#pragma once

#ifdef WEB_LOG

#define LOG_QUIET_MS 10000    // "no data" after this long without a byte
#define LOG_ERROR_MS 5000     // at most one receive-error or short-write line per port this often
#define LOG_MEMORY_MS 60000   // memory and Wi-Fi line (Debug)

static const char* uartErrorText(uint8_t e) {
    switch (e) {
        case UART_BREAK_ERROR: return "break";
        case UART_BUFFER_FULL_ERROR: return "receive buffer full";
        case UART_FIFO_OVF_ERROR: return "FIFO overflow";
        case UART_FRAME_ERROR: return "framing, check baud and frame";
        case UART_PARITY_ERROR: return "parity, check the frame";
        default: return "unknown";
    }
}

static void logTick() {
    static uint32_t lastTick = 0, lastMemory = 0;
    static uint32_t clients[NUM_COM], errors[NUM_COM], errorAt[NUM_COM], shorts[NUM_COM], shortAt[NUM_COM];
    static bool receiving[NUM_COM];
    const uint32_t now = millis();
    if (now - lastTick < 1000) return;
    lastTick = now;

    for (int num = 0; num < NUM_COM; num++) {
        PortStats& p = portStats[num];
        const char* name = kPortName[num];

        const uint32_t c = p.tcpClients.load(std::memory_order_relaxed);
        if (num != RADIO_COM && c != clients[num])  // the radio port logs its own owner changes
            LOGI(LOG_PORTS, "%s: %lu app%s connected (before %lu)", name, (unsigned long)c, c == 1 ? "" : "s",
                 (unsigned long)clients[num]);
        clients[num] = c;

        const uint32_t last = p.lastRxMs.load(std::memory_order_relaxed);
        const bool rx = last != 0 && (int32_t)(now - last) < LOG_QUIET_MS;
        if (rx != receiving[num]) {
            if (rx) LOGI(LOG_PORTS, "%s: data arriving", name);
            else LOGI(LOG_PORTS, "%s: no data for %d s", name, LOG_QUIET_MS / 1000);
            receiving[num] = rx;
        }

        const uint32_t e = p.uartErrors.load(std::memory_order_relaxed);
        if (e != errors[num] && (errorAt[num] == 0 || now - errorAt[num] >= LOG_ERROR_MS)) {
            LOGW(LOG_PORTS, "%s: %lu receive error%s, last: %s", name, (unsigned long)(e - errors[num]),
                 e - errors[num] == 1 ? "" : "s", uartErrorText(p.lastError.load(std::memory_order_relaxed)));
            errors[num] = e;
            errorAt[num] = now | 1u;
        }

        const uint32_t s = p.shortWrites.load(std::memory_order_relaxed);
        if (num != RADIO_COM && s != shorts[num] && (shortAt[num] == 0 || now - shortAt[num] >= LOG_ERROR_MS)) {
            LOGW(LOG_PORTS, "%s: %lu short write%s, an app is behind and skipped data", name,
                 (unsigned long)(s - shorts[num]), s - shorts[num] == 1 ? "" : "s");
            shorts[num] = s;
            shortAt[num] = now | 1u;
        }
    }

    // "Memory" counts all internal RAM, including IRAM that malloc() cannot use; "heap" is the
    // byte-addressable heap that lwIP, Wi-Fi, LittleFS and the web server allocate from:
    // "heap free/lowest/largest block kB".
#define HEAP8(fn) ((unsigned long)fn(MALLOC_CAP_8BIT) / 1024)
    if (now - lastMemory >= LOG_MEMORY_MS) {
        lastMemory = now;
        if (WiFi.getMode() & WIFI_MODE_AP)
            LOGD(LOG_SYSTEM, "Memory free %lu kB, lowest %lu kB; heap %lu/%lu/%lu kB; hotspot, %u phone(s)%s",
                 (unsigned long)ESP.getFreeHeap() / 1024, (unsigned long)ESP.getMinFreeHeap() / 1024, HEAP8(heap_caps_get_free_size),
                 HEAP8(heap_caps_get_minimum_free_size), HEAP8(heap_caps_get_largest_free_block), WiFi.softAPgetStationNum(), chipTempText());
        else
            LOGD(LOG_SYSTEM, "Memory free %lu kB, lowest %lu kB; heap %lu/%lu/%lu kB; Wi-Fi %d dBm%s",
                 (unsigned long)ESP.getFreeHeap() / 1024, (unsigned long)ESP.getMinFreeHeap() / 1024, HEAP8(heap_caps_get_free_size),
                 HEAP8(heap_caps_get_minimum_free_size), HEAP8(heap_caps_get_largest_free_block), (int)WiFi.RSSI(), chipTempText());
    }
}

// Send the lines after `since` as JSON, oldest first. A few lines at a time are
// picked under the lock, so the lock is never held while sending.
static void logSend(uint32_t since) {
    static LogLine batch[12];  // only the web task uses these
    static char buf[1400];
    uint32_t last, dropped;
    uint8_t boot;
    int64_t offset;
    portENTER_CRITICAL(&logLock);
    offset = logClockOffset;
    last = logStore.seq;
    dropped = logStore.ev.dropped > logStore.dbg.dropped ? logStore.ev.dropped : logStore.dbg.dropped;
    boot = logStore.boot;
    portEXIT_CRITICAL(&logLock);
    if (since > last) since = 0;  // the bridge lost power since the page's last request: send all
    const bool lost = since != 0 && dropped > since;

    statusServer.sendHeader("Cache-Control", "no-store");
    statusServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    statusServer.send(200, "application/json", "");
    size_t n = snprintf(buf, sizeof(buf), "{\"firmware\":\"%s\",\"boot\":%u,\"o\":%lld,\"seq\":%lu,\"now\":%lu,\"lost\":%s,\"lines\":[",
                        VERSION, boot, (long long)offset, (unsigned long)last, (unsigned long)millis(), lost ? "true" : "false");
    uint32_t cursor = since;
    bool comma = false;
    for (;;) {
        size_t count = 0;
        portENTER_CRITICAL(&logLock);
        while (count < sizeof(batch) / sizeof(batch[0])) {
            const LogLine* best = nullptr;
            for (const LogLine& l : logStore.ev.line)
                if (l.seq > cursor && l.seq <= last && (!best || l.seq < best->seq)) best = &l;
            for (const LogLine& l : logStore.dbg.line)
                if (l.seq > cursor && l.seq <= last && (!best || l.seq < best->seq)) best = &l;
            if (!best) break;
            batch[count++] = *best;
            cursor = best->seq;
        }
        portEXIT_CRITICAL(&logLock);
        for (size_t i = 0; i < count; i++) {
            const LogLine& l = batch[i];
            if (n > sizeof(buf) - (6 * LOG_TEXT + 96)) {  // room for one more line, fully escaped
                statusServer.sendContent(buf, n);
                n = 0;
            }
            n += snprintf(buf + n, sizeof(buf) - n,
                          "%s{\"q\":%lu,\"b\":%u,\"t\":%lu,\"c\":%llu,\"l\":\"%c\",\"s\":\"%s\",\"m\":\"", comma ? "," : "",
                          (unsigned long)l.seq, l.boot, (unsigned long)l.ms,
                          l.sec ? (unsigned long long)l.sec * 1000ULL + l.milli : 0ULL, l.level, kLogSource[l.src]);
            comma = true;
            for (uint8_t k = 0; k < l.len; k++) {
                const uint8_t c = (uint8_t)l.text[k];
                if (c == '"' || c == '\\') {
                    buf[n++] = '\\';
                    buf[n++] = (char)c;
                } else if (c < 0x20) {
                    n += snprintf(buf + n, sizeof(buf) - n, "\\u%04x", c);
                } else {
                    buf[n++] = (char)c;
                }
            }
            buf[n++] = '"';
            buf[n++] = '}';
        }
        if (count < sizeof(batch) / sizeof(batch[0])) break;
    }
    buf[n++] = ']';
    buf[n++] = '}';
    statusServer.sendContent(buf, n);
    statusServer.sendContent("");  // end of the chunked reply
}

static const char kLogPage[] = R"HTML(<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light dark"><title>WIFI Bridge log</title><link rel="icon" href="/device.webp">
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
.seg{display:inline-flex;border:1px solid var(--rule);border-radius:10px;overflow:hidden}
.seg button{border:0;border-radius:0;background:none;color:var(--ink);padding:8px 14px}
.seg button[aria-pressed=true],.chip[aria-pressed=true]{background:var(--green);color:var(--paper);border-color:var(--green)}
button{font:inherit;font-weight:600;border-radius:10px;padding:8px 14px;cursor:pointer;border:1px solid var(--rule);background:none;color:var(--ink)}
.chip{font-weight:400;font-size:14px;padding:5px 11px;border-radius:16px}
button:focus-visible{outline:2px solid var(--green);outline-offset:1px}
#st{color:var(--mute);font-size:14px;margin:0 0 8px}
#log{background:var(--field);border:1px solid var(--rule);border-radius:10px;padding:10px 12px;height:62vh;overflow:auto;font:13px/1.5 ui-monospace,Consolas,"Roboto Mono",monospace;white-space:pre-wrap;word-break:break-word;margin:0}
.ln{display:block;padding-left:20ch;text-indent:-20ch}
.t,.lD,.sep{color:var(--mute)}.lE{color:var(--red)}.lW{color:var(--amber)}.old{opacity:.65}
.key{color:var(--mute);font-size:13px;margin-top:8px}
</style>
<main>
<header><img src="/device.webp" width="240" height="317" alt=""><h1>Log</h1><a href="/">Status page</a></header>
<div class="bar"><span class="seg" id="views"></span></div>
<div class="bar" id="srcs"></div>
<div class="bar"><button id="pz" onclick="pause()">Pause</button><button onclick="clr()">Clear</button><button onclick="copy()">Copy</button><button onclick="save()">Save</button></div>
<p id="st">Loading the log</p>
<pre id="log" aria-live="off"></pre>
<p class="key"><b>Important</b>: errors and warnings. <b>Events</b>: adds what the bridge does. <b>All</b>: adds debug details. The bridge takes the time from the phone that opens this page; <b>+95.1s</b> means seconds after a start whose time is not known. Grey lines are from before a restart. Copy and Save take the lines shown. The bridge keeps its lines across a restart, not across a power cut; errors and warnings are also in the <a href="/saved">saved log</a>, which survives it.</p>
</main>
<script>
const $=i=>document.getElementById(i),MAX=1500,
SRC={wifi:'Wi-Fi',ports:'Ports',radio:'Radio',settings:'Settings',system:'System'},
LV={E:'Error',W:'Warning',I:'Info',D:'Debug'},
VIEWS={important:['Important','EW'],events:['Events','EWI'],all:['All','EWID']};
const kept=JSON.parse(localStorage.getItem('bridgeLog')||'{}');
let view=VIEWS[kept.view]?kept.view:'events',off=new Set(kept.off||[]),since=0,epoch=0,paused=false,lines=[],info={},note='',noteAt=0;
const esc=x=>x.replace(/[&<>]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;'})[c]);
const two=n=>String(n).padStart(2,'0');
function clock(ms){const d=new Date(ms);return two(d.getHours())+':'+two(d.getMinutes())+':'+two(d.getSeconds())+'.'+String(d.getMilliseconds()).padStart(3,'0')}
function stamp(d){return d.getFullYear()+'-'+two(d.getMonth()+1)+'-'+two(d.getDate())+' '+two(d.getHours())+':'+two(d.getMinutes())+':'+two(d.getSeconds())}
function shown(){const v=VIEWS[view][1];return lines.filter(l=>v.includes(l.l)&&!off.has(l.s))}
function remember(){localStorage.setItem('bridgeLog',JSON.stringify({view:view,off:[...off]}))}
function say(t){note=t;noteAt=Date.now();status()}
function status(){const n=shown().length;
$('st').textContent=(Date.now()-noteAt<4000?note+'. ':'')+(paused?'Paused. ':'')+(lines.length?n+' of '+lines.length+' lines shown':'No lines yet')}
function day(ms){const d=new Date(ms);return d.getFullYear()+'-'+two(d.getMonth()+1)+'-'+two(d.getDate())}
function hms(ms){const d=new Date(ms);return two(d.getHours())+':'+two(d.getMinutes())+':'+two(d.getSeconds())}
function after(t){const s=t/1000;return '+'+(s<1000?s.toFixed(1):Math.round(s))+'s'}
const sep=t=>'<span class="ln sep">── '+t+' ──</span>';
function draw(){const L=$('log'),end=L.scrollTop+L.clientHeight>=L.scrollHeight-20;let h=[],prev='',pday='';
for(const l of shown()){const k=l.e+'/'+l.b;if(prev&&k!==prev)h.push(sep('bridge restarted'));prev=k;
if(l.c&&day(l.c)!==pday){pday=day(l.c);h.push(sep(pday))}
h.push('<span class="ln'+(l.e===epoch&&l.b===info.boot?'':' old')+'"><span class="t" title="'+(l.t/1000).toFixed(3)+' s after the start">'+(l.c?hms(l.c):after(l.t).padStart(8))+'</span> <span class="l'+l.l+'">'+l.l+' '+SRC[l.s].padEnd(8)+' '+esc(l.m)+'</span></span>')}
L.innerHTML=h.join('');if(end)L.scrollTop=L.scrollHeight;status()}
function poll(){fetch('/api/log?since='+since+'&clock='+Date.now(),{cache:'no-store'}).then(r=>r.json()).then(d=>{
if(d.seq<since)epoch++;
info=d;
if(d.o)for(const l of lines)if(l.e===epoch&&l.b===d.boot)l.c=l.t+d.o;
if(d.lost)lines.push({e:epoch,b:d.boot,t:d.now,c:Date.now(),l:'W',s:'system',m:'Some lines were overwritten before this page fetched them'});
for(const l of d.lines)lines.push({e:epoch,b:l.b,t:l.t,c:l.c,l:l.l,s:l.s,m:l.m});
if(lines.length>MAX)lines.splice(0,lines.length-MAX);
since=d.seq;if(!paused)draw();else status()}).catch(()=>{$('st').textContent='The bridge is not answering.'})}
function ui(){$('views').innerHTML=Object.keys(VIEWS).map(k=>'<button aria-pressed="'+(k===view)+'" onclick="setView(\''+k+'\')">'+VIEWS[k][0]+'</button>').join('');
$('srcs').innerHTML=Object.keys(SRC).map(k=>'<button class="chip" aria-pressed="'+!off.has(k)+'" onclick="toggle(\''+k+'\')">'+SRC[k]+'</button>').join('');
$('pz').textContent=paused?'Resume':'Pause'}
function setView(k){view=k;remember();ui();draw()}
function toggle(k){off.has(k)?off.delete(k):off.add(k);remember();ui();draw()}
function pause(){paused=!paused;ui();draw()}
function clr(){lines=[];draw()}
function text(){const s=shown();return['# WIFI Bridge log, firmware '+(info.firmware||'?')+', saved '+stamp(new Date())+' (phone clock)',
'# View: '+VIEWS[view][0]+'; sources: '+Object.keys(SRC).filter(k=>!off.has(k)).map(k=>SRC[k]).join(', '),
'# Columns: date and time (time zone of this phone; - not known), seconds since start, start number, level, source, message']
.concat(s.map(l=>[l.c?day(l.c)+' '+clock(l.c):'-',(l.t/1000).toFixed(3),l.b,LV[l.l],SRC[l.s],l.m].join('\t'))).join('\n')+'\n'}
function legacyCopy(t){const a=document.createElement('textarea');a.value=t;a.setAttribute('readonly','');a.style.position='fixed';a.style.top='0';a.style.opacity='0';
document.body.appendChild(a);a.select();a.setSelectionRange(0,t.length);let ok=false;try{ok=document.execCommand('copy')}catch(e){}a.remove();return ok}
function copy(){const t=text(),n=shown().length,done=()=>say('Copied '+n+' lines'),fail=()=>say('The browser blocked copying; use Save');
if(navigator.clipboard&&window.isSecureContext)navigator.clipboard.writeText(t).then(done,()=>legacyCopy(t)?done():fail());
else legacyCopy(t)?done():fail()}
function save(){const d=new Date(),a=document.createElement('a');a.href=URL.createObjectURL(new Blob([text()],{type:'text/plain'}));
a.download='bridge-log-'+stamp(d).replace(/[: ]/g,'-')+'.txt';a.click();setTimeout(()=>URL.revokeObjectURL(a.href),5000);say('Saved '+shown().length+' lines')}
ui();poll();setInterval(poll,1000);
</script>
)HTML";

static void logRoutes() {
    statusServer.on("/log", HTTP_GET, []() { statusServer.send_P(200, "text/html", kLogPage); });
    statusServer.on("/api/log", HTTP_GET, []() {
        if (statusServer.hasArg("clock")) logSetClock(strtoull(statusServer.arg("clock").c_str(), nullptr, 10));
        logSend(statusServer.hasArg("since") ? strtoul(statusServer.arg("since").c_str(), nullptr, 10) : 0);
    });
}

#else

static inline void logRoutes() {}
static inline void logTick() {}

#endif
