#include <Arduino.h>
#include <Sentry.h>

typedef Sentry1 Sentry;

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

#define VISION_TYPE Sentry::kVisionColor
Sentry sentry;

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);

  Serial.println("Waiting for sentry initialize...");
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sentry.begin(&Wire)) { yield(); }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sentry.begin(&mySerial)) { yield(); }
#endif  // SENTRY_UART
  Serial.println("Sentry begin Success.");
  sentry_object_t param;
  param.x_value = sentry.cols() / 2;
  param.y_value = sentry.rows() / 2;
  param.width = 5;
  param.height = 5;
  err = sentry.SetParam(VISION_TYPE, &param);
  if (err) {
    Serial.print("sentry.SetParam ");
    if (err) {
      Serial.print("Error: 0x");
    } else {
      Serial.print("Success: 0x");
    }
    Serial.println(err, HEX);
    for (;;);
  }
  err = sentry.CameraSetAwb(kLockWhiteBalance);
  Serial.print("sentry.CameraLockAwb ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
  err = sentry.VisionBegin(VISION_TYPE);
  Serial.print("sentry.VisionBegin(kVisionColor) ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
}

void loop() {
  int obj_num = sentry.GetValue(VISION_TYPE, kStatus);
  if (obj_num) {
    Serial.print("Totally ");
    Serial.print(obj_num);
    Serial.println(" objects");
    for (int i = 1; i <= obj_num; ++i) {
      int l = sentry.GetValue(VISION_TYPE, kLabel, i);
      Serial.print('|');
      Serial.print(l);
    }
    Serial.println('|');
  }
}
