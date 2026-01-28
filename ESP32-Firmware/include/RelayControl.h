#pragma once
#include <Arduino.h>

enum class BusOwner {NONE, A, B};

class RelayControl{
public:
    void begin(int relayAPin, int relayBPin, bool activeLow);
    void setOwner(BusOwner owner);
    BusOwner owner() const {return _owner;}
private:
    int _pinA = -1;
    int _pinB = -1;
    bool _activeLow = true;;
    BusOwner _owner = BusOwner::NONE;

    void writeRelay(int pin, bool on);
};