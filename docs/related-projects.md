# Related projects

Source review, 2026-11-08. [Selection status](architecture.md#adr-02--implementation-and-reuse-baseline) · [Driver evidence (S11)](technical-investigation.md#becker-driver-follow-up--2026-11-08)

> [!NOTE]
> Candidate builds and hardware tests are pending. Review file-level licenses before reuse.

## Prior review shortlist

| Candidate / pinned source | Use | Gaps |
| --- | --- | --- |
| [LK8000 8d78c63](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp) | Commands/status, counted writes, CRC; [PR 941](https://github.com/LK8000/LK8000/pull/941), [PR 940](https://github.com/LK8000/LK8000/pull/940) | Shared parser/state defects; independent protocol validation |
| [Yuri 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) | First external bridge candidate; bounded reads, per-port routing; MIT | Checked writes, ownership, diagnostics, build pinning; [review](reviews/2026-11-08-yuri-bridge.md) |
| [RadioDriver a794c4a](https://github.com/rbp28668/RadioDriver/tree/a794c4a994838b94d2b0bfa69ef76cda9a698957) | Teensy/controller and Windows serial examples | Arbitrary startup frequencies; no validated simulator; incomplete license coverage |

---

## Other useful leads

| Project | Evidence and possible use | Remaining checks |
| --- | --- | --- |
| [DroneBridge ESP32](https://github.com/DroneBridge/ESP32/tree/25b3d9c60ca4cb958ad7156a742436c6a4361475) | README documents bidirectional transparent mode, TCP port 5760, configurable flush timeout, and host regression checks. Apache-2.0 metadata. Revision dated 2026-10-26. | Dedicated-radio bridge alternative. Confirm transparent TCP behavior and independent multi-UART capability. Forwarding code not audited here. |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2) | ESP32 gliding instrument with serial/Wi-Fi routing. Router.cpp contains message filtering and queue routing. GPL-2.0 metadata. Revision dated 2026-10-18. | Useful coexistence reference. Whole-firmware adoption expands scope, and Becker binary transparency is unverified. |
| [SD-Link AR620X](https://sdlink.de/wp-content/uploads/2026/02/SD-AR620X-E-NG_en_easyvfr_installation_v1.0.pdf) | Commercial BLE integration. Vendor documentation specifies serial-number and tandem-mode prerequisites for its adapter. | Compare prerequisites against the exact radio and manufacturer documentation before applying them here. |

---

## Historical XCSoar work

| Reference | Evidence / limit |
| --- | --- |
| [PR 189](https://github.com/XCSoar/XCSoar/pull/189), [190](https://github.com/XCSoar/XCSoar/pull/190), [449](https://github.com/XCSoar/XCSoar/pull/449) | Closed/unmerged at review. PR 189 reports AR6201 tuning/reconnect tests on Windows/UNIX; not current-build validation |
| [Issue 1092](https://github.com/XCSoar/XCSoar/issues/1092) | Reported eku60 success and protocol-publication obstacle; current policy unknown |

---

## Original verification and limitations

Pinned LK8000, Yuri, RadioDriver and XCVario paths inspected. DroneBridge documentation and PR metadata reviewed. Search was limited to the listed candidates and leads.
