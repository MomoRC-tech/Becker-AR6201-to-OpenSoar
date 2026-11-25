// Project: settings page (firmware plan step 3a, R15). ESP32 only.
//
// GET  /settings          the page
// POST /api/settings      check and save, then restart
// POST /api/keep          keep settings that are on trial
// POST /api/reset         forget all stored settings, then restart
// POST /api/restart       restart with the stored settings
// POST /api/chip-temp/calibrate set the chip temperature offset, then restart
// POST /api/lifetime/reset start a new lifetime record (lifetime.h)
// POST /api/saved/clear   delete the saved log (savedlog.h)
//
// No login (owner decision, 0.5.17). The
// active values come from the public /api/settings, which never sends passwords.
//
// Included by status_page.h inside its ESP32 and STATUS_PAGE block.

#pragma once

#ifdef STORED_SETTINGS

// Owner decision (0.5.17): no settings login on any build; anyone on the bridge's Wi-Fi
// can change settings. Firmware uploads (OTA) keep their password.
static bool settingsAuth() { return true; }

// Before a planned restart: end every app connection with a FIN, so the apps see the
// close and reconnect. Without it a phone that stays on the network keeps dead
// connections after the restart and shows "no data" until the user reconnects (S197).
// The app ports stop listening first: an app reconnects within a fraction of a second
// of the close, and that new connection would die unnoticed with the restart (S198);
// refused, the app tries again after the restart.
// Each task closes what it owns (closeNavApps in the loop task, closeSupplyApps in the
// status task), so no accept() runs on a closed listener and no pending connection is
// missed; the calling task does its part and waits up to 1 s for the other.
static void closeAppsBeforeRestart(bool inLoopTask) {
    appsClosing = true;
    if (inLoopTask) closeNavApps();
    else closeSupplyApps();
    const uint32_t start = millis();
    while (!(navAppsClosed && supplyAppsClosed) && millis() - start < 1000) delay(5);
    LOGI(LOG_SYSTEM, "Restart: %d app connection(s) closed%s", navAppsClosedN + supplyAppsClosedN,
         navAppsClosed && supplyAppsClosed ? "" : " (one task did not answer in 1 s)");
    delay(300);  // let the FINs go out
}

static void restartSoon() {  // status task (web handlers)
    delay(800);  // let the answer reach the phone
    closeAppsBeforeRestart(false);
    lifetimeSaveNow();
    ESP.restart();
}

static void handleNavScan() {
    if (!settingsAuth()) return;
    statusServer.sendHeader("Cache-Control", "no-store");
    if (statusServer.method() == HTTP_POST) {
        if (statusServer.hasArg("cancel")) {
            const NavResult current = navSnapshot();
            if (statusServer.arg("cancel") != String(current.id)) {
                statusServer.send(409, "text/plain", "This scan has been replaced.");
                return;
            }
            navCancel.store(true);
        } else {
            const String p = statusServer.arg("port");
            if (p != "0" && p != "1") {
                statusServer.send(400, "text/plain", "Select a navigation input.");
                return;
            }
            if (!navStart(p == "0" ? 0 : 1)) {
                statusServer.send(409, "text/plain", "A navigation scan is already running.");
                return;
            }
        }
    }
    const NavResult r = navSnapshot();
    static const char* states[] = {"idle", "listening", "measuring", "found", "quiet", "undecoded", "cancelled"};
    char body[192];
    snprintf(body, sizeof(body),
             "{\"id\":%u,\"port\":%d,\"state\":\"%s\",\"baud\":%lu,\"frame\":\"%s\",\"zander\":%u,\"flarm\":%u,\"generic\":%u}",
             r.id, r.port, states[unsigned(r.state)], (unsigned long)r.baud, r.frame,
             r.counts.zander, r.counts.flarm, r.counts.generic);
    statusServer.send(200, "application/json", body);
}

static const char* settingHint(SettingType type, int portNum) {
    switch (type) {
        case T_PROFILE: return "Choose a listed device; the others do not fit this port's wiring";
        case T_BAUD: return "Choose one of the listed rates";
        case T_FRAME: return "Data bits 5 to 8, parity N, E or O, stop bits 1 or 2, for example 8N1";
        case T_TCP: return "1 to 65535, not 80, not used by another device";
        case T_SSID: return "1 to 32 characters";
        case T_NETSSID: return "Empty for no network, or up to 32 characters";
        case T_NETPASS: return "8 to 63 characters";
        case T_APPASS: return "8 to 63 characters";
        case T_SECONDS: return "5 to 120 seconds";
        case T_IP: return "For example 10.10.10.10";
        case T_PHONE: return "Empty for automatic, or an address such as 192.168.43.1";
        case T_FLAG: return "On or off";
        case T_CAL: return "0.900 to 1.100";
        case T_OFFSET: return "0.0 to 60.0; 0 = no correction";
    }
    return "";
}

