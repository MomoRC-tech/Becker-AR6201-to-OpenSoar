// Project: read-only status page on port 80 and /api/status as JSON
// (firmware plan step 3, R10, R14). ESP32 only.
//
// The forwarding loop only adds to atomic counters (STAT_* macros). A web
// server in its own low-priority task on core 0 reads them; the Arduino loop
// runs on core 1, so serving the page cannot hold up forwarding.
//
// Included by bridge.ino after its globals (COM, NUM_COM) and settings.h.

#pragma once

#if defined(ESP32) && defined(STATUS_PAGE)

#include <WebServer.h>
#include <atomic>
#include <esp_timer.h>
#include <esp_wifi.h>

#include "device_image.h"  // lid photo for the page header

struct PortStats {
    std::atomic<uint32_t> fromDevice{0};   // bytes UART -> network
    std::atomic<uint32_t> toDevice{0};     // bytes network -> UART
    std::atomic<uint32_t> lastRxMs{0};     // millis() of the last byte from the device, 0 = none yet
    std::atomic<uint32_t> tcpClients{0};   // connected TCP clients
    std::atomic<uint32_t> uartErrors{0};   // receive buffer full, FIFO overflow, framing, parity, break
    std::atomic<uint32_t> shortWrites{0};  // TCP writes that took fewer bytes than offered
    std::atomic<uint8_t> lastError{0};     // hardwareSerial_error_t of the last receive error (web log)
};
static PortStats portStats[NUM_COM];

#define STAT(num, field, n) portStats[num].field.fetch_add((n), std::memory_order_relaxed)
#define STAT_RX(num) portStats[num].lastRxMs.store(millis() | 1u, std::memory_order_relaxed)
#define STAT_CLIENTS(num, n) portStats[num].tcpClients.store((n), std::memory_order_relaxed)

static const char* const kPortName[NUM_COM] = PORT_NAMES;

static WebServer statusServer(80);

#include "lifetime.h"  // Project: lifetime record in NVS (step 3f)
#include "supply.h"  // Project: supply voltage, OpenVario port and chart (step 3d)
#include "chiptemp.h"  // Project: ESP32 chip temperature (step 3e)
#include "savedlog.h"  // Project: errors and warnings kept through power cuts (step 3g)

