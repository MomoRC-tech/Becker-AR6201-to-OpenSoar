// OTA loader for the Wi-Fi bridge bench prototype.
//
// Flash once over USB with the "Minimal SPIFFS (1.9MB APP with OTA)"
// partition scheme. Afterwards every firmware image can be uploaded over
// Wi-Fi. Design: docs/design/architecture.md#adr-05
//
// Start-up:
//   1. Join the configured Wi-Fi network for up to 15 s.
//   2. If that fails, open the password-protected hotspot "WiFi-BridgeTom".
//   OTA uploads are accepted in both modes until the next restart.
//
// The loader configures no GPIO and no UART other than the boot console
// (UART0 TX on GPIO1), so the instrument interfaces stay idle.
//
// Build variants (compiler flag; see README):
//   (none)          Private build: network + hotspot, passwords from credentials.h.
//   -DHOTSPOT_ONLY  Public build: hotspot only, documented default passwords,
//                   no credentials.h needed. Safe to publish.
//   -DFALLBACK_TEST Temporary test build: joins a network that does not exist,
//                   so the 15 s timeout and hotspot fallback run. Without an
//                   OTA upload it boots the previous app slot after 5 min.

#include <WiFi.h>
#include <ArduinoOTA.h>

#if defined(HOTSPOT_ONLY) && defined(FALLBACK_TEST)
#error "HOTSPOT_ONLY and FALLBACK_TEST are separate builds."
#endif

#if defined(HOTSPOT_ONLY)
// Public defaults, documented in the README. Anyone with the repository can
// use them: replace this build with a private one outside the bench.
#define HOTSPOT_PASSWORD "BridgeTom-setup"
#define OTA_PASSWORD "BridgeTom-ota"
#elif __has_include("credentials.h")
#include "credentials.h"
#else
#error "Copy credentials.example.h to credentials.h and fill in the passwords."
#endif

#if defined(FALLBACK_TEST)
#include <esp_ota_ops.h>
#undef WIFI_SSID
#define WIFI_SSID "bridgetom-fallback-test-no-network"
static constexpr unsigned long kRevertAfterMs = 5UL * 60UL * 1000UL;
#endif

static bool otaStarted = false;

static constexpr char kHostname[] = "wifi-bridgetom";
static constexpr char kHotspotSsid[] = "WiFi-BridgeTom";
static constexpr unsigned long kJoinTimeoutMs = 15000;

static_assert(sizeof(HOTSPOT_PASSWORD) - 1 >= 8, "HOTSPOT_PASSWORD needs at least 8 characters (WPA2).");
static_assert(sizeof(OTA_PASSWORD) - 1 >= 8, "OTA_PASSWORD needs at least 8 characters.");

#if !defined(HOTSPOT_ONLY)
static bool joinNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Joining \"%s\" for up to %lu s\n", WIFI_SSID, kJoinTimeoutMs / 1000);

  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start >= kJoinTimeoutMs) {
      return false;
    }
    delay(250);
  }
  Serial.printf("Joined, IP %s\n", WiFi.localIP().toString().c_str());
  return true;
}
#endif

static bool openHotspot() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(kHotspotSsid, HOTSPOT_PASSWORD)) {
    return false;
  }
  Serial.printf("Hotspot \"%s\" open, IP %s\n", kHotspotSsid, WiFi.softAPIP().toString().c_str());
  return true;
}

static void startOta() {
  ArduinoOTA.setHostname(kHostname);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    otaStarted = true;
    Serial.println("OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA done, restarting");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA error %u\n", static_cast<unsigned>(error));
  });
  ArduinoOTA.begin();
  Serial.printf("OTA ready as %s.local\n", kHostname);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Wi-Fi bridge OTA loader");
#if defined(HOTSPOT_ONLY)
  Serial.println("Build: hotspot only");
#elif defined(FALLBACK_TEST)
  Serial.println("Build: FALLBACK TEST (temporary)");
#endif

  WiFi.setHostname(kHostname);
#if defined(HOTSPOT_ONLY)
  const bool joined = false;
#else
  const bool joined = joinNetwork();
  if (!joined) {
    Serial.println("Network not reached");
  }
#endif
  if (!joined && !openHotspot()) {
    Serial.println("Hotspot failed, restarting in 5 s");
    delay(5000);
    ESP.restart();
  }
  startOta();
}

#if defined(FALLBACK_TEST)
// Boot the other app slot (the previously installed loader) if no OTA upload
// arrived, so the test build never stays installed by accident.
static void revertIfIdle() {
  if (otaStarted || millis() < kRevertAfterMs) {
    return;
  }
  const esp_partition_t *previous = esp_ota_get_next_update_partition(nullptr);
  if (previous != nullptr && esp_ota_set_boot_partition(previous) == ESP_OK) {
    Serial.println("Fallback test timed out, booting previous slot");
  } else {
    Serial.println("Fallback test timed out, previous slot invalid, restarting");
  }
  delay(500);
  ESP.restart();
}
#endif

void loop() {
  ArduinoOTA.handle();
#if defined(FALLBACK_TEST)
  revertIfIdle();
#endif
  delay(10);
}
