#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK Sentry::kVisionColor

typedef Sentry2 Sentry;

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
#ifdef SENTRY_I2C
  Wire.begin();
  while (SENTRY_OK != sentry.begin(&Wire)) { yield(); }
#endif  // SENTRY_I2C
#ifdef SENTRY_UART
  Serial3.begin(9600);
  while (SENTRY_OK != sentry.begin(&Serial3)) { yield(); }
#endif  // SENTRY_UART
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  printf("SENTRY_MAX_RESULT = %d\n", SENTRY_MAX_RESULT);
  sentry.SeneorSetCoordinateType(kAbsoluteCoordinate);
  int param_num = 4;       // 1~SENTRY_MAX_RESULT
  sentry.SetParamNum(VISION_MASK, param_num);
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
    err = sentry.SetParam(VISION_MASK, &param, i);
    if (err) {
      printf("sentry.SetParam: %s[0x%x]\n", err ? "Error" : "Success", err);
      for (;;);
    }
  }
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
