#define BLYNK_TEMPLATE_NAME "WATER LEVEL ESP32"
#define BLYNK_AUTH_TOKEN "-VB-Ir4Zg8T30XJBtiNQcAXZHN93HJdB"
#define BLYNK_TEMPLATE_ID "TMPL6eNdoDhIz"

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



// WiFi credentials
char ssid[] = "freehotspotITB";
char pass[] = "STPGanesha2025";

// Blynk timer
BlynkTimer timer;

// Terminal widget pada V2
WidgetTerminal terminal(V2);

#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR    0x77

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

// flag dan interval
bool sendData = true;
int interval = 2000;   // default 2 seconds
int timerID;

void getHigh12SectionValue(void) {
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());
  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void) {
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());
  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read();
  }
  delay(10);
}

int readWaterLevelPercent() {
  getLow8SectionValue();
  getHigh12SectionValue();

  uint32_t touch_val = 0;
  uint8_t trig_section = 0;

  for (int i = 0 ; i < 8; i++) {
    if (low_data[i] > THRESHOLD) {
      touch_val |= 1 << i;
    }
  }
  for (int i = 0 ; i < 12; i++) {
    if (high_data[i] > THRESHOLD) {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }

  while (touch_val & 0x01) {
    trig_section++;
    touch_val >>= 1;
  }

  return trig_section * 5; // setiap section 5%
}

// Kirim data ke Blynk
void sendSensorData() {
  if (!sendData) return;

  int water_percent = readWaterLevelPercent();
  Serial.print("Water Level = ");
  Serial.print(water_percent);
  Serial.println("%");

  Blynk.virtualWrite(V0, water_percent);

  // log di terminal juga
  terminal.print("Water Level = ");
  terminal.print(water_percent);
  terminal.println("%");
  terminal.flush();
}

// Handle perintah terminal
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
    timer.deleteTimer(timerID);
    interval = 3600000;  // 1 jam
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

  terminal.flush();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // default interval 2 detik
  timerID = timer.setInterval(interval, sendSensorData);

  terminal.println("Terminal ready. Type 1/2/3/4 to control ESP32.");
  terminal.flush();
}

void loop() {
  Blynk.run();
  timer.run();
}
