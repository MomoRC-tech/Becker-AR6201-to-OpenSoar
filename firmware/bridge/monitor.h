// Project: data monitor (firmware plan step 3b, R16). ESP32 only.
//
// Keeps the latest chunks of data per device, both directions in time order,
// while someone watches /monitor. The forwarding loop (and the UDP task) only
// copy bytes into a fixed slot under a short spinlock; the web task reads them.
// Nothing is collected when no monitor page has asked for data for 10 s.
//
// Included by status_page.h inside its ESP32 and STATUS_PAGE block.

#pragma once

#define MON_SLOTS 64       // chunks kept per device
#define MON_BYTES 64       // bytes per chunk; longer reads take several chunks
#define MON_IDLE_MS 10000  // stop collecting this long after the last poll

struct MonSlot {
    uint32_t seq;  // 1, 2, 3 ... per device; 0 = empty
    uint32_t ms;   // millis() when copied
    uint8_t dir;   // 0 = from device, 1 = to device
    uint8_t len;
    uint8_t data[MON_BYTES];
};

static MonSlot monSlots[NUM_COM][MON_SLOTS];
static uint32_t monSeq[NUM_COM];
static volatile uint32_t monLastPoll = 0;  // millis() of the last /api/monitor request, 0 = never
static portMUX_TYPE monLock = portMUX_INITIALIZER_UNLOCKED;

static inline bool monWatching() {
    const uint32_t last = monLastPoll;
    return last != 0 && millis() - last < MON_IDLE_MS;
}

static void monAdd(int num, uint8_t dir, const uint8_t* data, size_t len) {
    if (len == 0 || !monWatching()) return;
    const uint32_t now = millis();
    while (len) {
        const uint8_t n = len > MON_BYTES ? MON_BYTES : (uint8_t)len;
        portENTER_CRITICAL(&monLock);
        const uint32_t seq = ++monSeq[num];
        MonSlot& s = monSlots[num][seq % MON_SLOTS];
        s.seq = seq;
        s.ms = now;
        s.dir = dir;
        s.len = n;
        memcpy(s.data, data, n);
        portEXIT_CRITICAL(&monLock);
        data += n;
        len -= n;
    }
}

#define MON(num, dir, data, len) monAdd((num), (dir), (const uint8_t*)(data), (len))

// Send the chunks after `since` as JSON, oldest first, streamed in pieces of
// about 1 kB from a fixed buffer; "lost" when some were overwritten.
static void monitorSend(int num, uint32_t since) {
    static MonSlot copy[MON_SLOTS];  // only the web task uses this
    static char buf[1200];
    uint32_t last;
    portENTER_CRITICAL(&monLock);
    memcpy(copy, monSlots[num], sizeof(copy));
    last = monSeq[num];
    portEXIT_CRITICAL(&monLock);
    monLastPoll = millis() | 1u;

    uint32_t first = since + 1;
    bool lost = false;
    if (last >= MON_SLOTS && first < last - MON_SLOTS + 1) {
        lost = since != 0;
        first = last - MON_SLOTS + 1;
    }
    statusServer.sendHeader("Cache-Control", "no-store");
    statusServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    statusServer.send(200, "application/json", "");
    size_t n = snprintf(buf, sizeof(buf), "{\"seq\":%lu,\"lost\":%s,\"now\":%lu,\"chunks\":[", (unsigned long)last,
                        lost ? "true" : "false", (unsigned long)millis());
    static const char hex[] = "0123456789abcdef";
    bool comma = false;
    for (uint32_t q = first; q <= last && last != 0; q++) {
        const MonSlot& s = copy[q % MON_SLOTS];
        if (s.seq != q) continue;
        if (n > sizeof(buf) - (2 * MON_BYTES + 48)) {  // room for one more chunk
            statusServer.sendContent(buf, n);
            n = 0;
        }
        n += snprintf(buf + n, sizeof(buf) - n, "%s{\"t\":%lu,\"d\":%u,\"x\":\"", comma ? "," : "", (unsigned long)s.ms, s.dir);
        comma = true;
        for (uint8_t i = 0; i < s.len; i++) {
            buf[n++] = hex[s.data[i] >> 4];
            buf[n++] = hex[s.data[i] & 15];
        }
        buf[n++] = '"';
        buf[n++] = '}';
    }
    buf[n++] = ']';
    buf[n++] = '}';
    statusServer.sendContent(buf, n);
    statusServer.sendContent("");  // end of the chunked reply
}

