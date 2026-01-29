#include <Arduino.h>
#include "config.h"
#include "Heartbeat.h"


// -----------------------------
// Heartbeat objects
// -----------------------------
HardwareSerial HBSerial(HB_UART_NUM);
Heartbeat hb(HBSerial);

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("=== Heartbeat Monitor (using Heartbeat.{h,cpp}) ===");
  Serial.printf("Booting Controller %c\n", (char)DEVICE_ID);
  Serial.printf("UART%d baud=%d TX=%d RX=%d\n",
                HB_UART_NUM, HB_UART_BAUD, HB_UART_TX_PIN, HB_UART_RX_PIN);
  Serial.printf("HB_SEND_MS=%d  HB_TIMEOUT_MS=%d\n", (int)HB_SEND_MS, (int)HB_TIMEOUT_MS);

  hb.begin(HB_UART_RX_PIN, HB_UART_TX_PIN, HB_UART_BAUD);
}

void loop() {
  const uint32_t now = millis();

  // 1) Always parse RX
  hb.tick();

  // 2) Send heartbeat periodically
  static uint32_t lastSend = 0;
  if ((uint32_t)(now - lastSend) >= (uint32_t)HB_SEND_MS) {
    lastSend = now;
    hb.send((char)DEVICE_ID, now);
  }

  // 3) Print when we receive a new valid heartbeat (detected by lastRx change)
  static uint32_t lastSeenRx = 0;
  const uint32_t rxMs = hb.lastRxMS();
  if (rxMs != 0 && rxMs != lastSeenRx) {
    lastSeenRx = rxMs;
    // Serial.printf("[RX OK] peer=%c  age_ms=%lu\n", hb.peerId(), (unsigned long)(now - rxMs));
  }

  // 4) Status every 1 second
  static uint32_t lastStatus = 0;
  if ((uint32_t)(now - lastStatus) >= 30000) {
    lastStatus = now;

    const bool alive = hb.peerAlive(now, HB_TIMEOUT_MS);
    const uint32_t age = (hb.lastRxMS() == 0) ? 0 : (now - hb.lastRxMS());

    Serial.printf("[STATUS] me=%c peer=%c alive=%d age_ms=%lu\n",
                  (char)DEVICE_ID,
                  hb.peerId(),
                  alive ? 1 : 0,
                  (unsigned long)age);
  }

  delay(1);
}
