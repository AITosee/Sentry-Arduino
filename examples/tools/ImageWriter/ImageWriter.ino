/**
 * @brief Sentry image writer: write image to flash.
 * Before compile:
 *  1. Sentry enable WiFi, set the baudrate to 1152000,
 *     and enable to USB.
 *  2. Convert your image to ctype code.
 *  3. Choose board `Generic ESP8285 Module (esp8266)`
 *     and Modify board config:
 *      1) Upload Speed: 57600
 *      2) Reset Method: no dtr(aka ck)
 *      3) Builtin Led: 0
 */
#include <Arduino.h>
#include <Sentry.h>
#include <Wire.h>

#include "img_linux.h"

class Sentry2ImageWriter : public Sentry2 {
 private:
  Stream *mySerial = nullptr;
 public:
  virtual ~Sentry2ImageWriter() = default;
  uint8_t begin(HwSentryUart::hw_uart_t communication_port) override {
    mySerial = communication_port;
    return Sentry2::begin(communication_port);
  }
  /**
   * @brief Write image to Sentry flash.
   * @param[in] img_id Image ID, from 0~0xFE
   * @param[in] addr Image address
   * @param[in] size Image size in byte
   * @retval SENTRY_OK: write success
   *         other: fail
   */
  uint8_t write(uint8_t img_id, const uint8_t *addr, uint32_t size) {
    uint8_t err = SENTRY_OK;

    err = stream_->Set(kRegImageWriteAddr, img_id);
    if (err) return err;

    uint8_t image_header[10] = {0};
    image_header[0] = 0xF0;
    image_header[6] = (size >> 0) & 0xFF;
    image_header[7] = (size >> 8) & 0xFF;
    image_header[8] = (size >> 16) & 0xFF;
    image_header[9] = (size >> 24) & 0xFF;
    mySerial->write(image_header, 10);
    mySerial->write(addr, size);
    mySerial->flush();

    return err;
  }
};

void setup() {
  Serial.begin(1152000);

  Sentry2ImageWriter writer{};
  printf("Sentry begin...\n");
  while (SENTRY_OK == writer.begin(&Serial)) { yield(); }
  printf("Sentry begin success!\n");

  printf("Writing image linux logo...\n");
  writer.write(0x00, img_linux_data, sizeof(img_linux_data));
  delay(100);
  printf("Image linux write ok!\n");
}

void loop() {
  yield();
}
