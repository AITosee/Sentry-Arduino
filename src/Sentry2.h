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

#ifndef SENTRY2_H_
#define SENTRY2_H_

#include <SentryFactory.h>

namespace tosee_sentry {

#define SENTRY2_DEVICE_ID 0x04

class Sentry2 : public SentryFactory {
 public:
  Sentry2(uint32_t address = 0x60)
      : SentryFactory(address, SENTRY2_DEVICE_ID, product_vision_state_,
                      kVisionMaxType, kVisionQrCode) {}
  virtual ~Sentry2() {}
  Sentry2(const Sentry2&) = delete;
  Sentry2& operator=(const Sentry2&) = delete;

  enum sentry_vision_e {
    kVisionColor = 1,
    kVisionBlob = 2,
    kVisionAprilTag = 3,
    kVisionLine = 4,
    kVisionLearning = 5,
    kVisionCard = 6,
    kVisionFace = 7,
    kVision20Classes = 8,
    kVisionQrCode = 9,
    kVisionCustom = 10,
    kVisionMotionDetect = 11,
    kVisionMaxType,
  };

  /* SentryFactory label */
  enum { kUnknownLabel = 0 };
  /* Sentry card label */
  enum card_label_e {
    kCardForward = 1,
    kCardLeft = 2,
    kCardRight = 3,
    kCardTurnAround = 4,
    kCardPark = 5,
    kCardGreenLight = 6,
    kCardRedLight = 7,
    kCardSpeed40 = 8,
    kCardSpeed60 = 9,
    kCardSpeed80 = 10,
    kCardCheck = 11,
    kCardCross = 12,
    kCardCircle = 13,
    kCardSquare = 14,
    kCardTriangle = 15,
    kCardPlus = 16,
    kCardMinus = 17,
    kCardDivide = 18,
    kCardEqual = 19,
    kCardZero = 20,
    kCardOne = 21,
    kCardTwo = 22,
    kCardThree = 23,
    kCardFour = 24,
    kCardFive = 25,
    kCardSix = 26,
    kCardSeven = 27,
    kCardEight = 28,
    kCardNine = 29,
  };
  /* SentryFactory 20 classes label */
  enum class20_label_e {
    kAirplane = 1,
    kBicycle = 2,
    kBird = 3,
    kBoat = 4,
    kBottle = 5,
    kBus = 6,
    kCar = 7,
    kCat = 8,
    kChair = 9,
    kCow = 10,
    kTable = 11,
    kDog = 12,
    kHorse = 13,
    kMotorBike = 14,
    kPerson = 15,
    kPlant = 16,
    kSheep = 17,
    kSofa = 18,
    kTrain = 19,
    kMonitor = 20
  };
  /* SentryFactory color label */
  enum color_label_e {
    kColorBlack = 1,
    kColorWhite = 2,
    kColorRed = 3,
    kColorGreen = 4,
    kColorBlue = 5,
    kColorYellow = 6
  };
  /**
   * @brief  begin vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionBegin(sentry_vision_e vision_type) {
    return SentryFactory::VisionBegin((int)vision_type);
  }
  /**
   * @brief  end vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionEnd(sentry_vision_e vision_type) {
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
  uint8_t SetParamNum(sentry_vision_e vision_type, int max_num) {
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
  uint8_t SetParam(sentry_vision_e vision_type, sentry_object_t* param,
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
  int GetValue(sentry_vision_e vision_type, sentry_obj_info_e obj_info,
               int obj_id = 1) {
    return SentryFactory::GetValue((int)vision_type, obj_info, obj_id);
  }

  bool VisionGetStatus(sentry_vision_e vision_type) {
    return SentryFactory::VisionGetStatus((int)vision_type);
  }

  uint8_t VisionSetDefault(sentry_vision_e vision_type) {
    return SentryFactory::VisionSetDefault((int)vision_type);
  }

  uint8_t UpdateResult(sentry_vision_e vision_type) {
    return SentryFactory::UpdateResult((int)vision_type);
  }

 private:
  sentry_vision_state_t* product_vision_state_[kVisionMaxType - 1] = {nullptr};
};

}  // namespace tosee_sentry

#endif /* SENTRY2_H_ */
