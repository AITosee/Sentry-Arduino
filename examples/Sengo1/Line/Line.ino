#include <Arduino.h>
#include <Sentry.h>

typedef Sengo1 Sengo;

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

#define VISION_TYPE Sengo::kVisionLine
Sengo sengo;

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);

  Serial.println("Waiting for sengo initialize...");
#ifdef SENGO_I2C
  Wire.begin();
  while (SENTRY_OK != sengo.begin(&Wire)) { yield(); }
#endif  // SENGO_I2C
#ifdef SENGO_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sengo.begin(&mySerial)) { yield(); }
#endif  // SENGO_UART
  Serial.println("Sengo begin Success.");
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVisionLine) ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
}

void loop() {
  int obj_num = sengo.GetValue(VISION_TYPE, kStatus);
  if (obj_num) {
    Serial.print("Totally ");
    Serial.print(obj_num);
    Serial.println(" objects");
    for (int i = 1; i <= obj_num; ++i) {
      int x1 = sengo.GetValue(VISION_TYPE, kXValue, i);
      int y1 = sengo.GetValue(VISION_TYPE, kYValue, i);
      int x2 = sengo.GetValue(VISION_TYPE, kWidthValue, i);
      int y2 = sengo.GetValue(VISION_TYPE, kHeightValue, i);
      int degree = sengo.GetValue(VISION_TYPE, kLabel, i);
      Serial.print("  obj");
      Serial.print(i);
      Serial.print(": ");
      Serial.print("x1=");
      Serial.print(x1);
      Serial.print(",y1=");
      Serial.print(y1);
      Serial.print(",x2=");
      Serial.print(x2);
      Serial.print(",y2=");
      Serial.print(y2);
      Serial.print(",degree=");
      Serial.println(degree);
    }
  }
}
