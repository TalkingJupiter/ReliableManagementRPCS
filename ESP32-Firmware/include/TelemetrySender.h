#pragma once

#include <Arduino.h>

// Lightweight UDP sender for telemetry JSON payloads over a W5500.
// (No ArduinoJson dependency; we send a pre-built JSON string.)

class TelemetrySender {
public:
  bool begin();
  bool isUp() const;
  bool sendUDP(const char* jsonPayload);

  // Formats ESP32 base MAC (EFUSE) as "AA:BB:CC:DD:EE:FF".
  static String deviceMacString();
};
