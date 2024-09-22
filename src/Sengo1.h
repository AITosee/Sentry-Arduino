// Copyright 2021 Tosee Intelligence (hangzhoou) co.,ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SENGO1_H_
#define SENGO1_H_

#include <SentryFactory.h>

namespace tosee_sentry {

#define SENGO1_DEVICE_ID 0x06

class Sengo1 : public SentryFactory {
 public:
  Sengo1(uint32_t address = 0x60)
      : SentryFactory(address, SENGO1_DEVICE_ID, product_vision_state_,
                      kVisionMaxType, kVisionQrCode) {}
  virtual ~Sengo1() {}
  Sengo1(const Sengo1 &) = delete;
  Sengo1 &operator=(const Sengo1 &) = delete;

  enum sengo_vision_e {
    kVisionColor = 1,
    kVisionBlob = 2,
    kVisionBall = 3,
    kVisionLine = 4,
    kVisionCard = 6,
    kVisionBody = 7,
    kVisionFace = 8,
    kVisionQrCode = 9,
    kVisionMaxType,
  };
  /* Sentry card label */
  enum card_label_e {
    kCardForward = 1,
    kCardLeft = 2,
    kCardRight = 3,
    kCardTurnAround = 4,
    kCardPark = 5,
  };
  /* Sentry color label */
  enum color_label_e {
    kColorBlack = 1,
    kColorWhite = 2,
    kColorRed = 3,
    kColorGreen = 4,
    kColorBlue = 5,
    kColorYellow = 6
  };
  /* Sentry ball label */
  enum ball_label_e { kBallTableTennis = 1, kBallTennis = 2 };

  /* SentryFactory label */
  enum { kUnknownLabel = 0 };
  /**
   * @brief  begin vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionBegin(sengo_vision_e vision_type) {
    return SentryFactory::VisionBegin((int)vision_type);
  }
  /**
   * @brief  end vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionEnd(sengo_vision_e vision_type) {
    return SentryFactory::VisionEnd((int)vision_type);
  }
  /**
   * @brief  Set max detect object number.
   * @param  vision_type: vision type.
   * @param  param:  vision parameter
   * @param  max_num:  max number
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t SetParamNum(sengo_vision_e vision_type, int max_num) {
    return SentryFactory::SetParamNum((int)vision_type, max_num);
  }
  /**
   * @brief  write vision parameter.
   * @param  vision_type: vision type.
   * @param  param:  vision parameter
   * @param  param_id:  parameter id
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t SetParam(sengo_vision_e vision_type, sentry_object_t *param,
                   int param_id = 1) {
    return SentryFactory::SetParam((int)vision_type, param, param_id);
  }
  /**
   * @brief  get vision result data, this function will update vision
   *         result automatically.
   * @param  vision_type: vision type.
   * @param  obj_info:  object information
   * @retval information value
   */
  int GetValue(sengo_vision_e vision_type, sentry_obj_info_e obj_info,
               int obj_id = 1) {
    return SentryFactory::GetValue((int)vision_type, obj_info, obj_id);
  }

  bool VisionGetStatus(sengo_vision_e vision_type) {
    return SentryFactory::VisionGetStatus((int)vision_type);
  }

  uint8_t VisionSetDefault(sengo_vision_e vision_type) {
    return SentryFactory::VisionSetDefault((int)vision_type);
  }

  uint8_t UpdateResult(sengo_vision_e vision_type) {
    return SentryFactory::UpdateResult((int)vision_type);
  }

  uint8_t CameraSetAwb(sentry_camera_white_balance_e) {
    /* Sengo1 DO NOT support configurate white balance */
    return SENTRY_FAIL;
  }

 private:
  sentry_vision_state_t *product_vision_state_[kVisionMaxType - 1] = {nullptr};
};

}  // namespace tosee_sentry

#endif /* SENGO1_H_ */
