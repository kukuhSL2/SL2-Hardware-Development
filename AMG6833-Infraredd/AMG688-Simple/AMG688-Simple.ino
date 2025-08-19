#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Seeed_AMG8833_driver.h>

// UUID BLE Service & Characteristic (bisa diganti sesuai kebutuhan)
#define SERVICE_UUID        "abcdefab-1234-5678-9abc-def012345678"
#define CHARACTERISTIC_UUID "12345678-abcd-4321-abcd-1234567890ef"

// BLE pointer
BLECharacteristic *pCharacteristic;

// AMG8833 sensor
AMG8833 amg8833;

// buffer pixel suhu (8x8 = 64 pixel)
float pixels[PIXEL_NUM] = {0};

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initialising AMG8833 Infrared Array Sensor...");

  // Inisialisasi sensor
  if (amg8833.init()) {
    Serial.println("AMG8833 init failed!!!");
    while (1) delay(100);
  }
  Serial.println("AMG8833 init OK!!!");

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-AMG8833");
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

  Serial.println("BLE Service ready, scan with BLE app...");
}

void loop() {
  // Baca suhu tiap pixel
  amg8833.read_pixel_temperature(pixels);

  // Print matrix ke Serial
  Serial.println("AMG8833 Temperature (8x8):");
  for (int i = 0; i < PIXEL_NUM; i++) {
    Serial.print(pixels[i], 2);
    Serial.print(" ");
    if ((i + 1) % 8 == 0) Serial.println();
  }

  // Buat JSON sederhana: {"pixels":[val1,val2,...,val64]}
  String json = "{ \"pixels\": [";
  for (int i = 0; i < PIXEL_NUM; i++) {
    json += String(pixels[i], 2);
    if (i < PIXEL_NUM - 1) json += ",";
  }
  json += "] }";

  // Kirim via BLE
  pCharacteristic->setValue(json.c_str());
  pCharacteristic->notify();

  Serial.println("Data sent via BLE.");
  Serial.println();

  delay(1000); // baca tiap 1 detik
}
