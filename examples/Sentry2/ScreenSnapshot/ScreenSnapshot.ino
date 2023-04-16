/**
 * @file ScreenSnapshot.ino
 * @author donion.yang
 * @brief 本示例程序用于介绍如何在Sentry2板载的ESP8285-WiFi芯片上获取一张图片,此例程并不适合UNO之类内存较小的开发板
 * @version 0.1
 * @date 2022-05-17
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <Sentry.h>

Sentry2 sentry;

void setup() {
  sentry_err_t err = SENTRY_OK;

  Serial.begin(4000000);
  Serial.printf("Waiting for sentry initialize...\n");

  while (SENTRY_OK != sentry.begin(&Serial)) {
    yield();
  }
  printf("Sentry begin Success.\n");
  printf("Sentry image_shape = %hux%hu\n", sentry.cols(), sentry.rows());
}

void loop() {
  sentry_err_t err = sentry.Snapshot(kSnapshot2Wifi); // Default is: kSnapshotFromCamera, kSnapshotTypeJPEG
  // err = sentry.Snapshot(kSnapshot2Wifi, kSnapshotFromCamera, kSnapshotTypeJPEGBase64); // Base64 type
  // TODO: receive an image

  delay(10000);
}
