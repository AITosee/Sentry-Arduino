#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

typedef Sentry2 Sentry;

#define SENTRY_I2C
// #define SENTRY_UART

Sentry sentry;

int serial_putc(char c, struct __file*) {
  Serial.write(c);
  return c;
}

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);
  fdevopen(&serial_putc, 0);
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sentry.begin(&Wire)) {
    yield();
  }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  Serial3.begin(9600);
  while (SENTRY_OK != sentry.begin(&Serial3)) {
    yield();
  }
#endif  // SENTRY_UART
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
}

void loop() {
  /* R + G + B = White */
  sentry.UserImageCoordinateConfig(1, 110, 110, 20, 20);
  sentry.ScreenFill(1, 255, 0, 0);
  sentry.UserImageCoordinateConfig(2, 0, 110, 20, 20);
  sentry.ScreenFill(2, 0, 255, 0);
  sentry.UserImageCoordinateConfig(3, 220, 110, 20, 20);
  sentry.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sentry.UserImageCoordinateConfig(2, 35, 110, 20, 20);
  sentry.ScreenFill(2, 0, 255, 0);
  sentry.UserImageCoordinateConfig(3, 185, 110, 20, 20);
  sentry.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sentry.UserImageCoordinateConfig(2, 70, 110, 20, 20);
  sentry.ScreenFill(2, 0, 255, 0);
  sentry.UserImageCoordinateConfig(3, 150, 110, 20, 20);
  sentry.ScreenFill(3, 0, 0, 255);
  delay(1000);
  sentry.ScreenFill(2, 0, 255, 0, 0);
  sentry.ScreenFill(3, 0, 0, 255, 0);
  sentry.UserImageCoordinateConfig(1, 100, 100, 40, 40);
  sentry.ScreenFill(1, 255, 255, 255);
  delay(3000);
}
