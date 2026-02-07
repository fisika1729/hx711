// #include "HX711NB.h"

// void HX711NB::begin(uint8_t doutPin, uint8_t sckPin, float scaleFactor)
// {
//     _scale.begin(doutPin, sckPin);
//     _scale.set_scale(scaleFactor);
//     _scale.tare();
// }

// void HX711NB::process()
// {
//     switch (_state) {

//     case WAIT_READY:
//         if (_scale.is_ready()) {
//             _state = READ;
//         }
//         break;

//     case READ:
//         // get_units(1) is fast and safe once ready
//         _valueKg = _scale.get_units(1);
//         _dataReady = true;
//         _state = WAIT_READY;
//         break;
//     }
// }

// bool HX711NB::available()
// {
//     if (_dataReady) {
//         _dataReady = false;
//         return true;
//     }
//     return false;
// }

// float HX711NB::getKg()
// {
//     return _valueKg;
// }


#include "HX711NB.h"

/* ---------------- Public API ---------------- */

void HX711NB::begin(uint8_t doutPin, uint8_t sckPin)
{
    _scale.begin(doutPin, sckPin);
}

void HX711NB::applyCalibration(float scale, long offset)
{
    _scaleFactor = scale;
    _offset      = offset;

    _scale.set_scale(scale);
    _scale.set_offset(offset);
}

CalibrationData HX711NB::getCalibration() const
{
    CalibrationData c{};
    c.scale  = _scaleFactor;
    c.offset = _offset;
    return c;
}

HX711& HX711NB::raw()
{
    return _scale;
}

/* ---------------- Non-blocking flow ---------------- */

void HX711NB::process()
{
    switch (_state) {

    case WAIT_READY:
        if (_scale.is_ready()) {
            _state = READ;
        }
        break;

    case READ:
        _valueKg = _scale.get_units(1);
        _dataReady = true;
        _state = WAIT_READY;
        break;
    }
}

bool HX711NB::available()
{
    if (_dataReady) {
        _dataReady = false;
        return true;
    }
    return false;
}

float HX711NB::getKg()
{
    return _valueKg;
}
