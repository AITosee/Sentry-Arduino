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

#define VISION_TYPE Sengo::kVision20Classes
Sengo sengo;

const char* classes20[] = {"unknown",
                           "Airplane",
                           "Bicycle",
                           "Bird",
                           "Boat",
                           "Bottle",
                           "Bus",
                           "Car",
                           "Cat",
                           "Chair",
                           "Cow",
                           "Dining Table",
                           "Dog",
                           "Horse",
                           "Motorbike",
                           "Person",
                           "Potted Plant",
                           "Sheep",
                           "Sofa",
                           "Train",
                           "TV Monitor"};

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);

  Serial.println("Waiting for sengo initialize...");
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sengo.begin(&Wire)) { yield(); }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  mySerial.begin(9600);
  while (SENTRY_OK != sengo.begin(&mySerial)) { yield(); }
#endif  // SENTRY_UART
  Serial.println("Sengo begin Success.");
  err = sengo.VisionBegin(VISION_TYPE);
  Serial.print("sengo.VisionBegin(kVision20Classes) ");
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
      Serial.println(classes20[l]);
    }
  }
}
