#include <SoftwareSerial.h>
#include <R558S.h>

SoftwareSerial sensorSerial(2, 3);  // RX, TX
R558S finger(&sensorSerial);

void setup() {
    Serial.begin(9600);
    delay(500);

    Serial.println(F("\n=============================="));
    Serial.println(F("        R558S DELETE"));
    Serial.println(F("=============================="));

    if (!finger.begin(57600)) {
        Serial.println(F("ERROR: Sensor communication failed."));
        while (true) {}
    }

    Serial.println(F("Sensor communication OK."));
}

int readNumber() {
    // Read a complete line so CR/LF from the previous entry cannot
    // become the next menu selection.
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

void deleteOne() {
    Serial.println(F("\nEnter Slot ID to delete (1-100):"));
    int id = readNumber();

    if (id < 1 || id > 100) {
        Serial.println(F("Invalid Slot ID. Range: 1-100."));
        return;
    }

    uint8_t code = finger.deleteModel((uint8_t)id);

    if (code == R558S_OK) {
        Serial.print(F("SUCCESS: Slot ID #"));
        Serial.print(id);
        Serial.println(F(" deleted."));
        finger.setLED(R558S_LED_GREEN);
    } else {
        Serial.print(F("Delete failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
    }

    delay(1000);
}

void deleteAll() {
    Serial.println(F("\nWARNING: THIS WILL DELETE ALL FINGERPRINTS."));
    Serial.println(F("This action cannot be undone."));
    Serial.println(F("Enter 99 to confirm:"));

    int confirmation = readNumber();

    if (confirmation != 99) {
        Serial.println(F("Operation cancelled."));
        return;
    }

    Serial.println(F("Deleting entire fingerprint database..."));

    uint8_t code = finger.emptyDatabase();

    if (code == R558S_OK) {
        Serial.println(F("SUCCESS: All fingerprints deleted."));
        finger.setLED(R558S_LED_GREEN);
    } else {
        Serial.print(F("Delete-all failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
    }

    delay(1500);
}

void loop() {
    Serial.println(F("\n------------------------------"));
    Serial.println(F("1. Delete fingerprint"));
    Serial.println(F("2. Delete ALL fingerprints"));
    Serial.println(F("------------------------------"));
    Serial.println(F("Select option:"));

    int option = readNumber();

    switch (option) {
        case 1:
            deleteOne();
            break;
        case 2:
            deleteAll();
            break;
        default:
            Serial.println(F("Invalid option."));
            break;
    }
}
