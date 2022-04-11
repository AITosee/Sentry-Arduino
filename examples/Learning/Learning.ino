#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

typedef Sentry2 Sentry;

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK Sentry::kVisionLearning

Sentry sentry;

int serial_putc(char c, struct __file*) {
  Serial.write(c);
  return c;
}

void setup() {
  Serial.begin(9600);
  fdevopen(&serial_putc, 0);
#ifdef SENTRY_I2C
  Wire.begin();
  sentry_err_t err = sentry.begin(&Wire);
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  Serial3.begin(9600);
  sentry_err_t err = sentry.begin(&Serial3);
#endif  // SENTRY_UART
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(kVisionLearning): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  unsigned long ts = millis();
  int obj_num = sentry.GetValue(VISION_MASK, kStatus);
  unsigned long te = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, te - ts);
    for (int i = 1; i <= obj_num; ++i) {
      int l = sentry.GetValue(VISION_MASK, kLabel, i);
      printf("  obj[%d]: label=%d\n", i, l);
    }
  }
}
