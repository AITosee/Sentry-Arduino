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
  // uint8_t buf[] = {0xFF, 9, 0x60, 0x23, kVisionQrCode, 0, 0, 0x94, 0xed};
  // Serial3.write(buf, sizeof(buf));
  // for (;;) {
  //   if (Serial3.available()) {
  //     // Serial.print(Serial3.read(), HEX);
  //     // Serial.print(',');
  //     char c = Serial3.read();
  //     printf("0x%02x", c);
  //     printf("[%c],", c);
  //   }
  // }
  sentry_err_t err = sentry.begin(&Serial3);
#endif  // SENTRY_UART
  printf("sentry.begin: %s[0x%x]\n", err ? "Error" : "Success", err);
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
  err = sentry.VisionBegin(kVisionQrCode);
  printf("sentry.VisionBegin(kVisionQrCode): %s[0x%x]\n", err ? "Error" : "Success", err);
}

void loop() {
  unsigned long ts = millis();
  int obj_num = sentry.GetValue(kVisionQrCode, kStatus);
  unsigned long te = millis();
  if (obj_num) {
    printf("Totally %d objects in %lums:\n", obj_num, te - ts);
    int x = sentry.GetValue(kVisionQrCode, kXValue);
    int y = sentry.GetValue(kVisionQrCode, kYValue);
    int w = sentry.GetValue(kVisionQrCode, kWidthValue);
    int h = sentry.GetValue(kVisionQrCode, kHeightValue);
    char* c = sentry.GetQrCodeValue();
    printf("  obj: x=%d,y=%d,w=%d,h=%d, value=%s\n", x, y, w, h, c);
  }
}
