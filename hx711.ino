// #include "HX711NB.h"

// constexpr uint8_t HX_DOUT_PIN = 5;
// constexpr uint8_t HX_SCK_PIN  = 4;
// constexpr float   CAL_FACTOR  = 2280.0f;

// HX711NB scale;

// void setup()
// {
//     Serial.begin(38400);
//     scale.begin(HX_DOUT_PIN, HX_SCK_PIN, CAL_FACTOR);

//     Serial.println("HX710B non-blocking HX711 setup ready");
// }

// void loop()
// {
//     scale.process();   // call often, never blocks

//     if (scale.available()) {
//         Serial.print("Weight: ");
//         Serial.print(scale.getKg(), 2);
//         Serial.println(" kg");
//     }

//     // Other tasks run freely here
// }




#include <Arduino.h>
#include <EEPROM.h>
#include "HX711NB.h"

/* ---------------- Config ---------------- */
constexpr uint8_t HX_DOUT_PIN = 5;
constexpr uint8_t HX_SCK_PIN  = 4;

constexpr uint32_t CAL_TIMEOUT_MS = 10000;
constexpr uint16_t EEPROM_SIZE    = 64;
constexpr uint16_t EEPROM_ADDR    = 0;
constexpr uint32_t CAL_MAGIC      = 0xC0FFEEAA;

/* ---------------- Globals ---------------- */
HX711NB scaleNB;
CalibrationData cal;

/* ---------------- EEPROM helpers ---------------- */

bool loadCalibration()
{
    EEPROM.get(EEPROM_ADDR, cal);
    return (cal.magic == CAL_MAGIC);
}

void saveCalibration()
{
    cal.magic = CAL_MAGIC;
    EEPROM.put(EEPROM_ADDR, cal);
    EEPROM.commit();
}

/* ---------------- Calibration ---------------- */

bool waitForCalibrationRequest()
{
    Serial.println("Press ENTER within 10 seconds to enter calibration...");
    uint32_t start = millis();

    while (millis() - start < CAL_TIMEOUT_MS) {
        if (Serial.available()) {
            Serial.read();
            return true;
        }
        delay(10);
    }
    return false;
}

void runCalibration(HX711NB& nb)
{
    HX711& hx = nb.raw();

    Serial.println("\n=== CALIBRATION MODE ===");
    Serial.println("Remove all weight, then press ENTER.");

    while (!Serial.available()) {}
    Serial.read();

    hx.tare();
    cal.offset = hx.get_offset();

    Serial.print("Offset captured: ");
    Serial.println(cal.offset);

    Serial.println("Place known weight and enter value in kg:");

    while (!Serial.available()) {}
    float knownWeight = Serial.parseFloat();

    Serial.println("Measuring...");
    delay(2000);

    float reading = hx.get_units(10);
    cal.scale = reading / knownWeight;

    nb.applyCalibration(cal.scale, cal.offset);
    saveCalibration();

    Serial.println("Calibration saved.");
}

/* ---------------- Setup ---------------- */

void setup()
{
    Serial.begin(38400);
    while (!Serial) {}

    EEPROM.begin(EEPROM_SIZE);

    scaleNB.begin(HX_DOUT_PIN, HX_SCK_PIN);

    bool doCal = waitForCalibrationRequest();

    if (doCal || !loadCalibration()) {
        runCalibration(scaleNB);
    } else {
        scaleNB.applyCalibration(cal.scale, cal.offset);
        Serial.println("Loaded calibration from EEPROM.");
    }

    Serial.println("System ready (NB mode).");
}

/* ---------------- Loop ---------------- */

void loop()
{
    scaleNB.process();   // non-blocking

    if (scaleNB.available()) {
        Serial.print("Weight: ");
        Serial.print(scaleNB.getKg(), 2);
        Serial.println(" kg");
    }

    // other tasks run freely
}
