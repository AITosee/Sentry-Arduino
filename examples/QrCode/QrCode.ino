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
  err = sentry.VisionBegin(kVisionQrCode);
  printf("sentry.VisionBegin(kVisionQrCode): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  ts = tn;
  int obj_num = sentry.GetValue(kVisionQrCode, kStatus);
  tn = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, tn - ts);
    int x = sentry.GetValue(kVisionQrCode, kXValue);
    int y = sentry.GetValue(kVisionQrCode, kYValue);
    int w = sentry.GetValue(kVisionQrCode, kWidthValue);
    int h = sentry.GetValue(kVisionQrCode, kHeightValue);
    char* c = sentry.GetQrCodeValue();
    printf("  obj: x=%d,y=%d,w=%d,h=%d, value=%s\n", x, y, w, h, c);
  }
}
