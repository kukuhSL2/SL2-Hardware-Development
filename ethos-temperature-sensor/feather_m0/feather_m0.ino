// Import for sleep
#include <RTCZero.h>

// Library for SHT4x
#include "Adafruit_SHT4x.h"

// Custom library for radio
#include "Radio.h"

// Define SHT power pin
#define SHT_PWD_PIN A0

// Define battery pin
#define VBATPIN A7

#if defined (MOTEINO_M0)
  #if defined(SERIAL_PORT_USBVIRTUAL)
    #define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards
  #endif
#endif

// The ID of the sensor, change depending on what number to assign
// This should match the sticker on the sensor
int sensor_id = 722;

// Change default frequency of radio
float radio_frequency = 923.2;
Radio radio(radio_frequency);

// RTC Clock for sleep
RTCZero zerortc;

// How often to take temperature measurements
const byte alarmSeconds = 0;
const byte alarmMinutes = 10;
const byte alarmHours = 0;

volatile bool alarmFlag = true; // Start awake

// Setup temp sensor
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void setup()
{
  // Begin console
  Serial.begin(115200);

  // Turn off LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Setup A0 to power output
  digitalWrite(SHT_PWD_PIN, LOW);
  pinMode(SHT_PWD_PIN, OUTPUT);

  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);
  Serial.println("Configured sensor");

  // Setup radio
  radio.setupRadio();
  radio.sleepRadio();

  // Setup clocks
  zerortc.begin();

  // Set alarm
  resetAlarm();
  // Setup handler for alarm
  zerortc.attachInterrupt(alarmMatch);
}

void loop()
{
  // Woken up from sleep
  if (alarmFlag == true) {
    alarmFlag = false;  // Clear flag
    // Turn on sensor
    digitalWrite(SHT_PWD_PIN, HIGH);
    delay(10);

    // Setup temp sensor
    int num_retries = 0;
    while (!sht4.begin() && num_retries < 10) {
      num_retries++; // Prevent infinite loop
      delay(15);
    }
    if (num_retries >= 10) {
      Serial.println("Unable to setup radio");
      sleepEverything();
      return; // End the loop immediately
    }

    // Read sensor data measurement
    sensors_event_t humidity, temperature;
    num_retries = 0;
    while(!sht4.getEvent(&humidity, &temperature) && num_retries < 10) {
      num_retries++; // Prevent infinite loop
      delay(15);
    }
    if (num_retries >= 10 || isnan(humidity.relative_humidity) || isnan(temperature.temperature)) {
      Serial.println("Unable to get temp/humidity measurement");
      sleepEverything();
      return; // End the loop immediately
    }

    // Turn off sensor
    digitalWrite(SHT_PWD_PIN, LOW);

    // Read battery voltage
    u_int32_t raw_voltage_value = analogRead(VBATPIN);
    float battery_voltage = raw_voltage_value * 2; // Divided by 2, so multiply back
    battery_voltage *= 3.3;  // Multiply by 3.3V (reference voltage)
    battery_voltage /= 1024; // Convert to voltage
    // Ensure battery voltage is within a realistic range
    if (battery_voltage <= 0.0 || battery_voltage > 5.0) {
      battery_voltage = 1; // Default safe value
    }

    // Send packet to gateway if sensor reading exists
    if (num_retries < 10) {
      radio.sendPacket(sensor_id, temperature.temperature, humidity.relative_humidity, battery_voltage);
      delay(10); // This is needed to prevent hanging
    }
  }
  // Reset alarm and return to sleep
  sleepEverything();
}

void sleepEverything(void) {
  // Turn off sensor
  digitalWrite(SHT_PWD_PIN, LOW);
  // Turn off radio
  radio.sleepRadio();
  // Reset alarm and return to sleep
  resetAlarm();
  zerortc.standbyMode();
}

void alarmMatch(void) {
  alarmFlag = true; // Set flag
}

void resetAlarm(void) {
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
  byte day = 1;
  byte month = 1;
  byte year = 1;

  zerortc.setTime(hours, minutes, seconds);
  zerortc.setDate(day, month, year);

  zerortc.setAlarmTime(alarmHours, alarmMinutes, alarmSeconds);
  zerortc.enableAlarm(zerortc.MATCH_HHMMSS);
}