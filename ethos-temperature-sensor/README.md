# Ethos Temperature Sensor

This is the code which runs on the Feather M0 board to send temperature sensor data to the base station.

## Installation/Setup

1. Install the required libraries from the Arduino IDE
2. Copy `conf.h.example` -> `conf.h` and add your 16 bit radio encryption key
3. Upload code to Feather M0 LoRa board with temperature sensor

## Arduino IDE Setup

1. Add the additional board manager url `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`
2. Install the `Arduino SAMD Boards` from the boards manager.
3. Install the `Adafruit SAMD Boards` from the boards manager.
4. Select the matching board: `Adafruit Feather M0 (SAMD21)`.

## Required Library

- **Radio Head** : Download the github repo as zip, then add it manually - [Library Source](https://github.com/epsilonrt/RadioHead)
- **Adafruit SHT4X** : Can be found on the library manager on the arduino IDE
- **Crypto** : Download the github repo as zip, then add it manually - [Library Source](https://github.com/OperatorFoundation/Crypto)
- **RTC Zero** : Download the github repo as zip, then add it manually - [Library Source](https://github.com/arduino-libraries/RTCZero)

## Important

Each of temperature sensor must have their unique sensor ID and the same frequency, so before to compile kindly to adjust the frequency and the Sensor ID

```c++
// This should match the sticker on the sensor
int sensor_id = 722;

// Change default frequency of radio
float radio_frequency = 923.2;

```
