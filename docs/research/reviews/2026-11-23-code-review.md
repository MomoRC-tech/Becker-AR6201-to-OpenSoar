# Bridge firmware 0.5.9 and test tools: code review

S176, 2026-11-23 · WP6 · firmware 0.5.9 and `tools/` at `5edfd0e`, Arduino-ESP32 3.3.12

Three parallel static reviews covered the data path and concurrency, persistence and the web interface, and the test tools. The main findings were then checked against the source. Fixes for findings 1–3 are in firmware 0.5.10 (S177).

> [!NOTE]
> Source reading only, plus offline checks: `py_compile` on all tools and the acceptance script's unit tests. No test was run on the bridge for this review.

---

## Result

| # | Finding | Severity | State |
| :-: | --- | --- | --- |
| 1 | Unauthenticated flash writes: a wrong-password request logs a Warning, and the public `/api/saved` flushed it to LittleFS without the 10 s limit (`savedlog.h:195`, `settings_page.h:23`). Each flash write stalls the UART receive interrupt (S133) | Medium | Fixed in 0.5.10 |
| 2 | UART receive errors on the radio line were only counted; the radio session stayed open, so a damaged reply reached the app (`bridge.ino` `serviceRadioPort`) | Medium | Fixed in 0.5.10 |
| 3 | A lagging navigation app got the start of one sentence joined to the end of another: the queue took the part of a read that fitted (`queueForApp`). Sentences without a checksum would be parsed with wrong fields | Medium | Fixed in 0.5.10 |
| 4 | Settings POST routes rely on basic auth only, with no Origin check or custom header: a page from another origin in the same browser can reset the bridge (`settings_page.h:362-377`) | Medium | Open |
| 5 | The settings login is the OTA password over plain HTTP: one captured login allows a firmware upload | Medium | Open |
| 6 | Flash writes leave the UART FIFO only 8 bytes of margin (`rxfifo_full_thrhd` 120 of 128); a sector erase during a burst can lose bytes on all ports | Medium (bench check) | Open |
| 7 | ROM and panic output (brownout, backtrace) still reach UART0, the Zander transmit pin; the web log redirect catches only ESP_LOG | Low | Open |
| 8 | A navigation scan holds that port's apps for up to 56 s, then replays queued data and late commands | Low | Open |
| 9 | Low items: unauthenticated `/api/log?clock=`; a settings record of another version blocks saves with a misleading message; a value equal to the default follows later default changes; port-clash errors marked on the other field; lifetime reset and saved-log clear report success unchecked; a corrupt saved log is formatted silently; OTA loader never falls back to the hotspot after losing the network | Low | Open |

Earlier review S138: finding 3 (rollback) is fixed in code; findings 1 (blocking UART write), 2 (closed socket kept until slot reuse) and 4 (socket limit) are unchanged.

---

## Test tools

| # | Finding | State |
| :-: | --- | --- |
| T1 | Bare-board slow-app verdict depended only on `--seconds`: below 55 s it passed without the app falling behind; above, any close passed, including a restart, and the check's own read could count as progress | Fixed: needs short writes, waits past the stall limit, checks for a restart |
| T2 | Radio probe was a fixed 4-byte frame at about 8 B/s; nothing checked for bytes after the final echo | Fixed: random 1–64-byte bursts with every byte value, trailing-byte check |
| T3 | The wired hour run did not watch restarts, memory or the supply port | Fixed: log read every minute (restart, lowest free memory); `--supply` reads port 4353 |
| T4 | `settings_test.py` left changed settings (port 10111) after a failed check | Fixed: always resets |
| T5 | No automated check for a stalled radio owner, stale bytes for the next owner or refused-client load | Added `tools/radio_robustness_test.py` |
| T6 | Duplicated helpers across scripts, bare `assert` verdicts, hard-coded ports, no CI | Open |

---

## Missing checks

- Run the 0.5.10 acceptance hour run on the wired bridge.
- Radio owner change: bytes the previous owner sent may still be on the UART when the next owner is accepted, so their echo or the radio's reply can reach the next owner. Measured on 0.5.10: 113 and 119 bytes (S177).
- Flash-write stall (finding 6): log many warnings during a FLARM loop run and watch the UART error counter.
