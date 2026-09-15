#include "R558S.h"

// =====================================================
// CONSTRUCTOR
// =====================================================

R558S::R558S(SoftwareSerial *serial) {
    _serial = serial;
}

// =====================================================
// INITIALIZATION
// =====================================================

bool R558S::begin(uint32_t baudrate) {

    _serial->begin(baudrate);

    delay(100);

    // Turn sensor LED on automatically.
    setLED(R558S_LED_AUTO_ON);

    delay(50);

    // Probe the sensor.
    uint8_t response = rawGetImage();

    if (response == R558S_TIMEOUT ||
        response == R558S_COMM_ERROR) {
        return false;
    }

    return true;
}

// =====================================================
// PURGE SENSOR RX BUFFER
// =====================================================

void R558S::purgeSensorBuffer() {

    while (_serial->available()) {
        _serial->read();
    }
}

// =====================================================
// SEND COMPLETE PACKET AND READ REPLY
// =====================================================

uint8_t R558S::sendPacket(
    const uint8_t *packet,
    uint8_t packetSize,
    uint8_t *reply,
    uint8_t replySize,
    uint16_t timeoutMs
) {

    purgeSensorBuffer();

    _serial->write(packet, packetSize);
    _serial->flush();

    uint8_t index = 0;

    unsigned long start = millis();

    while (millis() - start < timeoutMs) {

        if (_serial->available()) {

            if (index < replySize) {
                reply[index++] = _serial->read();
            }
            else {
                // Reply buffer full.
                _serial->read();
                break;
            }
        }
    }

    // Timeout
    if (index < replySize) {
        return R558S_TIMEOUT;
    }

    // Validate packet header.
    if (reply[0] != 0xEF ||
        reply[1] != 0x01) {

        return R558S_COMM_ERROR;
    }

    // Confirmation code.
    return reply[9];
}

// =====================================================
// LED CONTROL
// =====================================================

uint8_t R558S::setLED(uint8_t mode) {

    uint8_t reply[12];
    uint8_t status = R558S_COMM_ERROR;

    switch (mode) {

        case R558S_LED_BLUE: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x07,
                0x3C,
                0x03, 0x01, 0x01,
                0x00, 0x00,
                0x49
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }

        case R558S_LED_GREEN: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x07,
                0x3C,
                0x03, 0x02, 0x02,
                0x00, 0x00,
                0x4B
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }

        case R558S_LED_RED: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x07,
                0x3C,
                0x03, 0x04, 0x04,
                0x00, 0x00,
                0x4F
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }

        case R558S_LED_OFF: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x07,
                0x3C,
                0x04, 0x00, 0x00,
                0x00, 0x00,
                0x48
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }

        case R558S_LED_AUTO_ON: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x05,
                0x0E,
                0x11, 0x01,
                0x00, 0x26
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }

        case R558S_LED_AUTO_OFF: {

            uint8_t packet[] = {
                0xEF, 0x01,
                0xFF, 0xFF, 0xFF, 0xFF,
                0x01,
                0x00, 0x05,
                0x0E,
                0x11, 0x00,
                0x00, 0x25
            };

            status = sendPacket(
                packet,
                sizeof(packet),
                reply,
                12,
                300
            );

            break;
        }
    }

    return status;
}

// =====================================================
// GET IMAGE
// =====================================================

uint8_t R558S::rawGetImage() {

    uint8_t packet[] = {
        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x01,
        0x00, 0x03,
        0x01,
        0x00, 0x05
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        300
    );
}

uint8_t R558S::getImage() {
    return rawGetImage();
}

// =====================================================
// IMAGE -> CHARACTER BUFFER
// =====================================================

uint8_t R558S::rawImage2Tz(uint8_t bufferID) {

    uint16_t sum =
        0x01 +
        0x00 +
        0x04 +
        0x02 +
        bufferID;

    uint8_t packet[] = {

        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x01,

        0x00, 0x04,

        0x02,
        bufferID,

        (uint8_t)(sum >> 8),
        (uint8_t)(sum & 0xFF)
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        300
    );
}

uint8_t R558S::image2Tz(uint8_t bufferID) {
    return rawImage2Tz(bufferID);
}

// =====================================================
// REGISTER MODEL
// =====================================================

uint8_t R558S::rawRegModel() {

    uint8_t packet[] = {

        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x01,

        0x00, 0x03,

        0x05,
        0x00,
        0x09
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        300
    );
}

uint8_t R558S::createModel() {
    return rawRegModel();
}

// =====================================================
// STORE MODEL
// IMPORTANT:
// pageID IS USED DIRECTLY.
// NO pageID - 1.
// =====================================================

