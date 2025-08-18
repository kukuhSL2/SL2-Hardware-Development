#include <Wire.h>
#include <Adafruit_MCP9808.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID untuk BLE Service dan Characteristic (boleh diganti sesuai kebutuhan)
#define SERVICE_UUID        "abcdefab-1234-5678-9abc-def012345678"
#define CHARACTERISTIC_UUID "12345678-abcd-4321-abcd-1234567890ef"

// Object sensor MCP9808
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// BLE pointer
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initialize MCP9808...");

  // Inisialisasi sensor MCP9808
  if (!tempsensor.begin(0x18)) { // alamat default MCP9808 adalah 0x18
    Serial.println("MCP9808 is not detected on I2C!");
    while (1) delay(10);
  }
  Serial.println("MCP9808 Successfully Attached!");

  // Set resolusi (0-3, default 0 → 0.5°C, 3 → 0.0625°C)
  tempsensor.setResolution(3);

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-MCP9808");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY // untuk kirim otomatis
  );

  pCharacteristic->setValue("Ready");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Serial.println("BLE Service begin, use BLE Scanner App");
}

void loop() {
  // Baca suhu dari MCP9808
  float temperature = tempsensor.readTempC();

  // Tampilkan di Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Kirim ke BLE (string JSON sederhana)
  String sensorData = "{ \"temp\": " + String(temperature, 2) + " }";

  pCharacteristic->setValue(sensorData.c_str());
  pCharacteristic->notify(); // kirim notifikasi ke BLE client

  delay(2000); // jeda 2 detik
}
