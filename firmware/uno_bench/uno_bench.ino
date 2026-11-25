#include <AltSoftSerial.h>

constexpr uint8_t relayPin = 7;
// Match the approved module and use an external bias to hold it idle at reset.
constexpr uint8_t relayActive = HIGH;
constexpr uint8_t relayIdle = relayActive == HIGH ? LOW : HIGH;
constexpr unsigned long cutMs = 5000;
constexpr uint8_t endByte = 0xc0;
constexpr uint8_t escapeByte = 0xdb;

AltSoftSerial feed;  // Uno TX on D9, RX on D8
uint8_t frame[65];   // Type byte and up to 64 payload bytes
uint8_t frameSize = 0;
uint8_t feedOffset = 0;
uint8_t feedEnd = 0;
bool receiving = false;
bool escaped = false;
bool invalid = false;
bool ready = false;
bool cycling = false;
unsigned long cutStarted = 0;

void handleFrame() {
  if (frameSize == 5 && memcmp(frame, "UNO/1", 5) == 0) {
    ready = true;
    Serial.println(F("UNO/1 READY"));
  } else if (ready && frameSize > 1 && frame[0] == 'D') {
    feedOffset = 1;
    feedEnd = frameSize;
  } else if (ready && frameSize == 5 && memcmp(frame, "CYCLE", 5) == 0) {
    if (cycling) {
      Serial.println(F("BUSY"));
      return;
    }
    digitalWrite(relayPin, relayActive);
    cutStarted = millis();
    cycling = true;
    Serial.println(F("CUT"));
  } else {
    Serial.println(F("ERR"));
  }
}

void receiveByte(uint8_t value) {
  if (value == endByte) {
    if (invalid || escaped) {
      Serial.println(F("ERR"));
    } else if (frameSize != 0) {
      handleFrame();
    }
    receiving = true;
    escaped = invalid = false;
    frameSize = 0;
    return;
  }
  if (!receiving || invalid) {
    return;
  }
  if (escaped) {
    escaped = false;
    if (value == 0xdc) {
      value = endByte;
    } else if (value == 0xdd) {
      value = escapeByte;
    } else {
      invalid = true;
      return;
    }
  } else if (value == escapeByte) {
    escaped = true;
    return;
  }
  if (frameSize == sizeof(frame)) {
    invalid = true;
    return;
  }
  frame[frameSize++] = value;
}

void setup() {
  digitalWrite(relayPin, relayIdle);
  pinMode(relayPin, OUTPUT);
  feed.begin(19200);
  Serial.begin(115200);
}

void loop() {
  if (cycling && millis() - cutStarted >= cutMs) {
    digitalWrite(relayPin, relayIdle);
    cycling = false;
    Serial.println(F("RESTORED"));
  }
  if (feedOffset < feedEnd) {
    // If the AltSoftSerial queue is full, each write waits for at most
    // one byte time. Check the relay timer again before queuing the next byte.
    feed.write(frame[feedOffset++]);
    if (feedOffset == feedEnd) {
      Serial.println(F("QUEUED"));
    }
  } else if (Serial.available()) {
    receiveByte(Serial.read());
  }
}
