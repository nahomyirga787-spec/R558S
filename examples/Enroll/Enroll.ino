#include <SoftwareSerial.h>
#include <R558S.h>

SoftwareSerial sensorSerial(2, 3);  // RX, TX
R558S finger(&sensorSerial);

const uint8_t ENROLL_TIMES = 4;

void setup() {
    Serial.begin(9600);
    delay(500);

    Serial.println(F("\n=============================="));
    Serial.println(F("       R558S ENROLL"));
    Serial.println(F("=============================="));

    if (!finger.begin(57600)) {
        Serial.println(F("ERROR: Sensor communication failed."));
        while (true) {}
    }

    Serial.println(F("Sensor communication OK."));
}

int readSlotID() {
    // Discard any leftover CR/LF from the previous entry.
    while (Serial.available()) Serial.read();

    int value = 0;
    bool hasDigit = false;

    while (true) {
        while (!Serial.available()) {
            delay(5);
        }

        char c = Serial.read();

        if (c >= '0' && c <= '9') {
            hasDigit = true;
            value = value * 10 + (c - '0');
        }
        else if (c == '\r' || c == '\n') {
            if (hasDigit) {
                return value;
            }
        }
    }
}

void loop() {
    Serial.println(F("\nEnter Slot ID (1-100):"));
    int id = readSlotID();

    if (id < 1 || id > 100) {
        Serial.println(F("Invalid Slot ID. Range: 1-100."));
        return;
    }

    uint8_t slotID = (uint8_t)id;

    Serial.print(F("Starting 4-image enrollment for Slot ID #"));
    Serial.println(slotID);

    for (uint8_t step = 1; step <= ENROLL_TIMES; step++) {
        bool converted = false;

        while (!converted) {
            Serial.print(F("\n[Scan "));
            Serial.print(step);
            Serial.println(F("/4] Place finger firmly on sensor..."));
            finger.setLED(R558S_LED_BLUE);

            uint32_t start = millis();
            uint8_t imageCode = R558S_NOFINGER;

            while (millis() - start < 10000UL) {
                imageCode = finger.getImage();

                if (imageCode == R558S_OK) break;
                if (imageCode == R558S_NOFINGER || imageCode == R558S_TIMEOUT) {
                    delay(100);
                    continue;
                }

                Serial.print(F("Capture error: 0x"));
                Serial.println(imageCode, HEX);
                delay(500);
            }

            if (imageCode != R558S_OK) {
                Serial.println(F("Capture timeout. Restarting this scan."));
                continue;
            }

            delay(200);

            uint8_t conversionCode = finger.image2Tz(step);

            if (conversionCode == R558S_OK) {
                Serial.print(F("Scan "));
                Serial.print(step);
                Serial.println(F(" loaded successfully."));
                finger.setLED(R558S_LED_GREEN);
                converted = true;
            } else {
                Serial.print(F("Conversion failed: 0x"));
                Serial.println(conversionCode, HEX);
                finger.setLED(R558S_LED_RED);
                Serial.println(F("Remove finger and retry."));

                while (finger.getImage() == R558S_OK) delay(100);
                delay(1000);
            }
        }

        if (step < ENROLL_TIMES) {
            Serial.println(F("Remove finger entirely..."));
            delay(500);

            while (finger.getImage() == R558S_OK) delay(100);

            Serial.println(F("Sensor clear. Preparing next scan..."));
            delay(1500);
        }
    }

    Serial.println(F("\nCreating fingerprint model..."));

    uint8_t code = finger.createModel();

    if (code != R558S_OK) {
        Serial.print(F("ERROR: Model creation failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
        finger.waitForRemoval();
        delay(1000);
        return;
    }

    Serial.println(F("Model created successfully."));
    Serial.print(F("Storing in Slot ID #"));
    Serial.println(slotID);

    code = finger.storeModel(slotID);

    if (code == R558S_OK) {
        Serial.println(F("SUCCESS: Fingerprint enrolled."));
        finger.setLED(R558S_LED_GREEN);
    } else {
        Serial.print(F("ERROR: Storage failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
    }

    Serial.println(F("Remove finger..."));
    finger.waitForRemoval();
    delay(1000);
}
