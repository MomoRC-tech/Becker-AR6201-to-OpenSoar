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

#include <WiFi.h>
#include <ArduinoOTA.h>

#if __has_include("credentials.h")
#include "credentials.h"
#else
#error "Copy credentials.example.h to credentials.h and fill in the passwords."
#endif

static constexpr char kHostname[] = "wifi-bridgetom";
static constexpr char kHotspotSsid[] = "WiFi-BridgeTom";
static constexpr unsigned long kJoinTimeoutMs = 15000;

static_assert(sizeof(HOTSPOT_PASSWORD) - 1 >= 8, "HOTSPOT_PASSWORD needs at least 8 characters (WPA2).");
static_assert(sizeof(OTA_PASSWORD) - 1 >= 8, "OTA_PASSWORD needs at least 8 characters.");

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

  WiFi.setHostname(kHostname);
  if (!joinNetwork()) {
    Serial.println("Network not reached");
    if (!openHotspot()) {
      Serial.println("Hotspot failed, restarting in 5 s");
      delay(5000);
      ESP.restart();
    }
  }
  startOta();
}

void loop() {
  ArduinoOTA.handle();
  delay(10);
}
