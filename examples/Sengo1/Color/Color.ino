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

#define VISION_TYPE Sengo::kVisionColor
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
  sentry_object_t param;
  param.x_value = sengo.cols() / 2;
  param.y_value = sengo.rows() / 2;
  param.width = 5;
  param.height = 5;
  err = sengo.SetParam(VISION_TYPE, &param);
  if (err) {
    Serial.print("sengo.SetParam ");
    if (err) {
      Serial.print("Error: 0x");
    } else {
      Serial.print("Success: 0x");
    }
    Serial.println(err, HEX);
    for (;;);
  }
  /* Sengo1 DO NOT support cofigurate white balance, skip.
  err = sengo.CameraSetAwb(kLockWhiteBalance);
  Serial.print("sengo.CameraLockAwb ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
   */
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVisionColor) ");
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
      int l = sengo.GetValue(VISION_TYPE, kLabel, i);
      Serial.print('|');
      Serial.print(l);
    }
    Serial.println('|');
  }
}
