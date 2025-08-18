/*************************************************************
  Example: ESP32 + Blynk + Adafruit DPS310
*************************************************************/


#define BLYNK_TEMPLATE_ID "TMPL6eNdoDhIz"
#define BLYNK_TEMPLATE_NAME "izbat"
#define BLYNK_AUTH_TOKEN "rvTFZ3p78QueoDEaruO8a0HB4qV_s7Zr"


// Comment this out to disable prints
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#include <Adafruit_Sensor.h>
#include <Adafruit_DPS310.h>


// Replace with your WiFi credentials
char ssid[] = "izbat's Iphone";
char pass[] = "12345678";


// Blynk timer
BlynkTimer timer;


// Create DPS310 instance
Adafruit_DPS310 dps;


// Function to send data to Blynk
void sendSensorData() {
 sensors_event_t temp_event, pressure_event;
 dps.getEvents(&temp_event, &pressure_event);


 Serial.print("Temperature: ");
 Serial.print(temp_event.temperature);
 Serial.println(" °C");


 Serial.print("Pressure: ");
 Serial.print(pressure_event.pressure);
 Serial.println(" hPa");


 // Push to Blynk dashboard
 Blynk.virtualWrite(V0, temp_event.temperature); // assign to Gauge/Display widget
 Blynk.virtualWrite(V1, pressure_event.pressure);
}


void setup() {
 // Debug console
 Serial.begin(115200);


 // Connect to WiFi + Blynk
 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);


 // Initialize DPS310
 if (!dps.begin_I2C()) {
   Serial.println("Could not find a valid DPS310 sensor, check wiring!");
   while (1) delay(10);
 }
 Serial.println("DPS310 sensor found!");


 // Configure sensor sampling
 dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
 dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);


 // Send data every 2 seconds
 timer.setInterval(2000L, sendSensorData);
}


void loop() {
 Blynk.run();   // keep Blynk connected
 timer.run();   // run timed tasks
}

