#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID untuk BLE Service dan Characteristic
#define SERVICE_UUID        "abcdefab-1234-5678-9abc-def012345678"
#define CHARACTERISTIC_UUID "12345678-abcd-4321-abcd-1234567890ef"

#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR    0x77

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

// BLE pointer
BLECharacteristic *pCharacteristic;

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

// fungsi untuk membaca water level (%)
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

  // setiap section = 5% (total 20 section = 100%)
  return trig_section * 5;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Initialize Grove Water Level Sensor (I2C)...");

  // Inisialisasi BLE
  BLEDevice::init("ESP32S3-WaterLevel");
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
  int water_percent = readWaterLevelPercent();

  // Tampilkan di Serial Monitor
  Serial.print("Water Level = ");
  Serial.print(water_percent);
  Serial.println("%");
  Serial.println("****************************************");

  // Kirim ke BLE (JSON sederhana)
  String sensorData = "{ \"water_level\": " + String(water_percent) + " }";
  pCharacteristic->setValue(sensorData.c_str());
  pCharacteristic->notify();

  delay(2000);
}
