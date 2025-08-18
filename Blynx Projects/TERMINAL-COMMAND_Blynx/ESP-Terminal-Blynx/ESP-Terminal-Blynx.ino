
/*************************************************************
  Example: ESP32 + Blynk + Adafruit DPS310 + Terminal Control
*************************************************************/


#define BLYNK_TEMPLATE_ID "TMPL6eNdoDhIz"
#define BLYNK_TEMPLATE_NAME "izbat"
#define BLYNK_AUTH_TOKEN "rvTFZ3p78QueoDEaruO8a0HB4qV_s7Zr"


#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#include <Adafruit_Sensor.h>
#include <Adafruit_DPS310.h>


// WiFi credentials
char ssid[] = "freehotspotITB";
char pass[] = "STPGanesha2025";


// Blynk timer
BlynkTimer timer;


// Create DPS310 instance
Adafruit_DPS310 dps;


// Terminal widget (assign Virtual Pin V2 for example)
WidgetTerminal terminal(V2);


bool sendData = true;   // flag to control sending
int interval = 2000;    // default 2 seconds
int timerID;            // timer handler


// Function to send data to Blynk
void sendSensorData() {
 if (!sendData) return;  // do nothing if stopped


 sensors_event_t temp_event, pressure_event;
 dps.getEvents(&temp_event, &pressure_event);


 Serial.print("Temperature: ");
 Serial.print(temp_event.temperature);
 Serial.println(" °C");


 Serial.print("Pressure: ");
 Serial.print(pressure_event.pressure);
 Serial.println(" hPa");


 // Push to Blynk dashboard
 Blynk.virtualWrite(V0, temp_event.temperature);
 Blynk.virtualWrite(V1, pressure_event.pressure);
}


// Handle terminal input
BLYNK_WRITE(V2) {
 String cmd = param.asStr();


 if (cmd == "1") {
   sendData = false;
   terminal.println("Data sending stopped.");
 }
 else if (cmd == "2") {
   sendData = true;
   terminal.println("Data sending resumed.");
 }
 else if (cmd == "3") {
   timer.deleteTimer(timerID);          // remove old timer
   interval = 3600000;                  // 1 hour
   timerID = timer.setInterval(interval, sendSensorData);
   terminal.println("Interval changed to every 1 hour.");
 }
 else if (cmd == "4") {
   terminal.println("Rebooting ESP32...");
   terminal.flush();
   delay(1000);
   ESP.restart();
 }
 else {
   terminal.println("Unknown command. Use:");
   terminal.println("1 = Stop sending data");
   terminal.println("2 = Resume sending data");
   terminal.println("3 = Change interval to 1 hour");
   terminal.println("4 = Reboot ESP32");
 }


 terminal.flush();  // send to app
}


void setup() {
 Serial.begin(115200);


 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);


 // Initialize DPS310
 if (!dps.begin_I2C()) {
   Serial.println("Could not find DPS310 sensor, check wiring!");
   while (1) delay(10);
 }
 Serial.println("DPS310 sensor found!");


 // Configure sensor sampling
 dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
 dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);


 // Default timer (2 sec)
 timerID = timer.setInterval(interval, sendSensorData);


 terminal.println("Terminal ready. Type 1/2/3/4 to control ESP32.");
 terminal.flush();
}


void loop() {
 Blynk.run();
 timer.run();
}

