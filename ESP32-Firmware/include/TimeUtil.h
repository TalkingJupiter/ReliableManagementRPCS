#pragma once
#include <Arduino.h>

static inline bool elapsed(uint32_t now, uint32_t since, uint32_t interval){
    return (uint32_t)(now-since) >= interval;
}

