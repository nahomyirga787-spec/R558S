#include <SoftwareSerial.h>
#include <R558S.h>

SoftwareSerial sensorSerial(2, 3);  // RX, TX
R558S finger(&sensorSerial);

void setup() {
    Serial.begin(9600);
    delay(500);

    Serial.println(F("\n=============================="));
    Serial.println(F("      R558S IDENTIFY"));
    Serial.println(F("=============================="));

    if (!finger.begin(57600)) {
        Serial.println(F("ERROR: Sensor communication failed."));
        while (true) {}
    }

    Serial.println(F("Sensor communication OK."));
}

void loop() {
    Serial.println(F("\nPlace finger on sensor..."));

    if (!finger.waitForFinger(10000)) {
        Serial.println(F("Timeout: no finger detected."));
        return;
    }

    uint8_t code = finger.image2Tz(1);

    if (code != R558S_OK) {
        Serial.print(F("Feature extraction failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
        finger.waitForRemoval();
        return;
    }

    uint16_t matchID = 0;
    uint16_t matchScore = 0;

    code = finger.searchFinger(1, 1, 100, &matchID, &matchScore);

    if (code == R558S_OK) {
        Serial.println(F("\nMATCH FOUND"));
        Serial.print(F("Slot ID   : #"));
        Serial.println(matchID);
        Serial.print(F("Confidence: "));
        Serial.println(matchScore);
        finger.setLED(R558S_LED_GREEN);
    } else if (code == R558S_NOMATCH) {
        Serial.println(F("\nNO MATCH FOUND"));
        finger.setLED(R558S_LED_RED);
    } else {
        Serial.print(F("Search failed. Code: 0x"));
        Serial.println(code, HEX);
        finger.setLED(R558S_LED_RED);
    }

    Serial.println(F("Remove finger..."));
    finger.waitForRemoval();
    delay(1000);
}
