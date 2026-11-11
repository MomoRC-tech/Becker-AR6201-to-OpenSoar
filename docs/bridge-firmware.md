# Bridge firmware candidates

No firmware is selected ([ADR-02](architecture.md#adr-02--implementation-and-reuse-baseline)). Candidate order: Yuri, then the basic AlphaLima sketch. Required of any candidate: bounded and checked forwarding, isolated diagnostics, exclusive command ownership, navigation coexistence.

Source review, 2026-11-08. Candidate builds and hardware tests are pending. Review file-level licenses before reuse.

[Yuri review](reviews/2026-11-08-yuri-bridge.md) · [Becker driver research](becker-driver.md) · [Sources](sources.md)

## Candidates

| Candidate | Source findings | Limits |
| --- | --- | --- |
| [Yuri 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) | First external bridge candidate; bounded reads, per-port routing; MIT | Checked writes, ownership, diagnostics, build pinning; [review](reviews/2026-11-08-yuri-bridge.md) |
| [AlphaLima basic sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) | Separate TCP/UART paths; counted writes; no NMEA parser | Unconditional UART banners; drains beyond 1,023-byte capacity; unchecked writes; multiple writers; Bluetooth broadcasts |
| [Basic config](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) | 3 UARTs, TCP 8880–8882, 19200/8N1, AP/debug/OTA | Examples, not installed settings |
| [Basic license](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE) | MIT | Preserve notices |
| [Pro bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) | Persistent config/web/filtering/routes; counted UART chunks with filters off | TCP drains beyond 126 bytes; debug copy; unchecked writes; filters alter binary traffic; cross-port exclusion required |
| Pro licensing/build | No standalone license found despite README reference | Permission, web/SPIFFS dependencies and reproducibility unresolved |
| Installed bridge | Reported working navigation | Firmware/routing/returns unknown; preserve as backup |

## Alternatives

| Project | Evidence and possible use | Remaining checks |
| --- | --- | --- |
| [DroneBridge ESP32](https://github.com/DroneBridge/ESP32/tree/25b3d9c60ca4cb958ad7156a742436c6a4361475) | README documents bidirectional transparent mode, TCP port 5760, configurable flush timeout, and host regression checks. Apache-2.0 metadata. Revision dated 2026-10-26. | Dedicated-radio bridge alternative. Confirm transparent TCP behavior and independent multi-UART capability. Forwarding code not audited here. |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2) | ESP32 gliding instrument with serial/Wi-Fi routing. Router.cpp contains message filtering and queue routing. GPL-2.0 metadata. Revision dated 2026-10-18. | Useful coexistence reference. Whole-firmware adoption expands scope, and Becker binary transparency is unverified. |

## Verification

Pinned Yuri, AlphaLima and XCVario paths, configuration and licenses inspected. DroneBridge documentation reviewed. Search was limited to the listed candidates. No bridge execution.
