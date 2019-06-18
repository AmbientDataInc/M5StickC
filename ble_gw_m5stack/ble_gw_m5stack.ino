#include <M5Stack.h>
#include "BLEDevice.h"

uint8_t seq; // remember number of boots in RTC Memory
#define MyManufacturerId 0xffff  // test manufacturer ID

BLEScan* pBLEScan;

void setup() {
    M5.begin();

    M5.Lcd.setTextSize(3);

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(false);
}

void loop() {
    float temp, humid, press, vbat;

    BLEScanResults foundDevices = pBLEScan->start(3);  // スキャンする
    int count = foundDevices.getCount();
    for (int i = 0; i < count; i++) {
        BLEAdvertisedDevice d = foundDevices.getDevice(i);
        if (d.haveManufacturerData()) {  // ManufacturerDataがあるパケットを探す
            std::string data = d.getManufacturerData();
            int manu = data[1] << 8 | data[0];
            if (manu == MyManufacturerId && seq != data[2]) {  // カンパニーIDが0xFFFFで、
                                                               // シーケンス番号が新しいものを探す
                seq = data[2];
                temp = (float)(data[4] << 8 | data[3]) / 100.0;  // パケットから温度を取得
                humid = (float)(data[6] << 8 | data[5]) / 100.0;
                press = (float)(data[8] << 8 | data[7]) * 10.0 / 100.0;
                vbat = (float)(data[10] << 8 | data[9]) / 100.0;

                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.setCursor(20,  20); M5.Lcd.printf("seq: %d\r\n", seq);
                M5.Lcd.setCursor(20,  60); M5.Lcd.printf("temp: %4.1f'C\r\n", temp);
                M5.Lcd.setCursor(20, 100); M5.Lcd.printf("humid:%4.1f%%\r\n", humid);
                M5.Lcd.setCursor(20, 140); M5.Lcd.printf("press:%4.0fhPa\r\n", press);
                M5.Lcd.setCursor(20, 180); M5.Lcd.printf("vbat: %4.2fV\r\n", vbat);
            }
        }
    }
}
