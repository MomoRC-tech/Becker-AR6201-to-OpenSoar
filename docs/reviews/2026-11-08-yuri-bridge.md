# Yuri bridge source evaluation

2026-11-08. Authorized bounded WP3 source analysis; no firmware selection, implementation, build, flashing or hardware work. Requirements: R02/R03/R07/R08; R01/R04 remain OpenSoar/protocol responsibilities. Evidence below is **rechecked source** unless marked proposed or inferred.

## Revisions and maintenance

- Yuri default-branch head: [130f731416d43c89bce5626ef989b3e65918c9ce](https://github.com/yuri-rage/ESP-Serial-Bridge/commit/130f731416d43c89bce5626ef989b3e65918c9ce), 2024-08-21, merge of UDP fixes. Functional changes were authored in 2023; the 2024 merge is not a new TCP rewrite.
- AlphaLima basic default-branch head: [bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf](https://github.com/AlphaLima/ESP32-Serial-Bridge/commit/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf), 2026-07-28, README update.
- Both histories include f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe (2020-06-10). The [AlphaLima comparison](https://github.com/AlphaLima/ESP32-Serial-Bridge/compare/f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe...bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) contains six later commits and only README changes. The [Yuri comparison](https://github.com/yuri-rage/ESP-Serial-Bridge/compare/f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe...130f731416d43c89bce5626ef989b3e65918c9ce) contains ten later commits, including sketch/config changes and an additional client sketch.

Conclusion: AlphaLima has the newer repository timestamp; Yuri has later functional development and relevant fixes compared with basic AlphaLima. Yuri does not contain the later upstream documentation commits. Neither timestamp proves ongoing maintenance or modern toolchain compatibility. AlphaLima Pro is a separate project and is outside this comparison.

## Source findings

Source keys, pinned throughout this review:

- [Y sketch](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/ESP-Serial-Bridge.ino): globals, setup(), loop(), WiFiStationDisconnected().
- [Y config](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/config.h).
- [A sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino): setup(), loop().

| Area | Observation | Consequence / limit |
| --- | --- | --- |
| Binary forwarding | Y loop uses counted byte writes; TCP reads stop at 1,023 bytes; UART reads request at most 1,024 bytes. A loop continues draining after its index saturates. | Yuri removes the inspected drain-and-drop behavior in both directions. This does not establish loss-free operation under sustained load. |
| Routing | Separate server/client/buffer entries per UART; no NMEA parsing in TCP paths. Bluetooth selects one UART in both directions. | Suitable structure for a dedicated radio endpoint. Installed navigation routing may differ. |
| Writers | Y config enables TCP and UDP together, four TCP clients per UART. UDP setup callbacks write directly to each UART; Bluetooth can add a writer. | Multiple streams can reach one radio. A single TCP owner must be enforced specifically for the radio. |
| Partial writes | Y loop ignores UART/TCP write results and resets/reuses buffers. | Inference: a short write loses the unsent suffix; no application-level retry state exists. A successful write is not radio acceptance. |
| Scheduling | All ports and clients share loop(); synchronous writes occur sequentially. | Inference: a slow destination may delay servicing other UARTs. Actual blocking semantics/time bounds require the selected framework source and later measurement. |
| Diagnostics | Y setup routes ordinary startup messages through debug, unlike A's unconditional per-UART server banners. Y DEBUG maps to Serial/COM0. OTA callbacks still write directly to Serial. | Disabling DEBUG alone is insufficient when OTA is enabled. Boot-ROM/framework output remains board/toolchain dependent and was not inspected. |
| Reconnect | Y station setup and disconnect callback wait in while loops until Wi-Fi connects. | Setup cannot reach forwarding while disconnected. Callback scheduling impact is framework dependent; this is not proof that the main loop always blocks. Bounded reconnect handling is needed if station mode is retained. |
| Session boundaries | No radio-session state or pending-byte ownership; UART input is consumed even without a TCP receiver. | No offline status replay is provided. Reconnect must invalidate app state; future queues must not replay old commands to a new session. |
| Configuration | Y defaults to station mode and 115200 baud on all UARTs; TCP, UDP, DEBUG and OTA enabled. | These are upstream examples, not installation settings. Blind adoption could interrupt navigation. |

The [README](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/README.md) reports Arduino framework 2.0 and PlatformIO compatibility. The complete pinned tree contains no platformio.ini or dependency lock. Build reproducibility is therefore still work to specify, not established support for a current framework. The [LICENSE](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/LICENSE) is MIT with AlphaLima attribution; inspected sketch/config headers contain no different license grant. Preserve notices if code is later reused. No code was copied in this work.

## Smallest proposed changes

These are analysis outputs, not implementation authorization. A precise patch size depends on WP1. Retain existing navigation pins, settings, endpoints, client behavior and required transports; do not set MAX_NMEA_CLIENTS to one globally or disable a navigation transport merely to isolate the radio.

| Increment | Minimal proposal and expected result | Future regression evidence |
| --- | --- | --- |
| 1. Configuration and isolation (R02/R03/R06) | Assign one verified spare UART/endpoint to radio. Exclude it from UDP and Bluetooth input/output; use per-port exclusions if navigation needs those features. Suppress/reroute application diagnostics and OTA output away from every equipment UART; verify boot output separately. Preserve installed Wi-Fi mode and navigation settings. | Exact bytes per endpoint; attempted UDP/BT injection cannot reach radio; startup/OTA diagnostics absent from equipment links. |
| 2. Radio connection ownership (R03) | Accept one radio TCP connection, reject additional clients without disrupting the owner; retain navigation client capacity. Release ownership and pending session data on disconnect. | Competing client, disconnect/reconnect, and stale pending-command cases. |
| 3. Checked bounded forwarding (R02/R03) | Track pending offsets in both radio directions; advance only by accepted write count. Stop reading TCP when capacity is exhausted. Limit work per loop pass and use bounded writes so navigation is serviced. If existing navigation writes can block the same loop, bound those calls too while preserving routing. Define overflow/stall failure to close the radio session and report locally outside binary traffic; never silently drop bytes and continue as if valid. | Short/zero writes, oversized bursts, all byte values, slow readers, concurrent navigation and repeated disconnects. Time/queue limits must come from WP1/WP2, not invented defaults. |
| 4. Conditional station reconnect (R02/R03) | If the installation uses station mode, replace unbounded connection waits with timed retry state and invalidate radio sessions on link loss. If it uses AP mode, leave the unused station path out of the initial change. | Wi-Fi absent at startup and loss/recovery while servicing UARTs; no command replay. |
| 5. Reproducible configuration (R07) | Pin board/framework/dependencies and sanitized build configuration. Do not upgrade the framework merely because it is newer. | Later reproducible build on the selected environment; no build performed now. |

Finite buffers cannot preserve unlimited serial input during a network outage. Choose explicit overflow/session failure behavior and refresh radio state on reconnect. Keep Becker parsing, acknowledgment and active/standby decisions in OpenSoar; a bridge cannot fix the known driver defects or protect active frequency through byte forwarding alone.

Recommendation: retain Yuri as the first external candidate, because its existing bounded reads and routing reduce necessary changes. Before choosing it over the installed firmware, obtain the sanitized current configuration/source and spare-UART evidence. No navigation-preservation claim is possible without that comparison and later coexistence tests.

## Verification summary

Re-read both pinned sketches, Yuri config/README/LICENSE and recursive tree through GitHub; checked default-branch commit histories and both comparisons from the shared revision. Confirmed project remote HEAD matched local 3d1c69e before documentation edits. Reviewed documentation diff, links and whitespace. Source analysis only: no compilation, synthetic transport tests, board execution, radio commands or navigation measurements. Framework write blocking, boot output, installed topology, radio revision and license choice for this repository remain unresolved. WP3 selection remains open.
