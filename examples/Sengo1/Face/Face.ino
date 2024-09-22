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

#define VISION_TYPE Sengo::kVisionFace
Sengo sengo;
bool is_recorded_face = false;

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
  /* 清除 ID1 的人脸记录 */
  sentry_object_t param{};
  param.label = 0;
  err = sengo.SetParam(VISION_TYPE, &param, 1);
  Serial.print("sengo clear face ID1: ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVisionFace) ");
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
      int x = sengo.GetValue(VISION_TYPE, kXValue, i);
      int y = sengo.GetValue(VISION_TYPE, kYValue, i);
      int w = sengo.GetValue(VISION_TYPE, kWidthValue, i);
      int h = sengo.GetValue(VISION_TYPE, kHeightValue, i);
      int l = sengo.GetValue(VISION_TYPE, kLabel, i);
      if (is_recorded_face == false && l == 0)
      {
        /* 记录第一个陌生人脸至人脸 ID1 内 */
        is_recorded_face = true;
        sentry_object_t param{};
        param.label = 100;
        sentry_err_t err = sengo.SetParam(VISION_TYPE, &param, 1);
        Serial.print("sengo record face ID1: ");
        if (err) {
          Serial.print("Error: 0x");
        } else {
          Serial.print("Success: 0x");
        }
        Serial.println(err, HEX);
      }
      Serial.print("  obj");
      Serial.print(i);
      Serial.print(": ");
      Serial.print("x=");
      Serial.print(x);
      Serial.print(",y=");
      Serial.print(y);
      Serial.print(",w=");
      Serial.print(w);
      Serial.print(",h=");
      Serial.print(h);
      Serial.print(",label=");
      Serial.println(l);
    }
  }
}
