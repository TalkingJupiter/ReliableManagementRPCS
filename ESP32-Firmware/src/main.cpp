#include <Arduino.h>
#include "config.h"
#include "Heartbeat.h"
#include "TemperatureBus.h"

HardwareSerial HBSerial(HB_UART_NUM);
Heartbeat hb(HBSerial);
TemperatureBus tempBus;

static inline bool isControllerA() { return DEVICE_ID == 'A' || DEVICE_ID == 65; }
static inline bool isControllerB() { return DEVICE_ID == 'B' || DEVICE_ID == 66; }

static void printTemps() {
  Serial.printf(
    "[TEMP] inlet: %.2f %.2f %.2f | exhaust: %.2f %.2f %.2f\n",
    tempBus.intakeC(0), tempBus.intakeC(1), tempBus.intakeC(2),
    tempBus.exhaustC(0), tempBus.exhaustC(1), tempBus.exhaustC(2)
  );
}

static void maybePrintTemps() {
  if (tempBus.hasNewSample()) {
    tempBus.clearNewSampleFlag();
    printTemps();
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.printf("Booting Controller %c\n", (char)DEVICE_ID);

  // Start heartbeat UART link
  hb.begin(HB_UART_RX_PIN, HB_UART_TX_PIN, HB_UART_BAUD);

  // Start temperature buses (intake + exhaust)
  tempBus.begin(ONE_WIRE_BUS_COOL, ONE_WIRE_BUS_EXHAUST);

  Serial.println("Heartbeat + TemperatureBus started\n");

  // Optional sanity check (keep if you want)
  Serial.printf("[TEMP:init] intakeN=%u exhaustN=%u\n",
                tempBus.intakeDeviceCount(),
                tempBus.exhaustDeviceCount());
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

  // 3) Temperature sampling (tick exactly once per loop)
  tempBus.tick(now);

  // 4) Heartbeat status
  const bool peerAlive = hb.peerAlive(now, HB_TIMEOUT_MS);
  const uint32_t ageMs = (hb.lastRxMS() == 0) ? 0 : (now - hb.lastRxMS());

  // Controller A: prints HB status periodically + always prints temps when sampled
  if (isControllerA()) {
    static uint32_t lastStatus = 0;
    if ((uint32_t)(now - lastStatus) >= 1000) {
      lastStatus = now;
      Serial.printf("[HB:A] peer=%c, alive=%d, age_ms=%lu\n",
                    hb.peerId(),
                    peerAlive ? 1 : 0,
                    (unsigned long)ageMs);
    }

    maybePrintTemps();
  }

  // Controller B: prints temps only when A is unhealthy; logs transitions
  if (isControllerB()) {
    static bool lastHealthy = false;
    static bool everHealthy = false;

    // Your original policy: B considers "healthy" only if the peer is A and alive.
    const bool healthy = (hb.peerId() == 'A') && peerAlive;

    if (lastHealthy && !healthy) {
      Serial.printf("[ALERT:B] Lost heartbeat from A (timeout=%d ms). peer=%c age_ms=%lu\n",
                    (int)HB_TIMEOUT_MS, hb.peerId(), (unsigned long)ageMs);
      Serial.printf("[INFO:B] Trying to recover the bus communication...\n");
    }

    if (!lastHealthy && healthy) {
      if (everHealthy) {
        Serial.printf("[RECOVER:B] Heartbeat from A restored. age_ms=%lu\n",
                      (unsigned long)ageMs);
        Serial.printf("[INFO:B] Releasing the bus communication to device %c\n", hb.peerId());
      }
      everHealthy = true;
    }

    lastHealthy = healthy;

    // Print temps continuously while unhealthy (more useful than edge-only)
    if (!healthy) {
      maybePrintTemps();
    } else {
      // Keep this if you really want the spam; otherwise delete it.
      Serial.println("ESP A is healthy and responsible of communication");
    }
  }

  delay(1);
}
