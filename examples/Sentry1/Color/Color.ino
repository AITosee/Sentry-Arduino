#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

typedef Sentry1 Sentry;

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK Sentry::kVisionColor
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
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  sentry_object_t param;
  param.x_value = sentry.cols() / 2;
  param.y_value = sentry.rows() / 2;
  param.width = 5;
  param.height = 5;
  err = sentry.SetParam(VISION_MASK, &param);
  if (err) {
    printf("sentry.SetParam: %s[0x%x]\n", err ? "Error" : "Success", err);
    for (;;);
  }
  sentry.CameraSetAwb(kLockWhiteBalance);
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(kVisionColor): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  ts = tn;
  int obj_num = sentry.GetValue(VISION_MASK, kStatus);
  tn = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, tn - ts);
    for (int i = 1; i <= obj_num; ++i) {
      int l = sentry.GetValue(VISION_MASK, kLabel, i);
      printf("|%02d", l);
    }
    printf("|\n");
  }
}