// Parse one form value into v. Empty passwords mean: keep the current one.
static bool parseInto(const SettingItem& s, const String& raw, void* v) {
    String a = raw;
    if (!settingSecret(s)) a.trim();
    switch (s.type) {
        case T_BAUD:
        case T_TCP:
        case T_SECONDS: {
            if (a.length() == 0 || a.length() > 6) return false;
            for (size_t i = 0; i < a.length(); i++)
                if (!isDigit(a[i])) return false;
            const unsigned long n = strtoul(a.c_str(), nullptr, 10);
            if (s.type == T_BAUD) *(uint32_t*)v = n;
            else if (n > 65535) return false;
            else *(uint16_t*)v = (uint16_t)n;
            return true;
        }
        case T_IP:
        case T_PHONE: {
            if (s.type == T_PHONE && a.length() == 0) {  // automatic
                *(uint32_t*)v = 0;
                return true;
            }
            IPAddress ip;
            if (!ip.fromString(a)) return false;
            *(uint32_t*)v = (uint32_t)ip;
            return true;
        }
        case T_FLAG:
            if (a != "on" && a != "off") return false;
            *(uint8_t*)v = a == "on";
            return true;
        case T_CAL: {  // "1.023" -> 1023
            char* end = nullptr;
            const double f = strtod(a.c_str(), &end);
            if (a.length() == 0 || a.length() > 8 || *end || !(f > 0 && f < 10)) return false;
            *(uint16_t*)v = (uint16_t)lround(f * 1000);
            return true;
        }
        case T_OFFSET: {  // "37.8" -> 378, range in validAt()
            a.replace(',', '.');
            char* end = nullptr;
            const double f = strtod(a.c_str(), &end);
            if (a.length() == 0 || a.length() > 8 || *end || !(f > -1000 && f < 1000)) return false;
            *(int16_t*)v = (int16_t)lround(f * 10);
            return true;
        }
        default: {
            if (settingSecret(s) && a.length() == 0) return true;  // unchanged
            const size_t cap = valueSize(s.type);
            if (a.length() >= cap) return false;
            memset(v, 0, cap);
            strlcpy((char*)v, a.c_str(), cap);
            return true;
        }
    }
}

static void addError(String& errors, const char* key, const char* text) {
    errors += errors.length() ? "," : "";
    errors += "\"";
    errors += key;
    errors += "\":";
    jsonString(errors, text);
}

static void settingsStorageError() {
    statusServer.send(503, "application/json",
                      "{\"ok\":false,\"message\":\"Could not update settings in flash. Try again.\"}");
}

static void handleSettingsSave() {
    if (!settingsAuth()) return;
    BridgeSettings cand = settings;  // fields not sent keep their active value
    String errors;
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        if (!statusServer.hasArg(s.key)) continue;
        const size_t off = (const uint8_t*)s.value - (const uint8_t*)&settings;
        void* v = (uint8_t*)&cand + off;
        const int n = portOf(s);
        if (!parseInto(s, statusServer.arg(s.key), v) || !validAt(s.type, v, n)) addError(errors, s.key, settingHint(s.type, n));
    }
    for (int a = 0; a < TCP_LISTENERS; a++) {
        for (int b = a + 1; b < TCP_LISTENERS; b++) {
            if (tcpPortOf(cand, a) == tcpPortOf(cand, b)) {
                char k[3] = {'t', char('0' + b), 0};
                addError(errors, b < NUM_COM ? k : "vt", "Already used by another device");
            }
        }
    }
    if (errors.length()) {
        LOGW(LOG_SETTINGS, "Save refused: a field is wrong, nothing changed");
        statusServer.send(400, "application/json", "{\"ok\":false,\"errors\":{" + errors + "}}");
        return;
    }
    profilesMatchValues(cand);  // a preset with edited baud or frame is stored as Custom
    bool trial = false;
    if (!settingsSave(cand, trial)) {
        settingsStorageError();
        return;
    }
    statusServer.send(200, "application/json", trial ? "{\"ok\":true,\"trial\":true}" : "{\"ok\":true,\"trial\":false}");
    restartSoon();
}

#ifdef CHIP_TEMP
// Chip temperature calibration: after at least 30 min off in the same place the chip is
// at room temperature, so the earliest reading of a power-on start minus the room
// temperature is the sensor offset (about +38 degrees C on the bench bridge, S187).
// Form: room (degrees C), cold=on (the user confirms the 30 min off). Saves "ct", restarts.
static void calibrateRefuse(const char* text) {
    String body = "{\"ok\":false,\"message\":";
    jsonString(body, text);
    body += "}";
    statusServer.send(400, "application/json", body);
}