uint8_t R558S::rawStoreModel(
    uint8_t bufferID,
    uint8_t pageID
) {

    uint16_t sum =
        0x01 +
        0x00 +
        0x06 +
        0x06 +
        bufferID +
        0x00 +
        pageID;

    uint8_t packet[] = {

        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x01,

        0x00, 0x06,

        0x06,

        bufferID,

        0x00,
        pageID,

        (uint8_t)(sum >> 8),
        (uint8_t)(sum & 0xFF)
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        500
    );
}

uint8_t R558S::storeModel(uint8_t pageID) {

    return rawStoreModel(
        0x01,
        pageID
    );
}

// =====================================================
// SEARCH
// =====================================================

uint8_t R558S::rawSearchModel(
    uint8_t bufferID,
    uint16_t startPage,
    uint16_t pageNum,
    uint16_t *matchID,
    uint16_t *matchScore
) {

    uint8_t startHigh =
        (uint8_t)(startPage >> 8);

    uint8_t startLow =
        (uint8_t)(startPage & 0xFF);

    uint8_t countHigh =
        (uint8_t)(pageNum >> 8);

    uint8_t countLow =
        (uint8_t)(pageNum & 0xFF);

    uint16_t sum =
        0x01 +
        0x00 +
        0x08 +
        0x04 +
        bufferID +
        startHigh +
        startLow +
        countHigh +
        countLow;

    uint8_t packet[] = {

        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x01,

        0x00, 0x08,

        0x04,

        bufferID,

        startHigh,
        startLow,

        countHigh,
        countLow,

        (uint8_t)(sum >> 8),
        (uint8_t)(sum & 0xFF)
    };

    uint8_t reply[16];

    uint8_t code = sendPacket(
        packet,
        sizeof(packet),
        reply,
        16,
        750
    );

    if (code == R558S_OK) {

        *matchID =
            ((uint16_t)reply[10] << 8) |
            reply[11];

        *matchScore =
            ((uint16_t)reply[12] << 8) |
            reply[13];
    }

    return code;
}

uint8_t R558S::searchFinger(
    uint8_t bufferID,
    uint16_t startPage,
    uint16_t pageNum,
    uint16_t *matchID,
    uint16_t *matchScore
) {

    return rawSearchModel(
        bufferID,
        startPage,
        pageNum,
        matchID,
        matchScore
    );
}

// =====================================================
// DELETE MODEL
// IMPORTANT:
// pageID IS USED DIRECTLY.
// =====================================================

uint8_t R558S::rawDeleteModel(uint8_t pageID) {

    uint16_t page = pageID;

    uint16_t count = 1;

    uint8_t pageHigh =
        (uint8_t)(page >> 8);

    uint8_t pageLow =
        (uint8_t)(page & 0xFF);

    uint8_t countHigh =
        (uint8_t)(count >> 8);

    uint8_t countLow =
        (uint8_t)(count & 0xFF);

    uint16_t sum =
        0x01 +
        0x00 +
        0x07 +
        0x0C +
        pageHigh +
        pageLow +
        countHigh +
        countLow;

    uint8_t packet[] = {

        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,

        0x01,

        0x00, 0x07,

        0x0C,

        pageHigh,
        pageLow,

        countHigh,
        countLow,

        (uint8_t)(sum >> 8),
        (uint8_t)(sum & 0xFF)
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        500
    );
}

uint8_t R558S::deleteModel(uint8_t pageID) {

    return rawDeleteModel(pageID);
}

// =====================================================
// EMPTY DATABASE
// =====================================================

uint8_t R558S::rawEmptyDatabase() {
    // Instruction 0x0D: empty fingerprint library.
    uint8_t packet[] = {
        0xEF, 0x01,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x01,
        0x00, 0x03,
        0x0D,
        0x00, 0x11
    };

    uint8_t reply[12];

    return sendPacket(
        packet,
        sizeof(packet),
        reply,
        12,
        3000
    );
}

uint8_t R558S::emptyDatabase() {
    return rawEmptyDatabase();
}

// =====================================================
// WAIT FOR FINGER
// =====================================================

bool R558S::waitForFinger(uint32_t timeoutMs) {

    unsigned long start = millis();

    while (millis() - start < timeoutMs) {

        uint8_t code = rawGetImage();

        if (code == R558S_OK) {
            return true;
        }

        if (code == R558S_NOFINGER ||
            code == R558S_TIMEOUT) {

            delay(20);
            continue;
        }

        // Other error
        delay(20);
    }

    return false;
}

// =====================================================
// WAIT FOR FINGER REMOVAL
// =====================================================

void R558S::waitForRemoval() {

    while (true) {

        uint8_t code = rawGetImage();

        if (code != R558S_OK) {
            break;
        }

        delay(20);
    }

    delay(100);
}