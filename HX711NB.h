// #pragma once
// #include <Arduino.h>
// #include "HX711.h"

// class HX711NB {
// public:
//     void begin(uint8_t doutPin, uint8_t sckPin, float scaleFactor);
//     void process();
//     bool available();
//     float getKg();

// private:
//     enum State {
//         WAIT_READY,
//         READ
//     };

//     HX711 _scale;
//     State _state = WAIT_READY;
//     bool  _dataReady = false;
//     float _valueKg = 0.0f;
// };


#pragma once
#include <Arduino.h>
#include "HX711.h"

struct CalibrationData {
    float scale;
    long  offset;
    uint32_t magic;
};

class HX711NB {
public:
    void begin(uint8_t doutPin, uint8_t sckPin);

    /* NB flow (unchanged) */
    void process();
    bool available();
    float getKg();

    /* calibration interface */
    void applyCalibration(float scale, long offset);
    CalibrationData getCalibration() const;

    /* raw HX711 access (for calibration only) */
    HX711& raw();

private:
    enum State {
        WAIT_READY,
        READ
    };

    HX711 _scale;
    State _state = WAIT_READY;

    bool  _dataReady = false;
    float _valueKg   = 0.0f;

    float _scaleFactor = 1.0f;
    long  _offset      = 0;
};
