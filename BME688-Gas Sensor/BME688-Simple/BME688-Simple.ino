#include <Wire.h>
#include <Adafruit_BME680.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID untuk BLE Service dan Characteristic (boleh diganti sesuai kebutuhan)
#define SERVICE_UUID        "abcdefab-1234-5678-9abc-def012345678"
#define CHARACTERISTIC_UUID "12345678-abcd-4321-abcd-1234567890ef"

// Object sensor BME688 (pakai Adafruit BME680 driver)
Adafruit_BME680 bme; 

// BLE pointer
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initialize BME688...");

  // Inisialisasi sensor BME688 (alamat default 0x76 atau 0x77)
  if (!bme.begin(0x76)) {
    Serial.println("BME688 not detected on I2C!");
    while (1) delay(10);
  }
  Serial.println("BME688 Successfully Attached!");

  // Set konfigurasi sensor
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // suhu heater 320°C selama 150 ms

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-BME688");
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
  // Tunggu data tersedia
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    delay(2000);
    return;
  }

  // Ambil data sensor
  float temperature = bme.temperature;        // °C
  float pressure = bme.pressure / 100.0;      // hPa
  float humidity = bme.humidity;              // %
  float gas_resistance = bme.gas_resistance / 1000.0; // kΩ

  // Tampilkan di Serial Monitor
  Serial.print("Temp: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Gas: "); Serial.print(gas_resistance); Serial.println(" kΩ");
  Serial.println("---------------------------");

  // Buat JSON sederhana untuk BLE
  String sensorData = "{ \"temp\": " + String(temperature, 2) +
                      ", \"press\": " + String(pressure, 2) +
                      ", \"hum\": " + String(humidity, 2) +
                      ", \"gas\": " + String(gas_resistance, 2) + " }";

  // Kirim ke BLE
  pCharacteristic->setValue(sensorData.c_str());
  pCharacteristic->notify();

  delay(2000); // jeda 2 detik
}