// Status page; /device.webp is the housing lid photo from device_image.h.
static const char kStatusPage[] = R"HTML(<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light dark"><title>WIFI Bridge</title><link rel="icon" href="/device.webp">
<style>
:root{--ink:#262a27;--paper:#f4f6f3;--rule:#d5ddd6;--mute:#5d665f;--amber:#b26b00;--red:#b3261e;--live:#2e8b47;--green:#2f6b3b}
@media (prefers-color-scheme:dark){:root{--paper:#141a16;--ink:#e6ebe6;--rule:#2c3630;--mute:#9aa59c;--amber:#f0a73a;--red:#ff8a80;--live:#5fd17e;--green:#7cc487}}
*{box-sizing:border-box}
body{margin:0;background:var(--paper);color:var(--ink);font:16px/1.45 system-ui,"Segoe UI",Roboto,sans-serif}
main{max-width:640px;margin:0 auto;padding:16px 16px 40px}
header{display:flex;gap:18px;align-items:center;padding-bottom:16px}
header img{width:104px;height:auto;border-radius:9px;flex:none;display:block}
h1{margin:0 0 6px;font:800 28px/1.1 Arial,Helvetica,sans-serif;color:var(--green)}
.meta{color:var(--mute);font-size:15px}.meta b{color:var(--ink);font-weight:600;white-space:nowrap}
.meta div{margin-top:2px}
.down{margin:0 0 12px;padding:10px 14px;border-left:4px solid var(--red);color:var(--red)}
.trial{margin:0 0 12px;padding:10px 14px;border-left:4px solid var(--amber)}
section{border-top:1px solid var(--rule);padding:14px 0}
.row{display:flex;align-items:center;gap:10px}
h2{margin:0;font-size:21px;font-weight:700;flex:1}
.dot{width:11px;height:11px;border-radius:50%;background:var(--rule);flex:none}
.on .dot{background:var(--live)}
.pulse .dot{animation:p .9s ease-out}
@keyframes p{0%{box-shadow:0 0 0 0 var(--live)}100%{box-shadow:0 0 0 9px transparent}}
@media (prefers-reduced-motion:reduce){.pulse .dot{animation:none}}
.st{color:var(--mute);font-size:15px}.on .st{color:var(--ink)}
.brief{margin:4px 0 0 21px;font-variant-numeric:tabular-nums}
.bad{color:var(--red)}.chg{color:var(--amber)}
details{margin:6px 0 0 21px}
summary{color:var(--mute);font-size:14px;cursor:pointer;width:max-content}
summary:focus-visible{outline:2px solid var(--live);outline-offset:3px;border-radius:3px}
dl{display:grid;grid-template-columns:repeat(auto-fill,minmax(100px,1fr));gap:8px 16px;margin:8px 0 0}
dt{color:var(--mute);font-size:13px}dd{margin:0;font-variant-numeric:tabular-nums;font-weight:600}
.set details{margin-left:0}.set summary{font-size:16px;color:var(--ink);font-weight:700}
table{width:100%;border-collapse:collapse;margin-top:10px;font-variant-numeric:tabular-nums}
th{color:var(--mute);font-size:13px;font-weight:400;text-align:left;padding:4px 8px 4px 0}
td{padding:6px 8px 6px 0;border-top:1px solid var(--rule)}
.grp{display:grid;grid-template-columns:minmax(120px,190px) 1fr;gap:6px 16px;margin:16px 0 0}
.grp span{color:var(--mute)}
.key{color:var(--mute);font-size:13px;margin:12px 0 0}
footer{color:var(--mute);font-size:13px;border-top:1px solid var(--rule);padding-top:12px}
#sc{display:block;width:100%;height:auto;margin:8px 0 0}
#sc text{fill:var(--mute);font-size:19px;font-family:system-ui,sans-serif}
.bd{fill:var(--live);opacity:.25}.mn{fill:none;stroke:var(--green);stroke-width:2.5;stroke-linejoin:round}.gr{stroke:var(--rule);stroke-width:1}.lw{stroke:var(--red);stroke-width:1.5;stroke-dasharray:6 5}
a{color:inherit}
</style>
<main>
<header><img src="/device.webp" width="240" height="316" alt="WIFI Bridge housing with the MomoRC logo"><div><h1>WIFI Bridge</h1><div class="meta" id="meta">Connecting to the bridge</div></div></header>
<div class="trial" id="trial" hidden><span id="tt"></span> <a href="/settings">Keep or change</a></div>
<div class="down" id="down" hidden>The bridge is not answering. Check that this phone is on the bridge's Wi-Fi.</div>
<div id="ports"></div>
<section class="set" id="bootTemps" hidden><details><summary>Chip temperature during startup</summary><table>
<thead><tr><th>Time after boot</th><th id="bootHead">ESP32 reading</th></tr></thead><tbody id="bootRows"></tbody></table></details></section>
<section id="sup" hidden><div class="row"><span class="dot"></span><h2>Supply voltage</h2><span class="st" id="sv"></span></div><div class="brief" id="sb"></div><svg id="sc" viewBox="0 0 600 270" role="img" aria-label="Supply voltage since the bridge was switched on"></svg><p class="key" id="sk" hidden>Each minute since the bridge was switched on, up to 12 hours: line = mean, band = lowest to highest, dashed = <span id="sl"></span> V warning level.</p></section>
<section class="set" id="lt" hidden><details><summary>Lifetime record</summary><div class="grp" id="lr"></div><p class="key">Kept through power cuts; operating time is saved every 10 minutes. Reset on the <a href="/settings">settings page</a>.</p></details></section>
<section class="set"><details><summary>All settings</summary><div id="set">Loading</div><p class="key"><a href="/settings">Change settings or restart the bridge</a></p></details></section>
<footer><a href="/monitor">Data monitor</a>: see what passes the bridge. <a href="/log">Log</a>: what the bridge did; <a href="/saved">saved log</a>: errors and warnings kept through power cuts. Read-only. Refreshes every 2 seconds; settings apply when the bridge starts. Data: <a href="/api/status">status</a>, <a href="/api/settings">settings</a>, <a href="/api/supply">voltage</a>.</footer>
</main>
<script>
const $=i=>document.getElementById(i),last={},frame={};let built=false,unc='';
const esc=x=>String(x).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'})[c]);
function dur(s){const h=Math.floor(s/3600),m=Math.floor(s%3600/60);return h?h+' h '+m+' min':m?m+' min '+s%60+' s':s+' s'}
function size(b){return b<1e4?b+' B':b<1e7?(b/1024).toFixed(1)+' kB':(b/1048576).toFixed(1)+' MB'}
function ago(ms){return ms===null?'No data yet':ms<5000?'Receiving':'Quiet for '+dur(Math.round(ms/1000))}
function build(ports){let h='';ports.forEach((p,n)=>{h+='<section id="d'+n+'"><div class="row"><span class="dot"></span><h2>'+esc(p.name)+'</h2><span class="st" id="s'+n+'"></span></div><div class="brief" id="b'+n+'"></div><details><summary>Details</summary><dl id="x'+n+'"></dl><p class="key"><a href="/monitor?port='+n+'">Show data</a></p></details></section>'});$('ports').innerHTML=h;built=true}
function f(){fetch('/api/status',{cache:'no-store'}).then(r=>r.json()).then(d=>{
$('down').hidden=true;const w=d.wifi;
const link=w.mode==='hotspot'?'<div>Hotspot <b>'+w.ip+'</b></div><div>'+(w.stations.length?w.stations.length+(w.stations.length>1?' phones, ':' phone, ')+'signal <b>'+w.stations.map(x=>x+' dBm').join(', ')+'</b>':'No phone connected')+'</div>':'<div>Network <b>'+w.ip+'</b></div><div>Signal <b>'+w.rssi_dbm+' dBm</b></div>';
const ct=d.chip_temp;unc=ct&&!ct.corrected?' <a class="chg" href="/settings">uncorrected</a>':'';
const chip=ct?'<div>Chip temperature <b'+(ct.celsius!==null&&ct.celsius+ct.offset>=ct.warn_c?' class="bad"':'')+'>'+(ct.celsius===null?'no reading':Math.round(ct.celsius)+' °C')+'</b>'+unc+'</div>':'';
const samples=ct?.boot_samples,first=samples?.[0];
const degrees=c=>Number.isFinite(c)?c.toFixed(1)+' °C':'no reading';
const boot=first?'<div>Chip before Wi-Fi <b>'+degrees(first.celsius)+'</b>'+unc+' · '+first.ms+' ms after boot</div>':'';
$('bootHead').textContent='ESP32 reading'+(unc?' (uncorrected)':'');
$('bootTemps').hidden=!first;
if(first)$('bootRows').innerHTML=samples.map(s=>'<tr><td>'+(s.ms/1000).toFixed(3)+' s</td><td>'+degrees(s.celsius)+'</td></tr>').join('');
$('meta').innerHTML='<div>Firmware <b>'+esc(d.firmware.replace('Wi-Fi bridge ',''))+'</b></div><div>Up <b>'+dur(d.uptime_s)+'</b></div>'+chip+boot+link;
if(!built)build(d.ports);
d.ports.forEach((p,n)=>{const on=p.last_data_ms_ago!==null&&p.last_data_ms_ago<5000,moved=last[n]!==undefined&&last[n]!==p.bytes_from_device;last[n]=p.bytes_from_device;
$('d'+n).className=(on?'on':'')+(moved?' pulse':'');$('s'+n).textContent=ago(p.last_data_ms_ago);
let b=p.baud+' baud'+(frame[n]?' '+frame[n]:'')+', TCP '+p.tcp_port+', '+(p.tcp_clients?p.tcp_clients+(p.tcp_clients>1?' apps':' app')+' connected':'no app connected');
if(p.uart_errors)b+=', <span class="bad">'+p.uart_errors+' receive error'+(p.uart_errors>1?'s':'')+'</span>';
if(p.short_writes)b+=', <span class="bad">'+p.short_writes+' short write'+(p.short_writes>1?'s':'')+'</span>';
$('b'+n).innerHTML=b;
$('x'+n).innerHTML='<div><dt>From device</dt><dd>'+size(p.bytes_from_device)+'</dd></div><div><dt>To device</dt><dd>'+size(p.bytes_to_device)+'</dd></div><div><dt>Receive errors</dt><dd'+(p.uart_errors?' class="bad"':'')+'>'+p.uart_errors+'</dd></div><div><dt>Short writes</dt><dd'+(p.short_writes?' class="bad"':'')+'>'+p.short_writes+'</dd></div>'});
sup(d.supply);life(d.lifetime)}).catch(()=>{$('down').hidden=false})}
function sup(s){if(!s)return;$('sup').hidden=false;const on=s.sensor&&s.volts!==null;$('sup').className=on?'on':'';
$('sv').innerHTML=!s.sensor?'Sensor off':s.volts===null?'<span class="bad">No reading</span>':'<b class="'+(s.volts<s.low_v?'bad':'')+'">'+s.volts.toFixed(2)+' V</b>';
$('sb').innerHTML=s.sensor?'OpenVario, TCP '+s.tcp_port+', '+(s.tcp_clients?s.tcp_clients+(s.tcp_clients>1?' apps':' app')+' connected':'no app connected'):'Switch the sensor on in the settings once the divider is fitted.'}
function when(e){return 'at '+dur(e.up_s)+' operating time'+(e.unix?', '+new Date(e.unix*1000).toLocaleDateString()+' '+hm(e.unix*1000):'')}
function life(l){if(!l)return;$('lt').hidden=false;const c=l.max_chip,v=l.min_supply;
$('lr').innerHTML='<span>Operating time</span><div>'+dur(l.up_s)+'</div><span>Starts</span><div>'+l.starts+'</div>'+(l.since_unix?'<span>Since</span><div>'+new Date(l.since_unix*1000).toLocaleDateString()+'</div>':'')+
'<span>Highest chip temperature</span><div>'+(c?'<b>'+Math.round(c.celsius)+' °C</b>'+unc+' '+when(c):'None yet')+'</div><span>Lowest supply voltage</span><div>'+(v?'<b>'+v.volts.toFixed(2)+' V</b> '+when(v)+' (held 10 s)':'None yet')+'</div>'+(l.storage?'':'<span class="bad">Storage</span><div class="bad">Not available: kept until power off only</div>')}
let hist=null;function hg(){fetch('/api/supply',{cache:'no-store'}).then(r=>r.json()).then(h=>{hist=h;draw()}).catch(()=>{})}
function hm(t){const d=new Date(t);return String(d.getHours()).padStart(2,'0')+':'+String(d.getMinutes()).padStart(2,'0')}
function draw(){const h=hist,c=$('sc');$('sk').hidden=!(h&&h.sensor);if(!h||!h.sensor){c.innerHTML='';c.style.display='none';return}c.style.display='';
const W=600,H=270,L=62,R=12,T=12,B=34,now=Date.now(),m=h.points.length,tE=now-h.age_s*1000,P=[];
h.points.forEach((p,k)=>P.push([tE-(m-1-k)*6e4-3e4,p]));if(h.current)P.push([(tE+now)/2,h.current]);
const v=P.filter(q=>q[1]);if(!v.length){c.innerHTML='<text x="'+W/2+'" y="'+H/2+'" text-anchor="middle">The chart starts with the first reading</text>';return}
const t0=m?tE-m*6e4:tE,t1=Math.max(now,t0+6e5);let lo=Math.min(...v.map(q=>q[1][0]))/100,hi=Math.max(...v.map(q=>q[1][2]))/100;
lo-=.2;hi+=.2;if(lo<h.low_v+.6)lo=Math.min(lo,h.low_v-.2);if(hi-lo<1){const c2=(hi+lo)/2;lo=c2-.5;hi=c2+.5}
const X=t=>L+(t-t0)/(t1-t0)*(W-L-R),Y=u=>T+(hi-u)/(hi-lo)*(H-T-B);let s='';
const st=[.1,.2,.5,1,2,5].find(x=>(hi-lo)/x<=5);for(let u=Math.ceil(lo/st)*st;u<=hi+1e-9;u+=st)s+='<line class="gr" x1="'+L+'" x2="'+(W-R)+'" y1="'+Y(u)+'" y2="'+Y(u)+'"/><text x="'+(L-8)+'" y="'+(Y(u)+6)+'" text-anchor="end">'+u.toFixed(st<1?1:0)+'</text>';
const tz=-new Date().getTimezoneOffset()*6e4,ms=[5,10,15,30,60,120,180].map(x=>x*6e4).find(x=>(t1-t0)/x<=6);
for(let t=Math.ceil((t0+tz)/ms)*ms-tz;t<=t1;t+=ms)s+='<line class="gr" x1="'+X(t)+'" x2="'+X(t)+'" y1="'+T+'" y2="'+(H-B)+'"/><text x="'+X(t)+'" y="'+(H-8)+'" text-anchor="middle">'+hm(t)+'</text>';
if(h.low_v>=lo&&h.low_v<=hi)s+='<line class="lw" x1="'+L+'" x2="'+(W-R)+'" y1="'+Y(h.low_v)+'" y2="'+Y(h.low_v)+'"/>';
const runs=[];let r=[];for(const q of P){if(q[1])r.push(q);else if(r.length){runs.push(r);r=[]}}if(r.length)runs.push(r);
for(const g of runs){if(g.length===1){const q=g[0];s+='<rect class="bd" x="'+(X(q[0])-2)+'" width="4" y="'+Y(q[1][2]/100)+'" height="'+Math.max(1,Y(q[1][0]/100)-Y(q[1][2]/100))+'"/><circle cx="'+X(q[0])+'" cy="'+Y(q[1][1]/100)+'" r="2.5" fill="var(--green)"/>';continue}
s+='<polygon class="bd" points="'+g.map(q=>X(q[0])+','+Y(q[1][2]/100)).concat(g.slice().reverse().map(q=>X(q[0])+','+Y(q[1][0]/100))).join(' ')+'"/>';
s+='<polyline class="mn" points="'+g.map(q=>X(q[0])+','+Y(q[1][1]/100)).join(' ')+'"/>'}
$('sl').textContent=h.low_v.toFixed(1);c.innerHTML=s}
function g(){fetch('/api/settings',{cache:'no-store'}).then(r=>r.json()).then(d=>{
const ports={},order=[],wifi=[];for(const i of d.items){if(i.port){if(!ports[i.port]){ports[i.port]={};order.push(i.port)}ports[i.port][i.key[0]]=i}else wifi.push(i)}
order.forEach((k,n)=>{frame[n]=ports[k].f.value});
const pn={custom:'Custom'};for(const p of d.profiles||[])pn[p.id]=p.name;
const cell=(i,t)=>{const v=i.secret?'\u2022\u2022\u2022\u2022\u2022\u2022':esc(t||i.value);return i.origin==='changed'?'<b class="chg">'+v+'</b>':i.origin==='invalid'?'<b class="bad">'+v+'</b>':v};
let h=d.format_mismatch?'<p class="bad">Settings saved by another firmware version were ignored; the defaults are in use.</p>':'';
h+='<table><tr><th>Port</th><th>Device</th><th>Baud</th><th>Frame</th><th>TCP</th></tr>';
for(const k of order){const p=ports[k];h+='<tr><td>'+esc(k)+'</td><td>'+(p.p?cell(p.p,pn[p.p.value]):'')+'</td><td>'+cell(p.b)+'</td><td>'+cell(p.f)+'</td><td>'+cell(p.t)+'</td></tr>'}
h+='</table><div class="grp">';for(const i of wifi){const nm=(i.group&&i.group!=='Wi-Fi'?i.group+': ':'')+i.name;h+='<span>'+esc(nm.charAt(0).toUpperCase()+nm.slice(1))+'</span><div>'+cell(i)+'</div>'}
const any=d.items.some(i=>i.origin!=='default');
if(d.trial&&d.trial.active){$('trial').hidden=false;$('tt').textContent='New Wi-Fi or port settings are on trial for '+Math.ceil(d.trial.seconds_left/60)+' more min.'}
$('set').innerHTML=h+'</div>'+(any?'<p class="key"><b class="chg">Amber</b>: saved on the bridge. <b class="bad">Red</b>: saved value is invalid, the default is in use.</p>':'')}).catch(()=>setTimeout(g,2000))}
g();f();hg();setInterval(f,2000);setInterval(hg,30000);
</script>)HTML";

static String statusJson() {
    char part[384];  // one port line with all counters at their maximum is about 260 bytes
    String out;
    out.reserve(2048);  // Project 0.5.12: the full answer is about 1.5 kB; no regrowth while it is built
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
        // Project 0.5.12: the other core can store a time up to 1 ms after `now` (millis() | 1, or a byte arriving
        // while this loop runs); a signed age clamped at 0 avoids the wrap to 4294967295 ms ("Quiet for 1193 h").
        const int32_t ageMs = (int32_t)(now - last);
        if (last) snprintf(age, sizeof(age), "%lu", (unsigned long)(ageMs < 0 ? 0 : ageMs));
        else snprintf(age, sizeof(age), "null");
        snprintf(part, sizeof(part),
                 "%s{\"name\":\"%s\",\"uart\":%d,\"baud\":%lu,\"tcp_port\":%u,"
                 "\"bytes_from_device\":%lu,\"bytes_to_device\":%lu,\"last_data_ms_ago\":%s,"
                 "\"tcp_clients\":%lu,\"uart_errors\":%lu,\"short_writes\":%lu}",
                 num ? "," : "", kPortName[num], num, (unsigned long)settings.port[num].baud, settings.port[num].tcpPort,
                 (unsigned long)p.fromDevice.load(), (unsigned long)p.toDevice.load(), age,
                 (unsigned long)p.tcpClients.load(), (unsigned long)p.uartErrors.load(),
                 (unsigned long)p.shortWrites.load());
        out += part;
    }
    out += "]";
    supplyStatusJson(out);  // Project: supply voltage (step 3d)
    chipTempJson(out);      // Project: chip temperature (step 3e)
    lifetimeJson(out);      // Project: lifetime record (step 3f)
    savedJson(out);         // Project: saved log (step 3g)
    out += "}";
    return out;
}

static void jsonString(String& out, const char* s) {  // quoted and escaped JSON string
    out += '"';
    for (; *s; s++) {
        const char c = *s;
        if (c == '"' || c == '\\') { out += '\\'; out += c; }
        else if ((uint8_t)c < 0x20) { char u[8]; snprintf(u, sizeof(u), "\\u%04x", (uint8_t)c); out += u; }
        else out += c;
    }
    out += '"';
}

// Active settings and where each comes from (step 3a). Passwords are never sent.
static String settingsJson() {
    String out;
    out.reserve(2048);
    out += settingsFormatMismatch ? "{\"format_mismatch\":true," : "{\"format_mismatch\":false,";
    char part[160];  // one profile is about 115 bytes
    out += "\"profiles\":[";  // device profiles (WP9): what each preset fills in and needs
    for (size_t i = 0; i < kProfileCount; i++) {
        const DeviceProfile& p = kProfiles[i];
        snprintf(part, sizeof(part), "%s{\"id\":\"%s\",\"name\":\"%s\",\"baud\":\"%lu\",\"frame\":\"%s\",\"interface\":\"%s\"}",
                 i ? "," : "", p.id, p.name, (unsigned long)p.baud, p.frame, linkText(p.link));
        out += part;
    }
    out += "],\"ports\":[";  // per port: its wiring and the profiles that fit it
    for (int n = 0; n < NUM_COM; n++) {
        snprintf(part, sizeof(part), "%s{\"name\":\"%s\",\"interface\":\"%s\",\"profiles\":[", n ? "," : "", kPortName[n],
                 linkText(kPortLink[n]));
        out += part;
        bool first = true;
        for (const DeviceProfile& p : kProfiles) {
            if (!profileFits(p.id, n)) continue;
            out += first ? "\"" : ",\"";
            out += p.id;
            out += "\"";
            first = false;
        }
        out += "]}";
    }
    out += "],";
    if (settingsTrial) {
        const uint32_t left = millis() < SETTINGS_TRIAL_MS ? (SETTINGS_TRIAL_MS - millis()) / 1000 : 0;
        out += "\"trial\":{\"active\":true,\"seconds_left\":" + String(left);
#ifdef STORED_SETTINGS
        out += ",\"starts_left\":" + String(settingsTrialStartsLeft());
#endif
        out += "},\"items\":[";
    } else {
        out += "\"trial\":{\"active\":false},\"items\":[";
    }
    BridgeSettings defaults;
    fillDefaults(defaults);
    char value[72];
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        const int n = portOf(s);
        if (i) out += ",";
        out += "{\"key\":\"";
        out += s.key;
        out += "\",\"port\":";
        if (n >= 0) jsonString(out, kPortName[n]); else out += "null";
        out += ",\"group\":";
        jsonString(out, n >= 0 ? kPortName[n] : supplyKey(s) ? "Supply voltage" : chipKey(s) ? "Chip temperature" : "Wi-Fi");
        out += ",\"name\":";
        jsonString(out, settingName(s));
        out += ",\"secret\":";
        out += settingSecret(s) ? "true" : "false";
        out += ",\"value\":";
        if (settingSecret(s)) out += "null";
        else { settingValueText(s, value, sizeof(value)); jsonString(out, value); }
        out += ",\"default\":";
        if (settingSecret(s)) {
            out += "null";
        } else {
            SettingItem d = s;  // same item, pointing at the default value
            d.value = (uint8_t*)&defaults + ((const uint8_t*)s.value - (const uint8_t*)&settings);
            settingValueText(d, value, sizeof(value));
            jsonString(out, value);
        }
        out += ",\"origin\":\"";
        out += settingOriginText(s.origin);
        out += "\"}";
    }
    out += "]}";
    return out;
}

