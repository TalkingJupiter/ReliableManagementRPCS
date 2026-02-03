#include "TelemetrySender.h"
#include "config.h"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#ifdef ESP32
  #include <esp_mac.h>
#endif

static EthernetUDP gUdp;
static bool gEthUp = false;

static IPAddress radxaIP() {
  return IPAddress(RADXA_IP_A, RADXA_IP_B, RADXA_IP_C, RADXA_IP_D);
}

String TelemetrySender::deviceMacString() {
  uint8_t mac[6] = {0};

  #ifdef ESP32
    // ESP32 "base MAC" is stable and unique per chip.
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
  #else
    // Fallback (won't be unique).
    mac[0] = 0xAA; mac[1] = 0xBB; mac[2] = 0xCC;
    mac[3] = 0xDD; mac[4] = 0xEE; mac[5] = 0xFF;
  #endif

  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

bool TelemetrySender::begin() {
  // Initialize SPI for W5500
  SPI.begin(W5500_SCK_PIN, W5500_MISO_PIN, W5500_MOSI_PIN, W5500_CS_PIN);
  Ethernet.init(W5500_CS_PIN);

  // W5500 requires a MAC for Ethernet.begin (even if we use DHCP).
  // We'll derive one from the ESP32 base MAC.
  uint8_t base[6] = {0};
  #ifdef ESP32
    esp_read_mac(base, ESP_MAC_WIFI_STA);
  #else
    base[0]=0xDE; base[1]=0xAD; base[2]=0xBE; base[3]=0xEF; base[4]=0x00; base[5]=0x01;
  #endif
  byte ethMac[6] = { base[0], base[1], base[2], base[3], base[4], base[5] };

  Serial.println("[NET] Initializing W5500...");

  // Try DHCP first.
  if (Ethernet.begin(ethMac) == 0) {
    Serial.println("[NET] DHCP failed (continuing, link might still be down).");
  }

  delay(200);

  if (Ethernet.linkStatus() == LinkON) {
    gEthUp = true;
    Serial.print("[NET] Link up. IP=");
    Serial.println(Ethernet.localIP());
  } else {
    gEthUp = false;
    Serial.println("[NET] Link DOWN (check cable/switch). Telemetry will not send.");
  }

  // UDP does not need bind, but begin() sets a local port.
  gUdp.begin(0);
  return gEthUp;
}

bool TelemetrySender::isUp() const {
  // Update link status opportunistically.
  gEthUp = (Ethernet.linkStatus() == LinkON);
  return gEthUp;
}

bool TelemetrySender::sendUDP(const char* jsonPayload) {
  if (!jsonPayload || !jsonPayload[0]) return false;
  if (!isUp()) return false;

  const IPAddress dst = radxaIP();

  if (gUdp.beginPacket(dst, (uint16_t)RADXA_UDP_PORT) != 1) {
    return false;
  }
  gUdp.write((const uint8_t*)jsonPayload, strlen(jsonPayload));
  return (gUdp.endPacket() == 1);
}
