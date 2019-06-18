#include <M5StickC.h>  // ----A
#include "DHT12.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

DHT12 dht12;
Adafruit_BMP280 bme;

void setup() {
    M5.begin();
    M5.Axp.ScreenBreath(10);    // 画面の輝度を少し下げる ----B
    M5.Lcd.setRotation(3);      // 左を上にする         ----C
    M5.Lcd.setTextSize(2);      // 文字サイズを2にする
    M5.Lcd.fillScreen(BLACK);   // 背景を黒にする

    Wire.begin();               // I2Cを初期化する
    while (!bme.begin(0x76)) {  // BMP280を初期化する
        M5.Lcd.println("BMP280 init fail");
    }
}

void loop() {
    float tmp = dht12.readTemperature();
    float hum = dht12.readHumidity();
    float pressure = bme.readPressure();
    double vbat = M5.Axp.GetVbatData() * 1.1 / 1000;  // バッテリー電圧を取得 ----D

    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.printf("temp: %4.1f'C\r\n", tmp);
    M5.Lcd.printf("humid:%4.1f%%\r\n", hum);
    M5.Lcd.printf("press:%4.0fhPa\r\n", pressure / 100);
    M5.Lcd.printf("vbat: %4.2fV\r\n", vbat);
    delay(10 * 1000);
}
