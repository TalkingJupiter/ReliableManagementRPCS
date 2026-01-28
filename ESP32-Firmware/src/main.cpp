#include <Arduino.h>
#include "config.h"
#include "Heartbeat.h"
#include "RelayControl.h"
#include "RoleManager.h"

HardwareSerial HBSerial(HB_UART_NUM);

Heartbeat hb(HBSerial);
RelayControl relays;
RoleManager role(hb, relays, (char)DEVICE_ID);

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.printf("\nBooting Controller %c\n", (char)DEVICE_ID);

  hb.begin(HB_RX_PIN, HB_TX_PIN, HB_BAUD);

  relays.begin(RELAY_A_PIN, RELAY_B_PIN, RELAY_ACTIVE_LOW);

  role.begin();

  Serial.println("Heartbeat + Role Manager Started...");
}

void loop() {
  role.tick();

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 1000){
    Serial.printf("STATE=%d  peer=%c  peerAlive=%d  busOwner=%d\n", (int)role.state(), hb.peerId(), (int)hb.peerAlive(millis(), HB_TIMEOUT_MS), (int)relays.owner());
  }
  delay(5);
}