static const char kMonitorPage[] = R"HTML(<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light dark"><title>WIFI Bridge data</title><link rel="icon" href="/device.webp">
<style>
:root{--ink:#262a27;--paper:#f4f6f3;--rule:#d5ddd6;--mute:#5d665f;--amber:#b26b00;--green:#2f6b3b;--from:#2e8b47;--field:#fff}
@media (prefers-color-scheme:dark){:root{--paper:#141a16;--ink:#e6ebe6;--rule:#2c3630;--mute:#9aa59c;--amber:#f0a73a;--green:#7cc487;--from:#5fd17e;--field:#1c241f}}
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
.seg button[aria-pressed=true]{background:var(--green);color:var(--paper)}
button{font:inherit;font-weight:600;border-radius:10px;padding:8px 14px;cursor:pointer;border:1px solid var(--rule);background:none;color:var(--ink)}
button:focus-visible{outline:2px solid var(--green);outline-offset:1px}
#st{color:var(--mute);font-size:14px;margin:0 0 8px}
#log{background:var(--field);border:1px solid var(--rule);border-radius:10px;padding:10px 12px;height:65vh;overflow:auto;font:13px/1.5 ui-monospace,Consolas,"Roboto Mono",monospace;white-space:pre-wrap;word-break:break-all;margin:0}
.f{color:var(--from)}.o{color:var(--amber)}.t{color:var(--mute)}
.key{color:var(--mute);font-size:13px;margin-top:8px}
</style>
<main>
<header><img src="/device.webp" width="240" height="317" alt=""><h1>Data</h1><a href="/">Status page</a></header>
<div class="bar"><span class="seg" id="ports"></span><span class="seg"><button id="mt" onclick="setMode('t')">Text</button><button id="mh" onclick="setMode('h')">Hex</button></span>
<button id="pz" onclick="pause()">Pause</button><button onclick="clr()">Clear</button><button onclick="dl()">Download</button></div>
<p id="st">Waiting for data</p>
<pre id="log" aria-live="off"></pre>
<p class="key"><span class="f">from</span>: sent by the device to the apps. <span class="o">to</span>: sent by an app to the device. Times are seconds since the bridge started. The bridge keeps the data only while this page is open.</p>
</main>
<script>
const $=i=>document.getElementById(i),NAMES=['Zander','FLARM','Becker'],MAX=600;
let port=+(new URLSearchParams(location.search).get('port')||0),mode=port===2?'h':'t',since=0,paused=false,lines=[],part=['',''],lastDir=-1;
const esc=x=>x.replace(/[&<>]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;'})[c]);
function bytes(x){const b=[];for(let i=0;i<x.length;i+=2)b.push(parseInt(x.substr(i,2),16));return b}
function txt(b){return b.map(c=>c===13?'':c===10?'\n':c>=32&&c<127?String.fromCharCode(c):'\u00b7').join('')}
function hex(b){return b.map(c=>c.toString(16).padStart(2,'0').toUpperCase()).join(' ')}
function push(t,d,s){lines.push({t:t,d:d,s:s});if(lines.length>MAX)lines.splice(0,lines.length-MAX)}
function add(c){const b=bytes(c.x),t=(c.t/1000).toFixed(3);
if(mode==='h'){push(t,c.d,hex(b));return}
let s=part[c.d]+txt(b),k;while((k=s.indexOf('\n'))>=0){push(t,c.d,s.slice(0,k));s=s.slice(k+1)}
if(s.length>200){push(t,c.d,s);s=''}part[c.d]=s}
function draw(){const L=$('log'),end=L.scrollTop+L.clientHeight>=L.scrollHeight-20;
L.innerHTML=lines.map(l=>'<span class="t">'+l.t+'</span> <span class="'+(l.d?'o">to  ':'f">from')+'</span> '+esc(l.s)).join('\n');if(end)L.scrollTop=L.scrollHeight}
function poll(){fetch('/api/monitor?port='+port+'&since='+since,{cache:'no-store'}).then(r=>r.json()).then(d=>{
if(d.lost&&since)push((d.now/1000).toFixed(3),0,'... some data was not shown (too much at once)');
since=d.seq;if(!paused){d.chunks.forEach(add);draw()}
$('st').textContent=(paused?'Paused. ':'')+(lines.length?lines.length+' lines':'No data yet from '+NAMES[port]+' in this view')}).catch(()=>{$('st').textContent='The bridge is not answering.'})}
function ui(){$('ports').innerHTML=NAMES.map((n,i)=>'<button aria-pressed="'+(i===port)+'" onclick="setPort('+i+')">'+n+'</button>').join('');
$('mt').setAttribute('aria-pressed',mode==='t');$('mh').setAttribute('aria-pressed',mode==='h');$('pz').textContent=paused?'Resume':'Pause'}
function setPort(i){port=i;mode=i===2?'h':'t';since=0;clr();ui();history.replaceState(null,'','?port='+i)}
function setMode(m){mode=m;clr();ui()}
function pause(){paused=!paused;ui()}
function clr(){lines=[];part=['',''];draw()}
function dl(){const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([lines.map(l=>l.t+'\t'+(l.d?'to':'from')+'\t'+l.s).join('\n')+'\n'],{type:'text/plain'}));
a.download='bridge-'+NAMES[port].toLowerCase()+'-'+new Date().toISOString().slice(0,19).replace(/[:T]/g,'-')+'.txt';a.click()}
ui();poll();setInterval(poll,1000);
</script>
)HTML";

static void monitorRoutes() {
    statusServer.on("/monitor", HTTP_GET, []() {
        monLastPoll = millis() | 1u;  // start collecting before the first poll
        statusServer.send(200, "text/html", kMonitorPage);
    });
    statusServer.on("/api/monitor", HTTP_GET, []() {
        const int num = statusServer.hasArg("port") ? statusServer.arg("port").toInt() : 0;
        if (num < 0 || num >= NUM_COM) {
            statusServer.send(400, "application/json", "{\"error\":\"port 0 to 2\"}");
            return;
        }
        const uint32_t since = statusServer.hasArg("since") ? strtoul(statusServer.arg("since").c_str(), nullptr, 10) : 0;
        monitorSend(num, since);
    });
}
