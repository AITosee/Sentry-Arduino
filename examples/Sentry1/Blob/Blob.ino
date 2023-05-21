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

#define VISION_TYPE Sentry::kVisionBlob
Sentry sentry;

const char* blob_classes[] = {
  "UNKNOWN", "BLACK", "WHITE", "RED", "GREEN", "BLUE", "YELLOW"
};

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
  /* Must lock white balance */
  err = sentry.CameraSetAwb(kLockWhiteBalance);
  Serial.print("sentry.CameraLockAwb ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
  sentry.SetParamNum(VISION_TYPE, 1);
  sentry_object_t param = {0};
  /* Set minimum blob size(pixel) */
  param.width = 5;
  param.height = 5;
  /* Set blob color */
  param.label = Sentry::kColorRed;
  err = sentry.SetParam(VISION_TYPE, &param);
  Serial.print("sentry.SetParam ");
  Serial.print(blob_classes[param.label]);
  Serial.print(" ");
  if (err) {
    Serial.print("Error: 0x");
  } else {
    Serial.print("Success: 0x");
  }
  Serial.println(err, HEX);
  err = sentry.VisionBegin(VISION_TYPE);
  Serial.print("sentry.VisionBegin(kVisionBlob) ");
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
      int x = sentry.GetValue(VISION_TYPE, kXValue, i);
      int y = sentry.GetValue(VISION_TYPE, kYValue, i);
      int w = sentry.GetValue(VISION_TYPE, kWidthValue, i);
      int h = sentry.GetValue(VISION_TYPE, kHeightValue, i);
      int l = sentry.GetValue(VISION_TYPE, kLabel, i);
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
      Serial.println(blob_classes[l]);
    }
  }
}
