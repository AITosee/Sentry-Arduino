#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

// #define SENTRY_I2C
#define SENTRY_UART

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
  sentry_err_t err;
#ifdef SENTRY_I2C
  Wire.begin();
  err = sentry.begin(&Wire);
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  Serial3.begin(9600);
  err = sentry.begin(&Serial3);
#endif  // SENTRY_UART
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  int param_num = 4;       // 1~25
  sentry.SetParamNum(kVisionColor, param_num);
  sentry_object_t param;
  for (size_t i = 0; i < param_num; i++)
  {
    /* Set x/y/w/h */
    param.x_value = sentry.cols() * (i + 1) / (param_num + 1);
    param.y_value = 120;
    param.width = i * 2 + 1;
    param.height = i * 2 + 1;
    printf("\nSetParam[%u]: %hu,%hu,%hu,%hu\n", i, param.x_value, param.y_value,
           param.width, param.height);
    err = sentry.SetParam(kVisionColor, &param, i);
    if (err) {
      printf("sentry.SetParam: %s[0x%x]\n", err ? "Error" : "Success", err);
      for (;;);
    }
  }
  err = sentry.VisionBegin(kVisionColor);
  printf("sentry.VisionBegin(kVisionColor): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  ts = tn;
  int obj_num = sentry.GetValue(kVisionColor, kStatus);
  tn = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, tn - ts);
    for (int i = 0; i < obj_num; ++i) {
      int l = sentry.GetValue(kVisionColor, kLabel, i);
      printf("|%02d", l);
    }
    printf("|\n");
  }
}
