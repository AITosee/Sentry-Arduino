#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

typedef Sentry1 Sentry;

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK Sentry::kVisionLine
Sentry sentry;

unsigned long ts = millis();
unsigned long tn = ts;

int serial_putc(char c, struct __file*) {
  Serial.write(c);
  return c;
}

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(9600);
  fdevopen(&serial_putc, 0);

  printf("Waiting for sentry initialize...\n");
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sentry.begin(&Wire)) { yield(); }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  Serial3.begin(9600);
  while (SENTRY_OK != sentry.begin(&Serial3)) { yield(); }
#endif  // SENTRY_UART
  printf("Sentry begin Success.\n");
  printf("Sentry image_shape = %dx%d\n", sentry.cols(), sentry.rows());
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(kVisionLine): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  ts = tn;
  int obj_num = sentry.GetValue(VISION_MASK, kStatus);
  tn = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, tn - ts);
    for (int i = 1; i <= obj_num; ++i) {
      int x1 = sentry.GetValue(VISION_MASK, kXValue, i);
      int y1 = sentry.GetValue(VISION_MASK, kYValue, i);
      int x2 = sentry.GetValue(VISION_MASK, kWidthValue, i);
      int y2 = sentry.GetValue(VISION_MASK, kHeightValue, i);
      int degree = sentry.GetValue(VISION_MASK, kLabel, i);
      printf("  obj[%d]: x1=%d,y1=%d,x2=%d,y2=%d,degree=%d\n", i, x1, y1, x2,
             y2, degree);
    }
  }
}
