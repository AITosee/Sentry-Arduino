/**
 * @brief Sentry image In/Output: write image to flash or read from camera.
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


#define MAX_IMAGE_SIZE 25 * 1024
static uint8_t image_data[MAX_IMAGE_SIZE];

/**
 * Sentry2 image In/Output class, for 8285 ONLY
 */
class Sentry2ImageIO : public Sentry2 {
 private:
  Stream *mySerial = nullptr;
 public:
  enum image_format_e {
    kFormatUnknow = 0,
    kFormatGRAY = 1,
    kFormatRGB565 = 2,
    kFormatRGB888 = 3,
    kFormatJPEG = 4,
    kFormatPNG = 5
  };
  struct image_t
  {
    uint8_t *data = nullptr;
    uint16_t width = 0;
    uint16_t height = 0;
    image_format_e format = kFormatUnknow;
    int32_t size = 0;
  };
  virtual ~Sentry2ImageIO() = default;
  uint8_t begin(HwSentryUart::hw_uart_t communication_port, bool set_default = true) override {
    mySerial = communication_port;
    return Sentry2::begin(communication_port, set_default);
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

  /**
   * @brief Read image from camera.
   * @param[in] image_dest  kSnapshot2SD/kSnapshot2Uart/kSnapshot2Usb/kSnapshot2Wifi
   */
  const image_t read(uint8_t image_dest, uint8_t *addr, uint32_t size) {
    image_t image{};
    // uint8_t snapshot_to_wifi_cmd[] = {0xFF, 0x08, 0x60, 0x01, 0x1A, 0x88, 0x0A, 0xED};
    // mySerial->write(snapshot_to_wifi_cmd, 8);
    Snapshot(image_dest);

    /**
     * @brief 读取图像帧头标识 0xF0
     *
     */
    char header_start = 0;
    while (header_start != 0xF0) {
      mySerial->readBytes(&header_start, 1);
      // printf("%.02X ", header_start);
    }

    /* 读取图像属性 */
    mySerial->readBytes((char*)&image.format, 1);     // 图像格式类型，4 = jpeg格式
    mySerial->readBytes((char*)&image.width, 2);      // 图像宽度像素值
    mySerial->readBytes((char*)&image.height, 2);     // 图像高度像素值
    mySerial->readBytes((char*)&image.size, 4);       // 图像数据大小
    // printf(
    //     "image_type: %d, image_width: %d, image_height: %d, image_size: %d\n",
    //     image.format, image.width, image.height, image.size);
    if (image.size > size) {
      mySerial->println("image size is too large !!!");
      return image;
    }
    /* 读取数据 */
    int rx_bytes = 0;
    int frame_bytes = 200;
    int left = image.size;
    uint8_t *ptr = addr;

    while (left >= frame_bytes) {
      rx_bytes = mySerial->readBytes(ptr, frame_bytes);
      left -= rx_bytes;
      ptr += rx_bytes;
    }

    if (left > 0) {
      mySerial->readBytes(ptr, left);
    }

    image.data = addr;

    return image;
  }
};

void setup() {
  Serial.begin(1152000);

  Sentry2ImageIO image_io{};
  printf("Sentry begin...\n");
  while (SENTRY_OK != image_io.begin(&Serial, false)) {
    yield();
  }
  printf("Sentry begin success!\n");

  // 图像写入 flash
  printf("Writing image linux logo...\n");
  image_io.write(0x00, img_linux_data, sizeof(img_linux_data));
  delay(100);
  printf("Image linux write ok!\n");
  // 图像读取
  printf("Reading image...\n");
  const Sentry2ImageIO::image_t image = image_io.read(kSnapshot2Wifi, image_data, MAX_IMAGE_SIZE);
  if (image.data) {
    printf("Image read Success! shape = (%hux%hu)\n", image.width, image.height);
  } else {
    printf("Image read fail!\n");
  }
}

void loop() {
  yield();
}
