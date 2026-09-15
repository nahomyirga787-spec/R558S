# R558S Arduino Library

Arduino library for the R558S fingerprint sensor module.

## Features

- Fingerprint image capture
- Image-to-template conversion
- Fingerprint model creation
- Four-image enrollment workflow
- Template storage
- Fingerprint identification
- Template deletion
- Delete-all database operation
- Sensor LED control
- Finger detection and finger-removal helpers

## Hardware

The R558S runs on 3.3V and its RX line is not 5V-tolerant. If you are using
a 5V board such as the Arduino Uno, use a logic-level shifter (or a
resistor divider) between the Uno's TX pin and the sensor's RX pin, or
power and drive the sensor from a 3.3V-logic board directly. Driving RX
straight from a 5V TX pin is out of spec and is a common cause of
unreliable communication with this sensor.

The examples are written for an Arduino Uno using `SoftwareSerial`.

Example wiring:

| R558S | Arduino Uno |
|---|---|
| TX | D2 |
| RX | D3 |
| GND | GND |
| VCC | Sensor/module supply |

The software serial declaration is:

```cpp
SoftwareSerial sensorSerial(2, 3);
```

This means:

- Arduino D2 = RX
- Arduino D3 = TX

Connect the sensor TX to Arduino RX and the sensor RX to Arduino TX.

## Serial configuration

The R558S examples use:

- Sensor UART: 57600 baud
- Arduino Serial Monitor: 9600 baud

## Installation

### Arduino IDE

Download or clone this repository and place the `R558S` folder in your Arduino libraries directory.

Restart Arduino IDE after installation.

The examples are available under:

```text
File → Examples → R558S
```

## Examples

### Enroll

`Enroll` captures four fingerprint images, creates a fingerprint model, and stores it in the selected database address.

The example asks for a User ID from 1–100.

The example passes the selected Slot ID directly to the R558S database command. Use Slot IDs 1–100.

### Identify

`Identify` captures a fingerprint image, extracts its features, searches the sensor database, and reports the matching User ID and score.

### Delete

`Delete` provides two operations:

1. Delete one fingerprint by User ID.
2. Delete the entire fingerprint database.

The delete-all operation requires entering `99` as confirmation.

## Dependencies

This library uses Arduino `SoftwareSerial`.

For Arduino AVR boards such as the Uno, `SoftwareSerial` is normally available with the Arduino platform.

## API overview

```cpp
R558S finger(&sensorSerial);

finger.begin(57600);

finger.getImage();
finger.image2Tz(1);
finger.createModel();

finger.storeModel(0);
finger.deleteModel(0);
finger.emptyDatabase();

finger.searchFinger(
    1,
    0,
    100,
    &matchID,
    &matchScore
);

finger.waitForFinger();
finger.waitForRemoval();

finger.setLED(R558S_LED_GREEN);
```

## License

MIT License. See `LICENSE`.
