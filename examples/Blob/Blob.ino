#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK kVisionBlob

const char* blob_classes[] = {
  "UNKNOWN", "BLACK", "WHITE", "RED", "GREEN", "BLUE", "YELLOW"
};

Sentry sentry;
unsigned long ts = millis();
unsigned long tn = ts;

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
  printf("Sentry image_shape = %dx%d\n", sentry.cols(), sentry.rows());
  /* Must lock white balance */
  err = sentry.CameraSetAwb(kLockWhiteBalance);
  printf("sentry.CameraLockAwb: %s[0x%x]\n", err ? "Error" : "Success", err);
  sentry_object_t param = {0};
  /* Set minimum blob size(pixel) */
  param.width = 5;
  param.height = 5;
  /* Set blob color */
  param.label = SENTRY_COLOR_RED;
  err = sentry.SetParam(VISION_MASK, &param);
  printf("sentry.SetParam(%s): %s[0x%x]\n", blob_classes[param.label], err ? "Error" : "Success", err);
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(kVisionBlob): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  ts = tn;
  int obj_num = sentry.GetValue(VISION_MASK, kStatus);
  tn = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, tn - ts);
    for (int i = 0; i < obj_num; ++i) {
      int x = sentry.GetValue(VISION_MASK, kXValue, i);
      int y = sentry.GetValue(VISION_MASK, kYValue, i);
      int w = sentry.GetValue(VISION_MASK, kWidthValue, i);
      int h = sentry.GetValue(VISION_MASK, kHeightValue, i);
      int l = sentry.GetValue(VISION_MASK, kLabel, i);
      printf("  obj[%d]: x=%d,y=%d,w=%d,h=%d, label=%s\n", i, x, y, w, h, blob_classes[l]);
    }
  }
}
