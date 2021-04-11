#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

// #define SENTRY_I2C
#define SENTRY_UART

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
  sentry_object_t param;
  param.x_value = 160;
  param.y_value = 120;
  param.width = 5;
  param.height = 5;
  for (size_t i = 0; i < 4; i++)
  {
    param.x_value = sentry.cols() * (i + 1) / 5;
    param.width = i * 2 + 1;
    param.height = i * 2 + 1;
    err = sentry.SetParam(kVisionColorRecog, &param, i);
    if (err) {
      printf("sentry.SetParam: %s[0x%x]\n", err ? "Error" : "Success", err);
      for (;;);
    }
  }
  err = sentry.VisionBegin(kVisionColorRecog);
  printf("sentry.VisionBegin(kVisionColorRecog): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  unsigned long ts = millis();
  int obj_num = sentry.GetValue(kVisionColorRecog, kStatus);
  unsigned long te = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, te - ts);
    for (int i = 0; i < obj_num; ++i) {
      int l = sentry.GetValue(kVisionColorRecog, kLabel, i);
      printf("|%02d", l);
      if (i && (i + 1) % 5 == 0) {
        printf("|\n");
      }
    }
  }
}
