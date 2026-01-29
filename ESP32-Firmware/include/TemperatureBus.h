#pragma once
#include <Arduino.h>

class TemperatureBus{
    public:
        static constexpr uint8_t SENSORS_PER_BUS = 3;

        bool begin(int intakePin, int exhaustPin); // once in setup
        void tick(uint32_t nowMs);

        // true if we have at least one reading on each bus
        bool ready() const;

        bool hasNewSample() const;
        void clearNewSampleFlag();

        // Backward-compatible: "primary" reading for each bus (index 0).
        // Returns NAN if not available.
        float intakeC() const { return _intakePrimaryC; }
        float exhaustC() const { return _exhaustPrimaryC; }

        // New: read individual sensors (0..2). Returns NAN if missing/unavailable.
        float intakeC(uint8_t idx) const {
            return (idx < SENSORS_PER_BUS) ? _intakeC[idx] : NAN;
        }
        float exhaustC(uint8_t idx) const {
            return (idx < SENSORS_PER_BUS) ? _exhaustC[idx] : NAN;
        }

        // Device Counts for OneWire temp busses
        uint8_t intakeDeviceCount() const { return _intakeDeviceCount; }
        uint8_t exhaustDeviceCount() const { return _exhaustDeviceCount; }

    private:
        enum State { IDLE, REQUESTED, READ_READY };

        bool scanBuses();
        void requestConversion();
        void readTemperatures();

        int _intakePin;
        int _exhaustPin;

        void* _oneWireIntake = nullptr;
        void* _oneWireExhaust = nullptr;
        void* _dtIntake = nullptr;
        void* _dtExhaust = nullptr;

        uint8_t _intakeDeviceCount = 0;
        uint8_t _exhaustDeviceCount = 0;

        // Latest readings for up to 3 sensors per bus
        float _intakeC[SENSORS_PER_BUS]  = { NAN, NAN, NAN };
        float _exhaustC[SENSORS_PER_BUS] = { NAN, NAN, NAN };

        // Backward-compatible "primary" values (index 0)
        float _intakePrimaryC = NAN;
        float _exhaustPrimaryC = NAN;

        State _state = IDLE;
        uint32_t _lastRequestMs = 0;
        uint32_t _lastSampleMs = 0;

        bool _newSample = false;
};
