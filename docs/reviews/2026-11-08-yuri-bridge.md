# Yuri bridge source evaluation

S08, 2026-11-08. R02/R03/R07/R08. Source analysis; firmware selection open.

## Revisions and maintenance

| Source | Review baseline |
| --- | --- |
| [Yuri 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/commit/130f731416d43c89bce5626ef989b3e65918c9ce) | 2024-08-21 merge; functional fixes authored 2023 |
| [AlphaLima bb42dc3](https://github.com/AlphaLima/ESP32-Serial-Bridge/commit/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) | 2026-07-28 README update |
| Shared `f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe` | 2020-06-10; later [AlphaLima changes](https://github.com/AlphaLima/ESP32-Serial-Bridge/compare/f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe...bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf): 6 documentation commits; [Yuri changes](https://github.com/yuri-rage/ESP-Serial-Bridge/compare/f6c6ada6aa9a437e4c4a2436d45f98551cd9d6fe...130f731416d43c89bce5626ef989b3e65918c9ce): 10 commits including code/config |

Yuri has later functional fixes; timestamps establish neither current maintenance nor toolchain compatibility.

---

## Source findings

Pinned sources: [Y sketch](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/ESP-Serial-Bridge.ino) (`setup`, `loop`, `WiFiStationDisconnected`); [Y config](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/config.h); [A sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) (`setup`, `loop`).

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

| Build/license evidence | Limit |
| --- | --- |
| [README](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/README.md): Arduino 2.0/PlatformIO reported | No platformio.ini/dependency lock in pinned tree; build unverified |
| [LICENSE](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/LICENSE): MIT, AlphaLima attribution | No conflicting sketch/config grant found; preserve notices |

---

## Smallest proposed changes

Proposed for the new bridge (ADR-03). Match navigation interfaces, settings and transports. Restrict radio clients separately.

| Increment | Minimal proposal and expected result | Future regression evidence |
| --- | --- | --- |
| 1. Configuration and isolation (R02/R03/R06) | Assign one verified spare UART/endpoint to radio. Exclude it from UDP and Bluetooth input/output; use per-port exclusions if navigation needs those features. Suppress/reroute application diagnostics and OTA output away from every equipment UART; verify boot output separately. Preserve installed Wi-Fi mode and navigation settings. | Exact bytes per endpoint; attempted UDP/BT injection cannot reach radio; startup/OTA diagnostics absent from equipment links. |
| 2. Radio connection ownership (R03) | Accept one radio TCP connection, reject additional clients without disrupting the owner; retain navigation client capacity. Release ownership and pending session data on disconnect. | Competing client, disconnect/reconnect, and stale pending-command cases. |
| 3. Checked bounded forwarding (R02/R03) | Track pending offsets in both radio directions; advance only by accepted write count. Stop reading TCP when capacity is exhausted. Limit work per loop pass and use bounded writes so navigation is serviced. If existing navigation writes can block the same loop, bound those calls too while preserving routing. Define overflow/stall failure to close the radio session and report locally outside binary traffic; never silently drop bytes and continue as if valid. | Short/zero writes, oversized bursts, all byte values, slow readers, concurrent navigation and repeated disconnects. Time/queue limits must come from WP1/WP2, not invented defaults. |
| 4. Conditional station reconnect (R02/R03) | If the installation uses station mode, replace unbounded connection waits with timed retry state and invalidate radio sessions on link loss. If it uses AP mode, leave the unused station path out of the initial change. | Wi-Fi absent at startup and loss/recovery while servicing UARTs; no command replay. |
| 5. Reproducible configuration (R07) | Pin board/framework/dependencies and sanitized build configuration. Do not upgrade the framework merely because it is newer. | Later reproducible build on the selected environment; no build performed now. |

Finite buffers cannot cover unlimited outages. Define overflow/session failure; no old-command replay. OpenSoar owns protocol/state; forwarding alone cannot preserve active frequency.

---

## Verification summary

Pinned sketches/config/README/license/tree and histories inspected. No compilation, transport tests, board/radio/navigation measurements. Framework blocking, boot output, installed interfaces and firmware selection open.
