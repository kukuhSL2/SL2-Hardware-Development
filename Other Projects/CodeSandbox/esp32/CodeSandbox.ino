#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_DPS310.h>

#define WIFI_SSID     "freehotspotITB"
#define WIFI_PASS     "STPGanesha2025"
// Example: "https://your-codesandbox-username-xyz-8080.app.github.dev" or the Preview URL
#define SERVER_URL    "https://ksjh75-8080.csb.app"  // no trailing slash

Adafruit_DPS310 dps;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(); // XIAO ESP32S3 default I2C: SDA=D4, SCL=D5

  if (!dps.begin_I2C()) { // default I2C addr 0x77
    Serial.println("DPS310 not found");
    while (1) delay(10);
  } else {
    Serial.println("DPS310 found!");
    dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected!");
}

void loop() {
  sensors_event_t temp_event, pressure_event;
  if (dps.temperatureAvailable() && dps.pressureAvailable()) {
    dps.getEvents(&temp_event, &pressure_event);

    float t = temp_event.temperature;  // °C

    // Build JSON manually
    String payload = String("{") +
      "\"deviceId\":\"xiao-esp32s3\"," +
      "\"temperature\":" + String(t, 2) + "," +
      "\"ts\":" + String((unsigned long) (millis() + 1710000000000UL)) + 
      "}";

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(String(SERVER_URL) + "/ingest");
      http.addHeader("Content-Type", "application/json");
      int code = http.POST(payload);
      Serial.printf("POST /ingest -> %d\n", code);
      if (code > 0) Serial.println(http.getString());
      http.end();
    } else {
      Serial.println("WiFi disconnected");
      WiFi.reconnect();
    }
  } else {
    Serial.println("DPS310 read not available yet");
  }

  delay(5000); // every 5s
}
