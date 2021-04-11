#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

#define VISION_MASK kVisionFace

Sentry sentry;

int serial_putc(char c, struct __file*) {
  Serial.write(c);
  return c;
}

void setup() {
  Serial.begin(9600);
  fdevopen(&serial_putc, 0);
  printf("sizeof(Sentry) = %u\n", sizeof(Sentry));
  printf("sizeof(sentry_vision_state_t) = %u\n", sizeof(sentry_vision_state_t));
  printf("Wire.begin\n");
  Wire.begin();
  sentry_err_t err = sentry.begin(&Wire);
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(VISION_FACE_DETECT_MASK): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  unsigned long ts = millis();
  int obj_num = sentry.GetValue(VISION_MASK, kStatus);
  unsigned long te = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, te - ts);
    for (int i = 0; i < obj_num; ++i) {
      int x = sentry.GetValue(VISION_MASK, kXValue, i);
      int y = sentry.GetValue(VISION_MASK, kYValue, i);
      int w = sentry.GetValue(VISION_MASK, kWidthValue, i);
      int h = sentry.GetValue(VISION_MASK, kHeightValue, i);
      int l = sentry.GetValue(VISION_MASK, kLabel, i);
      printf("  obj[%d]: x=%d,y=%d,w=%d,h=%d, label=%d\n", i, x, y, w, h, l);
    }
  }
}