#include "settings_page.h"  // Project: settings page (step 3a)
#include "monitor.h"        // Project: data monitor (step 3b)
#include "log_page.h"       // Project: web log (step 3c)

static void statusTask(void*) {
    statusServer.on("/", HTTP_GET, []() { statusServer.send(200, "text/html", kStatusPage); });
    statusServer.on("/device.webp", HTTP_GET, []() {
        statusServer.sendHeader("Cache-Control", "max-age=86400");
        statusServer.send_P(200, "image/webp", (const char*)kDeviceImage, sizeof(kDeviceImage));
    });
    statusServer.on("/api/status", HTTP_GET, []() {
        statusServer.sendHeader("Cache-Control", "no-store");
        statusServer.send(200, "application/json", statusJson());
    });
    statusServer.on("/api/settings", HTTP_GET, []() {
        statusServer.sendHeader("Cache-Control", "no-store");
        statusServer.send(200, "application/json", settingsJson());
    });
    settingsRoutes();
    monitorRoutes();
    logRoutes();
    supplyRoutes();
#ifdef BENCH_OPEN
    benchRoutes(statusServer);  // bench.h: core dump download
#endif
    savedRoutes();
    lifetimeBegin();
    savedLogBegin();
    supplyBegin();
    statusServer.onNotFound([]() { statusServer.send(404, "text/plain", "Not found"); });
    statusServer.begin();
    for (;;) {
        statusServer.handleClient();
        settingsTrialCheck();
        logTick();
        supplyTick();
        chipTempTick();
        lifetimeTick();
        savedLogTick();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Before COM[num]->begin(): count UART receive errors.
static void statusAttachUart(int num) {
    COM[num]->onReceiveError([num](hardwareSerial_error_t e) {
#ifdef NAV_DETECTION
        if (navPort.load() == num) {
            navRxError.store(true);
            return;  // Deliberately trying a mismatched rate is normal during detection.
        }
#endif
        STAT(num, uartErrors, 1);
        portStats[num].lastError.store(e, std::memory_order_relaxed);
    });
}

// At the end of setup(), once Wi-Fi is up.
static void statusBegin() {
    // Idle priority: the WebServer header reader spins with yield() while a
    // request trickles in (up to 10 s); at priority 1 that starved IDLE0 and
    // the task watchdog reset the bridge (two core dumps, 2026-11-23).
    xTaskCreatePinnedToCore(statusTask, "status", 6144, nullptr, tskIDLE_PRIORITY, nullptr, 0);
    MDNS.addService("http", "tcp", 80);
}

#else

#define STAT(num, field, n) ((void)0)
#define STAT_RX(num) ((void)0)
#define STAT_CLIENTS(num, n) ((void)0)
#define MON(num, dir, data, len) ((void)0)
static inline void statusAttachUart(int) {}
static inline void statusBegin() {}

#endif
