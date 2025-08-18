#include <Wire.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID untuk BLE Service dan Characteristic (boleh diganti sesuai kebutuhan)
#define SERVICE_UUID        "abcdefab-1234-5678-9abc-def012345678"
#define CHARACTERISTIC_UUID "12345678-abcd-4321-abcd-1234567890ef"

// Pin Flame Sensor (gunakan pin ADC ESP32S3, contoh GPIO 34)
#define FLAME_SENSOR_PIN 34  

// BLE pointer
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initialize Flame Sensor (Analog)...");

  pinMode(FLAME_SENSOR_PIN, INPUT);

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-FlameSensor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->setValue("Ready");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Serial.println("BLE Service begin, use BLE Scanner App");
}

void loop() {
  // Baca nilai analog flame sensor (0–4095 di ESP32 ADC)
  int flameValue = analogRead(FLAME_SENSOR_PIN);

  // Deteksi sederhana api (semakin dekat api, nilai biasanya makin kecil)
  bool flameDetected = (flameValue < 2000); // threshold bisa disesuaikan

  // Print ke Serial Monitor
  Serial.print("Flame Value: ");
  Serial.print(flameValue);
  Serial.print(" -> Flame Detected: ");
  Serial.println(flameDetected ? "NO" : "YES");

  // Buat JSON sederhana
  String sensorData = "{ \"flame_val\": " + String(flameValue) +
                      ", \"flame_detected\": " + String(flameDetected ? "true" : "false") + " }";

  // Kirim ke BLE
  pCharacteristic->setValue(sensorData.c_str());
  pCharacteristic->notify();

  delay(1000); // jeda 1 detik
}