static void handleChipCalibrate() {
    if (!settingsAuth()) return;
    if (esp_reset_reason() != ESP_RST_POWERON)
        return calibrateRefuse("This start was not from power on. Switch the bridge off for at least 30 minutes, "
                               "switch it on, then calibrate.");
    const ChipTempSample first = chipBootSamples[0];
    if (chipBootCount.load(std::memory_order_acquire) == 0 || !first.raw)
        return calibrateRefuse("The sensor gave no reading at this start; calibration is not possible.");
    if (statusServer.arg("cold") != "on")
        return calibrateRefuse("Confirm that the bridge was off for at least 30 minutes, in the same place as now.");
    String a = statusServer.arg("room");
    a.trim();
    a.replace(',', '.');
    char* end = nullptr;
    const double room = strtod(a.c_str(), &end);
    if (a.length() == 0 || a.length() > 8 || *end || !(room >= 0 && room <= 45))
        return calibrateRefuse("Enter the room temperature, 0 to 45 \xC2\xB0" "C.");
    const float firstC = (first.raw - 32.0f) / 1.8f;
    BridgeSettings cand = settings;
    cand.chipTempOffset = (int16_t)lroundf((firstC - (float)room) * 10);
    if (!validAt(T_OFFSET, &cand.chipTempOffset, -1))
        return calibrateRefuse("The result is outside 0 to 60 \xC2\xB0" "C. Check the room temperature and that the "
                               "bridge was really off for 30 minutes.");
    LOGI(LOG_SETTINGS, "Chip calibrated: first %.1f, room %.1f, offset %.1f \xC2\xB0" "C", firstC, room,
         cand.chipTempOffset / 10.0);
    bool trial = false;
    if (!settingsSave(cand, trial)) {
        settingsStorageError();
        return;
    }
    char body[64];
    snprintf(body, sizeof(body), "{\"ok\":true,\"offset\":%.1f,\"trial\":%s}", cand.chipTempOffset / 10.0,
             trial ? "true" : "false");
    statusServer.send(200, "application/json", body);
    restartSoon();
}
#endif

static void handleSettingsKeep() {
    if (!settingsAuth()) return;
    if (!settingsKeep()) {
        settingsStorageError();
        return;
    }
    statusServer.send(200, "application/json", "{\"ok\":true}");
}

// Restart only: nothing is written. During a trial the next start counts as one
// of the SETTINGS_TRIAL_BOOTS starts and the 10-minute window begins again.
static void handleRestart() {
    if (!settingsAuth()) return;
    LOGI(LOG_SETTINGS, "Restart requested on the settings page");
    statusServer.send(200, "application/json", "{\"ok\":true}");
    restartSoon();
}

static void handleSettingsReset() {
    if (!settingsAuth()) return;
    if (!settingsReset()) {
        settingsStorageError();
        return;
    }
    statusServer.send(200, "application/json", "{\"ok\":true}");
    restartSoon();
}

// Called from the status task loop: a trial that was not kept in time goes back.
static void settingsTrialCheck() {
    static uint32_t lastAttempt = 0;
    const uint32_t now = millis();
    if (settingsTrial && now > SETTINGS_TRIAL_MS &&
        (lastAttempt == 0 || now - lastAttempt >= 5000)) {
        lastAttempt = now;
        if (!settingsRevert()) return;
        closeAppsBeforeRestart(false);  // status task
        lifetimeSaveNow();
        delay(200);
        ESP.restart();
    }
}

