#include <Arduino.h>
#include <Sentry.h>

typedef Sengo2 Sengo;

#define SENTRY_I2C
// #define SENTRY_UART

#ifdef SENTRY_I2C
#include <Wire.h>
#endif
#ifdef SENTRY_UART
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
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sengo.begin(&Wire)) {
    yield();
  }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sengo.begin(&mySerial)) {
    yield();
  }
#endif  // SENTRY_UART
  Serial.println("Sengo begin Success.");
  sengo.UserImageCoordinateConfig(1, 0, 0, 240, 240);
  sengo.ScreenShowFromFlash(1);
  delay(5000);
  sengo.ScreenShowFromFlash(1, 0);
}

void loop() {
}
