#pragma once
#include <Arduino.h>

// ===============
// BUILD TIME CONFIG
// ==============

#ifdef DEVICE_ID
#define DEVICE_ID 'A'
#endif

#ifdef HB_UART_NUM
#define HB_UART_NUM 1
#endif

#ifdef HB_RX_PIN
#define HB_RX_PIN 16
#endif

#ifdef HB_TX_PIN
#define HB_TX_PIN 17
#endif

#ifdef HB_BAUD
#define HB_BAUD 115200
#endif

#ifdef HB_SEND_MS
#define HB_SEND_MS 250
#endif
#ifdef HB_TIMEOUT_MS 
#define HB_TIMEOUT_MS 2000
#endif

#ifdef HB_TAKEOVER_HOLD_MS
#define HB_TAKEOVER_HOLD_MS 5000
#endif

#ifdef RELAY_A_PIN
#define RELAY_A_PIN 25
#endif
#ifdef RELAY_B_PIN
#define RELAY_B_PIN 26
#endif

#ifdef RELAY_ACTIVE_LOW
#define RELAY_ACTIVE_LOW 1
#endif

#ifdef BREAK_BEFORE_MAKE_MS
#define BREAK_BEFORE_MAKE_MS 30
#endif
