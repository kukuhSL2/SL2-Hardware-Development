#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_DPS310.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID untuk BLE Service dan Characteristic (boleh diganti sesuai kebutuhan)
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-ba0987654321"

// Object sensor DPS310
Adafruit_DPS310 dps;

// BLE pointer
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initialize DPS310...");

  // Inisialisasi I2C sensor DPS310
  if (!dps.begin_I2C()) {
    Serial.println("DPS310 is not detected on I2C!");
    while (1) delay(10);
  }
  Serial.println("DPS310 Successfully Attached!");

  // Konfigurasi mode pengukuran
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-DPS310");
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
  sensors_event_t temp_event, pressure_event;

  // Baca data sensor
  dps.getEvents(&temp_event, &pressure_event);

  float temperature = temp_event.temperature;
  float pressure = pressure_event.pressure;

  // Tampilkan di Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  // Kirim ke BLE (dalam bentuk string JSON sederhana)
  String sensorData = "{ \"temp\": " + String(temperature, 2) +
                      ", \"pressure\": " + String(pressure, 2) + " }";

  pCharacteristic->setValue(sensorData.c_str());
  pCharacteristic->notify(); // kirim notifikasi ke BLE client

  delay(2000); // jeda 2 detik
}
