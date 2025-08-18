# Sensor Documentation

This repository contains usage examples and datasheets for several commonly used sensors in hardware development. Each folder includes datasheets and simple example code (mostly in Arduino `.ino` format).

## Prerequisites

Before running any example code from this repository, make sure you have the following prerequisites:

- **Arduino IDE** (latest version recommended)
- **XIAO-ESP32S3** (e.g., Arduino Uno, Mega, Nano, or compatible)
- **Required sensor hardware** (as listed in each section below)
- **Corresponding Arduino libraries** (see each sensor/project section for details)

### Project-Specific Prerequisites

#### Blynx Projects

- Arduino IDE
- XIAO-ESP32S3 (compatible with Blynx)
- Blynx app installed on your mobile device
- Blynx Arduino library (install via Arduino Library Manager)

#### BME688-Gas Sensor

- Arduino IDE
- XIAO-ESP32S3
- BME688 sensor module
- Adafruit BME680/BME688 Arduino library (install via Arduino Library Manager)

#### DPS310-Barometer

- Arduino IDE
- XIAO-ESP32S3
- DPS310 barometric pressure sensor
- Adafruit DPS310 Arduino library (install via Arduino Library Manager)

#### Flame Sensor

- Arduino IDE
- XIAO-ESP32S3
- Flame sensor module
  (No special library required; uses digitalRead/analogRead)

#### Grove-Water Level Sensor

- Arduino IDE
- XIAO-ESP32S3
- Grove Water Level Sensor
  (No special library required; uses analogRead)

#### MCP9808-Temperature

- Arduino IDE
- XIAO-ESP32S3
- MCP9808 temperature sensor module
- Adafruit MCP9808 Arduino library (install via Arduino Library Manager)

## List of Sensors

## Additional Projects

### Blynx Projects

This section contains example projects using the Blynx platform for sensor integration and IoT applications.

- **DPS310-Blynx/DPS310-Blynx.ino**: Example code for integrating the DPS310 barometric pressure sensor with Blynx, demonstrating remote monitoring or IoT connectivity.

### 1. BME688-Gas Sensor

The BME688 is a digital gas sensor capable of detecting air quality, humidity, pressure, and temperature. This folder contains:

- **BME688-Datasheet.pdf & BME688-Datasheet2.pdf**: Official datasheets for the BME688 sensor.
- **BME688-Simple/BME688-Simple.ino**: Simple example code for reading data from the BME688 sensor using Arduino.

### 2. DPS310-Barometer

The DPS310 is a high-precision digital barometric pressure sensor, suitable for measuring air pressure and altitude. This folder contains:

- **DPS31-Datasheet-en.pdf & DPS31-Grove-Datasheet-en.pdf**: Datasheets for the DPS310 sensor and Grove version.
- **sketch_aug18a/DPS310-Simple.ino**: Simple example code for reading pressure data from the DPS310 sensor.

### 3. Flame Sensor

The Flame Sensor is used to detect the presence of fire or flame. This folder contains:

- **Flame Sensor-Datasheet.pdf**: Datasheet for the flame sensor.
- **FlameSensor-Simple.pdf/FlameSensor-Simple.pdf.ino**: Simple example code for detecting fire using the flame sensor.

### 4. Grove-Water Level Sensor

This sensor is used to detect water level or height. This folder contains:

- **Water Level Sensor-Datasheet.pdf**: Datasheet for the Grove water level sensor.
- **WaterLevel-Simple/WaterLevel-Simple.ino**: Simple example code for reading water level data.

### 5. MCP9808-Temperature

The MCP9808 is a high-accuracy digital temperature sensor. This folder contains:

- **MCP9808-0.5C-Maximum-Accuracy-Digital-Temperature-Sensor-Data-Sheet-DS20005095B.pdf**: Official MCP9808 datasheet.
- **MCP9808-Grove-Temprature sensor-Datasheet.pdf**: Grove version datasheet.
- **MCP9808-Simple/MCP9808-Simple.ino**: Simple example code for reading temperature from the MCP9808 sensor.

---

Each folder can be used as a reference to understand the sensor specifications and basic data reading implementation using Arduino.
