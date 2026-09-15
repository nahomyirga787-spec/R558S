#ifndef R558S_H
#define R558S_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// R558S return codes
#define R558S_OK            0x00
#define R558S_NOFINGER      0x02
#define R558S_IMAGEFAIL     0x03
#define R558S_MESSFAIL      0x06
#define R558S_FEATUREFAIL   0x07
#define R558S_NOMATCH       0x09
#define R558S_ENROLLFAIL    0x0A
#define R558S_BADLOCATION   0x0B

#define R558S_TIMEOUT       0xFF
#define R558S_COMM_ERROR    0xFE

// R558S LED modes
#define R558S_LED_OFF       0
#define R558S_LED_BLUE      1
#define R558S_LED_GREEN     2
#define R558S_LED_RED       3
#define R558S_LED_AUTO_ON   4
#define R558S_LED_AUTO_OFF  5

class R558S {
public:
    explicit R558S(SoftwareSerial *serial);

    bool begin(uint32_t baudrate = 57600);

    uint8_t setLED(uint8_t mode);

    uint8_t getImage();
    uint8_t image2Tz(uint8_t bufferID);
    uint8_t createModel();

    uint8_t storeModel(uint8_t pageID);
    uint8_t deleteModel(uint8_t pageID);
    uint8_t emptyDatabase();

    uint8_t searchFinger(
        uint8_t bufferID,
        uint16_t startPage,
        uint16_t pageNum,
        uint16_t *matchID,
        uint16_t *matchScore
    );

    bool waitForFinger(uint32_t timeoutMs = 10000);
    void waitForRemoval();

private:
    SoftwareSerial *_serial;

    uint8_t sendPacket(
        const uint8_t *packet,
        uint8_t packetSize,
        uint8_t *reply,
        uint8_t replySize,
        uint16_t timeoutMs
    );

    uint8_t rawGetImage();
    uint8_t rawImage2Tz(uint8_t bufferID);
    uint8_t rawRegModel();
    uint8_t rawStoreModel(uint8_t bufferID, uint8_t pageID);

    uint8_t rawSearchModel(
        uint8_t bufferID,
        uint16_t startPage,
        uint16_t pageNum,
        uint16_t *matchID,
        uint16_t *matchScore
    );

    uint8_t rawDeleteModel(uint8_t pageID);
    uint8_t rawEmptyDatabase();


    void purgeSensorBuffer();
};

#endif
