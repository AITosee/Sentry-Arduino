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
  int param_num = 5;       // 1~SENTRY_MAX_RESULT
  sengo.SetParamNum(VISION_TYPE, param_num);
  sentry_object_t param;
  for (size_t i = 1; i <= param_num; i++)
  {
    /* Set x/y/w/h */
    param.x_value = 100 * i / (param_num + 1);
    param.y_value = 50;
    param.width = i * 2 + 1;
    param.height = i * 2 + 1;
    err = sengo.SetParam(VISION_TYPE, &param, i);
    if (err) {
      Serial.print("sengo.SetParam");
      Serial.print(i);
      Serial.print(" Error: 0x");
      Serial.println(err, HEX);
      for (;;);
    }
  }
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
    Serial.println("|\n");
  }
}
