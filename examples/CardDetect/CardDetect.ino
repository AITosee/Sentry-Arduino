#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

// #define SENTRY_I2C
#define SENTRY_UART
#define VISION_MASK kVisionCard

char* card_classes[] = {
  "unknown",
  "forward",      "left",       "right",    "turn_around",  "park",
  "green_light",  "red_light",
  "speed_40",     "speed_60",   "speed_80",
  "check",        "cross",      "circle",   "square",       "triangle",
  "plus",         "minus",      "divide",   "equal",
  "zero",         "one",        "two",      "three",        "four",
  "five",         "six",        "seven",    "eight",        "nine",
  "unknown",
  "Aa", "Bb", "Cc", "Dd", "Ee", "Ff", "Gg", "Hh", "Ii", "Jj", "Kk", "Ll", "Mm", "Nn",
  "Oo", "Pp", "Qq", "Rr", "Ss", "Tt", "Uu", "Vv", "Ww", "Xx", "Yy", "Zz"
};

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
  printf("Sentry image_shape = %dx%d\n", sentry.cols(), sentry.rows());
  err = sentry.VisionBegin(VISION_MASK);
  printf("sentry.VisionBegin(VISION_MASK): %s[0x%x]\n", err ? "Error" : "Success", err);
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
      printf("  obj[%d]: x=%d,y=%d,w=%d,h=%d, label=%s\n", i, x, y, w, h, card_classes[l]);
    }
  }
}