static const char kSettingsPage[] = R"HTML(<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="color-scheme" content="light dark"><title>WIFI Bridge settings</title><link rel="icon" href="/device.webp">
<style>
:root{--ink:#262a27;--paper:#f4f6f3;--rule:#d5ddd6;--mute:#5d665f;--amber:#b26b00;--red:#b3261e;--green:#2f6b3b;--field:#fff}
@media (prefers-color-scheme:dark){:root{--paper:#141a16;--ink:#e6ebe6;--rule:#2c3630;--mute:#9aa59c;--amber:#f0a73a;--red:#ff8a80;--green:#7cc487;--field:#1c241f}}
*{box-sizing:border-box}
body{margin:0;background:var(--paper);color:var(--ink);font:16px/1.45 system-ui,"Segoe UI",Roboto,sans-serif}
main{max-width:640px;margin:0 auto;padding:16px 16px 40px}
header{display:flex;gap:14px;align-items:center;padding-bottom:12px}
header img{width:52px;height:auto;border-radius:5px}
h1{margin:0;font:800 26px/1.1 Arial,Helvetica,sans-serif;color:var(--green)}
header a{margin-left:auto;font-size:15px}
a{color:inherit}
.trial{border-left:4px solid var(--amber);padding:10px 14px;margin:0 0 12px}
.trial p{margin:0 0 8px}
fieldset{border:0;border-top:1px solid var(--rule);margin:0;padding:14px 0}
legend{font-size:20px;font-weight:700;padding:0 0 4px}
.f{display:grid;grid-template-columns:repeat(auto-fill,minmax(130px,1fr));gap:12px 16px}
.f.w{grid-template-columns:1fr}
.dev{grid-column:1/-1}
label{display:block;color:var(--mute);font-size:14px}
input,select{display:block;width:100%;margin-top:3px;padding:8px 10px;font:inherit;color:var(--ink);background:var(--field);border:1px solid var(--rule);border-radius:8px}
input:focus,select:focus,button:focus-visible{outline:2px solid var(--green);outline-offset:1px}
input:disabled{opacity:.6}
.def{font-size:13px;color:var(--mute)}.chg{color:var(--amber)}
.err{color:var(--red);font-size:13px}
.note{color:var(--mute);font-size:14px;margin:4px 0 16px}
.btns{display:flex;gap:12px;flex-wrap:wrap;align-items:center}
button{font:inherit;font-weight:600;border-radius:10px;padding:10px 18px;cursor:pointer;border:1px solid var(--green)}
.main{background:var(--green);color:var(--paper)}
.quiet{background:none;color:var(--ink);border-color:var(--rule)}
#msg{margin-top:14px}
.menu{position:sticky;top:0;z-index:2;background:var(--paper);display:flex;flex-wrap:wrap;gap:6px;padding:8px 0;border-bottom:1px solid var(--rule)}
.menu button{padding:6px 12px;border-radius:999px;font-size:14px;background:none;color:var(--ink);border-color:var(--rule)}
.menu button.on{background:var(--green);color:var(--paper);border-color:var(--green)}
details.sec{border:1px solid var(--rule);border-radius:10px;margin:10px 0;scroll-margin-top:96px}
details.sec>summary{display:flex;gap:8px;align-items:center;padding:12px 14px;font-size:18px;font-weight:700;cursor:pointer;list-style:none}
details.sec>summary::-webkit-details-marker{display:none}
details.sec>summary::after{content:'\25B8';margin-left:auto;color:var(--mute)}
details.sec[open]>summary::after{content:'\25BE'}
details.sec>.body{padding:0 14px 14px}
.badge{font-size:12px;font-weight:600;color:var(--amber);border:1px solid var(--amber);border-radius:999px;padding:1px 8px}
.badge.e{color:var(--red);border-color:var(--red)}
.bar{position:sticky;bottom:0;z-index:2;background:var(--paper);border-top:1px solid var(--rule);padding:10px 0;margin-top:12px}
.bar .note{margin:0 0 8px}
</style>
<main>
<header><img src="/device.webp" width="240" height="317" alt=""><h1>Settings</h1><a href="/">Status page</a></header>
<div class="trial" id="trial" hidden><p id="tt"></p><button class="main" type="button" onclick="keep()">Keep these settings</button></div>
<nav class="menu" id="menu" aria-label="Sections"></nav>
<form id="fm" onsubmit="return save(event)"><div id="fs">Loading</div>
<div class="bar"><p class="note">Saving restarts the bridge. A change to the network, hotspot or ports (also the supply voltage port) runs on trial: reconnect and press Keep within 10 minutes, otherwise the previous settings come back. Leave a password empty to keep it.</p>
<div class="btns"><button class="main" type="submit">Save and restart</button><button class="quiet" type="button" onclick="reset()">Reset to defaults</button></div></div></form>
<details class="sec" id="s_Records" hidden><summary>🗂️ Records</summary><div class="body"><p class="note">Kept through power cuts: the lifetime record (operating time, starts, highest chip temperature, lowest supply voltage) and the saved log (errors and warnings). Clearing them leaves the settings as they are.</p><div class="btns"><button class="quiet" type="button" onclick="lreset()">Reset lifetime record</button><button class="quiet" type="button" id="sclr" hidden onclick="sclear()">Clear saved log</button></div></div></details>
<details class="sec" id="s_Restart"><summary>🔄 Restart</summary><div class="body"><p class="note">Restarts the bridge with the saved settings. Apps reconnect on their own.</p><div class="btns"><button class="quiet" type="button" onclick="restart()">Restart bridge</button></div></div></details>
<div id="msg" role="status"></div>
</main>
<script>
const $=i=>document.getElementById(i);
const esc=x=>String(x).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'})[c]);
const BAUD=[1200,2400,4800,9600,19200,38400,57600,115200,230400],FRAME=['8N1','8E1','8O1','8N2','7E1','7O1','7N1'];
let items=[],PROF={},PORTS=[],NAME={custom:'Custom'},groups=[];
const ICON={'Network':'📶','Hotspot':'📡','Zander':'🧭','FLARM':'✈️','Becker':'📻','Supply voltage':'🔋','Chip temperature':'🌡️','Records':'🗂️','Restart':'🔄'};
const sid=k=>'s_'+k.replace(/\W/g,'');
// Sections start closed; the menu opens one and closes the others.
function menu(){const ks=groups.concat($('s_Records').hidden?[]:['Records'],['Restart']);
$('menu').innerHTML=ks.map(k=>'<button type="button" data-k="'+esc(k)+'">'+(ICON[k]||'')+' '+esc(k)+'</button>').join('');
$('menu').querySelectorAll('button').forEach(b=>b.onclick=()=>show(b.dataset.k));
document.querySelectorAll('details.sec').forEach(d=>d.ontoggle=mark);mark()}
function show(k){const d=$(sid(k)),was=d&&d.open;document.querySelectorAll('details.sec').forEach(x=>x.open=false);if(d&&!was){d.open=true;d.scrollIntoView({block:'start'})}mark()}
// OpenSoar/XCSoar device entry for a TCP port field; the driver follows the device chosen on that port.
const DRV={zs1:'Zander / SDI',flarm:'FLARM',ar6201:'AR62xx',custom:'Generic'};
function ohint(k){const h=$('oh_'+k),t=$('k_'+k);if(!h||!t)return;const p=k==='vt'?null:$('k_p'+k[1]),d=k==='vt'?'OpenVario':DRV[p?p.value:'custom']||'Generic';
h.innerHTML='In OpenSoar: Device → Port <b>TCP port</b>, TCP port <b>'+esc(t.value)+'</b>, Driver <b>'+esc(d)+'</b>'}
function ohints(){for(const k of ['t0','t1','t2','vt'])ohint(k)}
function mark(){$('menu').querySelectorAll('button').forEach(b=>{const d=$(sid(b.dataset.k));b.classList.toggle('on',!!(d&&d.open))})}
function field(i){const id='k_'+i.key,lab=esc(i.name.charAt(0).toUpperCase()+i.name.slice(1)),n=i.key[1],dev=i.port&&i.key[0]==='p';let c,w='';
if(dev){const P=PORTS[+n];c='<select id="'+id+'" name="'+i.key+'" onchange="preset('+n+')">'+P.profiles.concat('custom').map(x=>'<option value="'+x+'"'+(x===i.value?' selected':'')+'>'+esc(NAME[x])+'</option>').join('')+'</select>';w='<span class="def">Wiring: '+esc(P.interface)+'. Presets fill in baud and frame.</span>'}
else if(i.port&&(i.key[0]==='b'||i.key[0]==='f')){const o=i.key[0]==='b'?BAUD:[...new Set([...FRAME,i.value])];c='<select id="'+id+'" name="'+i.key+'" onchange="manual('+n+')">'+o.map(x=>'<option'+(String(x)===i.value?' selected':'')+'>'+esc(x)+'</option>').join('')+'</select>'}
else if(i.secret)c='<input id="'+id+'" name="'+i.key+'" type="password" autocomplete="new-password" placeholder="Unchanged">';
else if(i.key==='vs')c='<select id="'+id+'" name="vs">'+['on','off'].map(x=>'<option'+(x===i.value?' selected':'')+'>'+x+'</option>').join('')+'</select>';
else{const num=i.key[0]==='t'||i.key==='nj'||i.key==='vt',dec=i.key==='vc'||i.key==='ct'||i.key==='pa';c='<input id="'+id+'" name="'+i.key+'" value="'+esc(i.value)+'"'+(num?' inputmode="numeric"':dec?' inputmode="decimal"':'')+'>';
if(i.key==='vc')w='<label for="mv">Calibrate: multimeter reading at the XT30, V</label><input id="mv" inputmode="decimal"><button class="quiet" type="button" onclick="cal()">Set factor</button><span class="def" id="cm"></span>';
if(i.key[0]==='t'||i.key==='vt'){w='<span class="def" id="oh_'+i.key+'"></span>';c=c.replace('>',' oninput="ohint(\''+i.key+'\')">')}
if(i.key==='pa')w='<span class="def">Where the bridge dials OpenSoar&#39;s TCP port entries when it is on a network. Empty: the phone that connected last, else the network&#39;s gateway (a phone hotspot).</span>';
if(i.key==='ct')w='<span class="def">The ESP32 sensor reads too high; this is subtracted from every chip temperature. 0 = uncorrected.</span><br><button class="quiet" type="button" onclick="ctc()">Calibrate chip temperature</button><div id="ctc" hidden></div>'}
const dv=dev?NAME[i.default]||i.default:i.default||(i.key==='pa'?'automatic':'');
if(dev && +n<2)w+='<p class="def">Detection briefly pauses forwarding on this input.</p><div class="btns"><button class="quiet detect" type="button" onclick="startScan('+n+')">Detect</button><button class="quiet" id="cancel'+n+'" hidden type="button" onclick="cancelScan()">Cancel scan</button><button class="quiet" id="use'+n+'" hidden type="button" onclick="useScan('+n+')">Use these settings</button></div><span class="def" role="status" id="scan'+n+'"></span>';
const d=i.secret?'':'<span class="def'+(i.origin==='changed'?' chg':'')+'">'+(i.origin==='changed'?'Changed, default ':'Default ')+esc(dv)+'</span>';
return '<div'+(dev||w?' class="dev"':'')+'><label for="'+id+'">'+lab+'</label>'+c+d+(w?'<br>'+w:'')+'<div class="err" id="e_'+i.key+'"></div></div>'}
function cal(){const m=parseFloat($('mv').value.replace(',','.'));fetch('/api/status',{cache:'no-store'}).then(r=>r.json()).then(d=>{const s=d.supply;
if(!s||!s.sensor||s.raw_volts===null||!(m>0)){$('cm').textContent=' Needs the sensor switched on, a reading and a meter value.';return}
const f=m/s.raw_volts;$('cm').textContent=' The bridge reads '+s.raw_volts.toFixed(2)+' V uncalibrated.';
if(f<0.9||f>1.1){$('cm').textContent+=' Factor '+f.toFixed(3)+' is outside 0.900 to 1.100: check the divider and the meter.';return}
$('k_vc').value=f.toFixed(3);$('cm').textContent+=' Factor set; save to apply.'})}
function ctc(){const b=$('ctc');b.hidden=false;b.textContent='Loading';fetch('/api/status',{cache:'no-store'}).then(r=>r.json()).then(d=>{const ct=d.chip_temp,f=ct&&ct.boot_samples&&ct.boot_samples[0];
if(!ct||!ct.power_on){b.innerHTML='<p class="def">Needs a start from power off. Switch the bridge off for at least 30 minutes, switch it on, then calibrate.</p>';return}
if(!f||f.celsius===null){b.innerHTML='<p class="def">The sensor gave no reading at this start: calibration is not possible.</p>';return}
b.innerHTML='<p class="def">Earliest reading of this start: <b>'+(f.celsius+ct.offset).toFixed(1)+' °C</b> uncorrected, '+(f.ms/1000).toFixed(2)+' s after power-on.</p>'+
'<label><input type="checkbox" id="cold" style="display:inline;width:auto"> The bridge was off for at least 30 minutes, in the same place as now.</label>'+
'<label for="room">Room temperature now, °C</label><input id="room" inputmode="decimal">'+
'<p class="def">Offset = earliest reading − room temperature, accurate to about ±5 °C. Calibrating restarts the bridge.</p>'+
'<button class="main" type="button" onclick="ctgo()">Calibrate and restart</button><div class="err" id="cte"></div>'}).catch(()=>{b.textContent='The bridge did not answer. Try again.'})}
function ctgo(){const b=new URLSearchParams();b.append('room',$('room').value);if($('cold').checked)b.append('cold','on');$('cte').textContent='';
post('/api/chip-temp/calibrate',b).then(async r=>{if(r.status===401)throw Error('Settings password required. Reopen this page and sign in.');const d=await r.json().catch(()=>({}));if(!d.ok)throw Error(d.message||'The bridge could not calibrate.');return d})
.then(d=>{$('fm').hidden=true;$('msg').textContent='Offset '+d.offset.toFixed(1)+' °C saved. '+(d.trial?'The bridge restarts now. Reconnect, open this page again and press Keep within 10 minutes.':'The bridge restarts now; the status page opens in 20 seconds.');if(!d.trial)setTimeout(()=>location.href='/',20000)})
.catch(e=>{$('cte').textContent=e instanceof TypeError?'The bridge did not answer. Check the Wi-Fi connection and try again.':e.message})}
function preset(n){const p=PROF[$('k_p'+n).value];if(p){$('k_b'+n).value=p.baud;$('k_f'+n).value=p.frame}ohints()}
function manual(n){const s=$('k_p'+n),p=s&&PROF[s.value];if(p&&($('k_b'+n).value!==p.baud||$('k_f'+n).value!==p.frame))s.value='custom';ohints()}
function load(){fetch('/api/settings',{cache:'no-store'}).then(r=>r.json()).then(d=>{items=d.items;PORTS=d.ports||[];for(const p of d.profiles||[]){PROF[p.id]=p;NAME[p.id]=p.name}const g={},o=[];
for(const i of d.items){const k=i.group||i.port||'Network';if(!g[k]){g[k]=[];o.push(k)}g[k].push(i)}
$('fs').innerHTML=o.map(k=>{const c=g[k].filter(i=>i.origin==='changed').length;return '<details class="sec" id="'+sid(k)+'"><summary>'+(ICON[k]||'')+' '+esc(k)+(c?'<span class="badge">'+c+' changed</span>':'')+'<span class="badge e" hidden>check</span></summary><div class="body"><div class="f'+(k==='Network'||k==='Hotspot'?' w':'')+'">'+g[k].map(field).join('')+'</div></div></details>'}).join('');
groups=o;menu();ohints();if(location.hash)show(decodeURIComponent(location.hash.slice(1)));
if(d.trial&&d.trial.active){tr=d.trial;$('trial').hidden=false;$('tt').textContent='These Wi-Fi or port settings are on trial. Keep them within '+Math.ceil(d.trial.seconds_left/60)+' min, otherwise the previous settings come back.'}})}
function post(u,body){return fetch(u,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:body||''})}
const scanUrl='/api/nav-scan', suggestions=[];
let scanId=0;
async function scanReply(response){
    if(!response.ok)throw Error(response.status===409?await response.text():'Bridge did not accept the scan request.');
    return response.json();
}
function displayScan(r){
    const n=r.port, labels=[];
    if(r.zander>=3)labels.push('Zander ('+r.zander+')');
    if(r.flarm>=3)labels.push('FLARM ('+r.flarm+')');
    if(!labels.length)labels.push('Generic NMEA ('+r.generic+')');
    const messages={listening:r.baud?'Listening at '+r.baud+' baud, '+r.frame+'…':'Starting scan…',
        measuring:'Measuring the incoming bit rate…',found:labels.join(' + ')+' · '+r.baud+' baud · read using '+r.frame,
        quiet:'No signal observed.',undecoded:'Activity observed, decoding unresolved.',cancelled:'Scan cancelled.'};
    $('scan'+n).textContent=messages[r.state];
    if(r.state==='found'){suggestions[n]=r;$('use'+n).hidden=false}
    return r.state==='listening'||r.state==='measuring';
}
async function startScan(n){
    const buttons=document.querySelectorAll('.detect');
    buttons.forEach(b=>b.disabled=true);
    $('use'+n).hidden=true;
    $('scan'+n).textContent='Starting scan…';
    try{
        let r=await scanReply(await post(scanUrl,'port='+n));
        scanId=r.id;
        $('cancel'+n).hidden=false;
        while(displayScan(r)){
            await new Promise(resolve=>setTimeout(resolve,500));
            r=await scanReply(await fetch(scanUrl,{cache:'no-store'}));
            if(r.id!==scanId||r.port!==n)throw Error('This scan has been replaced.');
        }
    }catch(e){$('scan'+n).textContent=e.message+' Any running scan ends automatically.'}
    finally{scanId=0;$('cancel'+n).hidden=true;buttons.forEach(b=>b.disabled=false)}
}
async function cancelScan(){
    try{await scanReply(await post(scanUrl,'cancel='+scanId))}
    catch(e){$('msg').textContent=e.message}
}
function useScan(n){
    const r=suggestions[n];
    const id=r.zander>=3 && r.flarm<3?'zs1':r.flarm>=3 && r.zander<3?'flarm':'custom';
    const p=PROF[id];
    $('k_p'+n).value=p && String(p.baud)===String(r.baud) && p.frame===r.frame?id:'custom';
    $('k_b'+n).value=r.baud;
    $('k_f'+n).value=r.frame;
    $('use'+n).hidden=true;
    $('scan'+n).textContent+=' · Added to form. Save and restart to apply.';
}
async function settingsReply(r){
    if(r.status===401)throw Error('Settings password required. Reopen this page and sign in.');
    const d=await r.json().catch(()=>({}));
    if((r.ok&&d.ok===true)||(r.status===400&&d.errors))return d;
    throw Error(d.message||'The bridge could not update settings.');
}
function settingsError(e){$('msg').textContent=e instanceof TypeError?'The bridge did not answer. Check the Wi-Fi connection and try again.':e.message}
function save(e){e.preventDefault();document.querySelectorAll('.err').forEach(x=>x.textContent='');document.querySelectorAll('.badge.e').forEach(x=>x.hidden=true);
const b=new URLSearchParams();for(const i of items){const el=$('k_'+i.key);if(el&&!el.disabled)b.append(i.key,el.value)}
post('/api/settings',b).then(settingsReply).then(d=>{if(!d.ok){for(const k in d.errors){const x=$('e_'+k);if(!x)continue;x.textContent=d.errors[k];const s=x.closest('details');if(s){s.open=true;s.querySelector('.badge.e').hidden=false}}mark();$('msg').textContent='Not saved. Correct the fields in the sections marked check.';return}
$('fm').hidden=true;$('msg').textContent=d.trial?'Saved. The bridge restarts now. Reconnect to its Wi-Fi, open this page again and press Keep within 10 minutes.':'Saved. The bridge restarts now; the status page opens in 20 seconds.';
if(!d.trial)setTimeout(()=>location.href='/',20000)}).catch(settingsError);return false}
function keep(){post('/api/keep').then(settingsReply).then(()=>{$('trial').hidden=true;$('msg').textContent='Kept. These settings stay.'}).catch(settingsError)}
function reset(){if(!confirm('Forget all saved settings and restart with the firmware defaults?'))return;
post('/api/reset').then(settingsReply).then(()=>{$('fm').hidden=true;$('msg').textContent='Reset. The bridge restarts with the firmware defaults; the status page opens in 20 seconds.';setTimeout(()=>location.href='/',20000)}).catch(settingsError)}
let tr=null;
function restart(){if(!confirm(tr?(tr.starts_left>0?'Restart the bridge? The settings stay on trial and the 10 minutes start again. Restarts left before the previous settings come back: '+(tr.starts_left-1)+'.':'Restart the bridge? The settings on trial are not kept: the previous settings come back.'):'Restart the bridge now? Apps reconnect on their own.'))return;
post('/api/restart').then(settingsReply).then(()=>{$('fm').hidden=true;$('s_Restart').hidden=true;$('msg').textContent='The bridge restarts now; the status page opens in 20 seconds.';setTimeout(()=>location.href='/',20000)}).catch(settingsError)}
function lreset(){if(!confirm('Start a new lifetime record? Operating time, starts and the extremes start again from zero.'))return;
post('/api/lifetime/reset').then(r=>r.json()).then(()=>{$('msg').textContent='Lifetime record reset.'}).catch(()=>{$('msg').textContent='The bridge did not answer. Try again.'})}
function sclear(){if(!confirm('Delete all saved errors and warnings?'))return;
post('/api/saved/clear').then(r=>r.json()).then(()=>{$('msg').textContent='Saved log cleared.'}).catch(()=>{$('msg').textContent='The bridge did not answer. Try again.'})}
fetch('/api/status',{cache:'no-store'}).then(r=>r.json()).then(d=>{if(d.lifetime){$('s_Records').hidden=false;if(groups.length)menu()}if(d.saved_log&&d.saved_log.ready)$('sclr').hidden=false}).catch(()=>{});
load();
</script>
)HTML";

static void settingsRoutes() {
    statusServer.on("/api/nav-scan", HTTP_GET, handleNavScan);
    statusServer.on("/api/nav-scan", HTTP_POST, handleNavScan);
    statusServer.on("/settings", HTTP_GET, []() {
        if (!settingsAuth()) return;
        statusServer.send_P(200, "text/html", kSettingsPage);
    });
    statusServer.on("/api/settings", HTTP_POST, handleSettingsSave);
    statusServer.on("/api/keep", HTTP_POST, handleSettingsKeep);
    statusServer.on("/api/reset", HTTP_POST, handleSettingsReset);
    statusServer.on("/api/restart", HTTP_POST, handleRestart);
#ifdef CHIP_TEMP
    statusServer.on("/api/chip-temp/calibrate", HTTP_POST, handleChipCalibrate);
#endif
#ifdef LIFETIME_RECORD
    statusServer.on("/api/lifetime/reset", HTTP_POST, []() {
        if (!settingsAuth()) return;
        lifetimeReset();
        statusServer.send(200, "application/json", "{\"ok\":true}");
    });
#endif
#ifdef SAVED_LOG
    statusServer.on("/api/saved/clear", HTTP_POST, []() {
        if (!settingsAuth()) return;
        savedClear();
        statusServer.send(200, "application/json", "{\"ok\":true}");
    });
#endif
}

#else

static inline void settingsRoutes() {}
static inline void settingsTrialCheck() {}

#endif
