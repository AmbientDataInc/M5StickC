#include <M5StickC.h>
#include "DHT12.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "esp_sleep.h"

DHT12 dht12;
Adafruit_BMP280 bme;

#define T_PERIOD     10   // Transmission period
#define S_PERIOD     290  // Silent period
RTC_DATA_ATTR static uint8_t seq; // remember number of boots in RTC Memory

uint16_t temp;
uint16_t humid;
uint16_t press;
uint16_t vbat;

void setAdvData(BLEAdvertising *pAdvertising) {
    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

    oAdvertisementData.setFlags(0x06); // BR_EDR_NOT_SUPPORTED | LE General Discoverable Mode

    std::string strServiceData = "";
    strServiceData += (char)0x0c;   // 長さ
    strServiceData += (char)0xff;   // AD Type 0xFF: Manufacturer specific data
    strServiceData += (char)0xff;   // Test manufacture ID low byte
    strServiceData += (char)0xff;   // Test manufacture ID high byte
    strServiceData += (char)seq;                   // シーケンス番号
    strServiceData += (char)(temp & 0xff);         // 温度の下位バイト
    strServiceData += (char)((temp >> 8) & 0xff);  // 温度の上位バイト
    strServiceData += (char)(humid & 0xff);        // 湿度の下位バイト
    strServiceData += (char)((humid >> 8) & 0xff); // 湿度の上位バイト
    strServiceData += (char)(press & 0xff);        // 気圧の下位バイト
    strServiceData += (char)((press >> 8) & 0xff); // 気圧の上位バイト
    strServiceData += (char)(vbat & 0xff);         // 電池電圧の下位バイト
    strServiceData += (char)((vbat >> 8) & 0xff);  // 電池電圧の上位バイト

    oAdvertisementData.addData(strServiceData);
    pAdvertising->setAdvertisementData(oAdvertisementData);
}

void setup() {
    M5.begin();
    M5.Axp.ScreenBreath(10);    // 画面の輝度を下げる
    M5.Lcd.setRotation(3);      // 左を上にする
    M5.Lcd.setTextSize(2);      // 文字サイズを2にする
    M5.Lcd.fillScreen(BLACK);   // 背景を黒にする

    Wire.begin();               // I2Cを初期化する
    while (!bme.begin(0x76)) {  // BMP280を初期化する
        M5.Lcd.println("BMP280 init fail");
    }

    temp = (uint16_t)(dht12.readTemperature() * 100);
    humid = (uint16_t)(dht12.readHumidity() * 100);
    press = (uint16_t)(bme.readPressure() / 100 * 10);
    vbat = (uint16_t)(M5.Axp.GetVbatData() * 1.1 / 1000 * 100);

    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.printf("temp: %4.1f'C\r\n", (float)temp / 100);
    M5.Lcd.printf("humid:%4.1f%%\r\n", (float)humid / 100);
    M5.Lcd.printf("press:%4.0fhPa\r\n", (float)press / 10);
    M5.Lcd.printf("vbat: %4.2fV\r\n", (float)vbat / 100);

    BLEDevice::init("AmbientEnv-02");                  // デバイスを初期化
    BLEServer *pServer = BLEDevice::createServer();    // サーバーを生成

    BLEAdvertising *pAdvertising = pServer->getAdvertising(); // アドバタイズオブジェクトを取得
    setAdvData(pAdvertising);                          // アドバタイジングデーターをセット

    pAdvertising->start();                             // アドバタイズ起動
    delay(T_PERIOD * 1000);                            // T_PERIOD秒アドバタイズする
    pAdvertising->stop();                              // アドバタイズ停止

    seq++;                                             // シーケンス番号を更新
    delay(10);
    esp_deep_sleep(1000000LL * S_PERIOD);              // S_PERIOD秒Deep Sleepする
}

void loop() {
}
