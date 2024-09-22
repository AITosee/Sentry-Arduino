#include <Arduino.h>
#include <Sentry.h>

typedef Sengo2 Sengo;

#define SENGO_I2C
// #define SENGO_UART

#ifdef SENGO_I2C
#include <Wire.h>
#endif
#ifdef SENGO_UART
#include <SoftwareSerial.h>
#define TX_PIN 11
#define RX_PIN 10
SoftwareSerial mySerial(RX_PIN, TX_PIN);
#endif

Sengo sengo;

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);

  Serial.println("Waiting for sengo initialize...");
#ifdef SENGO_I2C
  Wire.begin();
  while (SENTRY_OK != sengo.begin(&Wire)) {
    yield();
  }
#endif  // SENGO_I2C
#ifdef SENGO_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sengo.begin(&mySerial)) {
    yield();
  }
#endif  // SENGO_UART
  Serial.println("Sengo begin Success.");
}

void loop() {
  /* R + G + B = White */
  sengo.UserImageCoordinateConfig(1, 110, 110, 20, 20);
  sengo.ScreenFill(1, 255, 0, 0);
  sengo.UserImageCoordinateConfig(2, 0, 110, 20, 20);
  sengo.ScreenFill(2, 0, 255, 0);
  sengo.UserImageCoordinateConfig(3, 220, 110, 20, 20);
  sengo.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sengo.UserImageCoordinateConfig(2, 35, 110, 20, 20);
  sengo.ScreenFill(2, 0, 255, 0);
  sengo.UserImageCoordinateConfig(3, 185, 110, 20, 20);
  sengo.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sengo.UserImageCoordinateConfig(2, 70, 110, 20, 20);
  sengo.ScreenFill(2, 0, 255, 0);
  sengo.UserImageCoordinateConfig(3, 150, 110, 20, 20);
  sengo.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sengo.ScreenFill(2, 0, 255, 0, 0);
  sengo.ScreenFill(3, 0, 0, 255, 0);
  sengo.UserImageCoordinateConfig(1, 100, 100, 40, 40);
  sengo.ScreenFill(1, 255, 255, 255);
  delay(3000);
}
