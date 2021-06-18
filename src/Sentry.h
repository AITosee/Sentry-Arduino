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

#ifndef SENTRY_H_
#define SENTRY_H_

#include "hardware/hw_sentry_i2c.h"
#include "sentry_stream_base.h"
#include "sentry_uart.h"

/*
 * sentry_vision_mask_t
 */
typedef unsigned short sentry_vision_mask_t;

// sentry_vision_mask_t: Vision Type User Input
#define VISION_COLOR_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionColorDetect)
#define VISION_COLOR_RECOGNITION_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionColorRecog)
#define VISION_BALL_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionBall)
#define VISION_LINE_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionLine)
#define VISION_BODY_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionBody)
#define VISION_CARD_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionCard)
#define VISION_FACE_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionFace)
#define VISION_20CLASSES_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVision20Classes)
#define VISION_QR_CODE_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionQrCode)
#define VISION_OBJ_TRACK_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionObjTrack)
#define VISION_MOVING_OBJ_DETECT_MASK \
  (sentry_vision_mask_t) visionTypeEnumToMacro(kVisionMovingObjDetect)
#define VISION_ALL_MASK \
  (sentry_vision_mask_t)(visionTypeEnumToMacro(kVisionMaxType) - 1)

/* Sentry label */
#define SENTRY_UNKNOWN_LABEL 0U
/* Sentry card label */
#define SENTRY_CARD_FORWARD 1U
#define SENTRY_CARD_LEFT 2U
#define SENTRY_CARD_RIGHT 3U
#define SENTRY_CARD_TURN_AROUND 4U
#define SENTRY_CARD_PARK 5U
#define SENTRY_CARD_GREEN_LIGHT 6U
#define SENTRY_CARD_RED_LIGHT 7U
#define SENTRY_CARD_SPEED_40 8U
#define SENTRY_CARD_SPEED_60 9U
#define SENTRY_CARD_SPEED_80 10U
#define SENTRY_CARD_CHECK 11U
#define SENTRY_CARD_CROSS 12U
#define SENTRY_CARD_CIRCLE 13U
#define SENTRY_CARD_SQUARE 14U
#define SENTRY_CARD_TRIANGLE 15U
#define SENTRY_CARD_PLUS 16U
#define SENTRY_CARD_MINUS 17U
#define SENTRY_CARD_DIVIDE 18U
#define SENTRY_CARD_EQUAL 19U
#define SENTRY_CARD_ZERO 20U
#define SENTRY_CARD_ONE 21U
#define SENTRY_CARD_TWO 22U
#define SENTRY_CARD_THREE 23U
#define SENTRY_CARD_FOUR 24U
#define SENTRY_CARD_FIVE 25U
#define SENTRY_CARD_SIX 26U
#define SENTRY_CARD_SEVEN 27U
#define SENTRY_CARD_EIGHT 28U
#define SENTRY_CARD_NINE 29U
#define SENTRY_CARD_A 31U
#define SENTRY_CARD_B 32U
#define SENTRY_CARD_C 33U
#define SENTRY_CARD_D 34U
#define SENTRY_CARD_E 35U
#define SENTRY_CARD_F 36U
#define SENTRY_CARD_G 37U
#define SENTRY_CARD_H 38U
#define SENTRY_CARD_I 39U
#define SENTRY_CARD_J 40U
#define SENTRY_CARD_K 41U
#define SENTRY_CARD_L 42U
#define SENTRY_CARD_M 43U
#define SENTRY_CARD_N 44U
#define SENTRY_CARD_O 45U
#define SENTRY_CARD_P 46U
#define SENTRY_CARD_Q 47U
#define SENTRY_CARD_R 48U
#define SENTRY_CARD_S 49U
#define SENTRY_CARD_T 50U
#define SENTRY_CARD_U 51U
#define SENTRY_CARD_V 52U
#define SENTRY_CARD_W 53U
#define SENTRY_CARD_X 54U
#define SENTRY_CARD_Y 55U
#define SENTRY_CARD_Z 56U
/* Sentry 20 classes label */
#define SENTRY_OBJ_AIRPLANE 1U
#define SENTRY_OBJ_BICYCLE 2U
#define SENTRY_OBJ_BIRD 3U
#define SENTRY_OBJ_BOAT 4U
#define SENTRY_OBJ_BOTTLE 5U
#define SENTRY_OBJ_BUS 6U
#define SENTRY_OBJ_CAR 7U
#define SENTRY_OBJ_CAT 8U
#define SENTRY_OBJ_CHAIR 9U
#define SENTRY_OBJ_COW 10U
#define SENTRY_OBJ_TABLE 11U
#define SENTRY_OBJ_DOG 12U
#define SENTRY_OBJ_HORSE 13U
#define SENTRY_OBJ_MOTOR_BIKE 14U
#define SENTRY_OBJ_PERSON 15U
#define SENTRY_OBJ_PLANT 16U
#define SENTRY_OBJ_SHEEP 17U
#define SENTRY_OBJ_SOFA 18U
#define SENTRY_OBJ_TRAIN 19U
#define SENTRY_OBJ_MONITOR 20U
/* Sentry color label */
#define SENTRY_COLOR_BLACK 1U
#define SENTRY_COLOR_WHITE 2U
#define SENTRY_COLOR_RED 3U
#define SENTRY_COLOR_GREEN 4U
#define SENTRY_COLOR_BLUE 5U
#define SENTRY_COLOR_YELLOW 6U
/* Sentry ball type */
// #define SENTRY_BALL_TABLE_TENNIS 0x01U
// #define SENTRY_BALL_TENNIS 0x02U

class Sentry {
 public:
  /**
   * @brief  Construct class Sentry.
   * @param  address: MU vision sensor device address.
   * @retval none
   */
  Sentry(uint32_t address = 0x60);
  virtual ~Sentry();

  /**
   * @brief  MU vision sensor begin.
   * @param  communication_port: MuVsUart(uart).
   * @retval SENTRY_OK: begin success.
   *         other: protocol assert fail.
   */
  uint8_t begin(HwSentryUart::hw_uart_t communication_port);
  /**
   * @brief  MU vision sensor begin.
   * @param  communication_port: hw_i2c_t(i2c).
   * @retval SENTRY_OK: begin success.
   *         other: protocol assert fail.
   */
  uint8_t begin(hw_i2c_t* communication_port);

  // Based interface
  /**
   * @brief  begin vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionBegin(sentry_vision_e vision_type);
  /**
   * @brief  end vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionEnd(sentry_vision_e vision_type);
  /**
   * @brief  get vision result data, this function will update vision
   *         result automatically.
   * @param  vision_type: vision type.
   * @param  obj_info:  object information
   * @retval information value
   */
  int GetValue(sentry_vision_e vision_type, sentry_obj_info_e obj_info,
               int obj_id = 0);
  char* GetQrCodeValue() {
    if (qrcode_state_) {
      return qrcode_state_->qrcode_result[0].str;
    }
    return nullptr;
  }

  /**
   * @brief  Set max detect object number.
   * @param  vision_type: vision type.
   * @param  param:  vision parameter
   * @param  max_num:  max number
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t SetParamNum(sentry_vision_e vision_type, int max_num);

  /**
   * @brief  write vision parameter.
   * @param  vision_type: vision type.
   * @param  param:  vision parameter
   * @param  param_id:  parameter id
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t SetParam(sentry_vision_e vision_type, sentry_object_t* param,
                   int param_id) {
    if (param_id < 0 || param_id >= SENTRY_MAX_RESULT) {
      return SENTRY_FAIL;
    }
    return stream_->SetParam(vision_type, param, param_id);
  }
  /**
   * @brief  get vision result buffer pointer.
   *         this function WILL NOT update vision result, please use
   *         function `UpdateResult([vision_type])` or
   *         `GetValue([vision_type], kStatus)` to update vision result before
   * this function
   * @param  vision_type: vision type.
   * @retval vision result buffer pointer,
   *         return `nullptr` if the vision type is not `begin` or not supported
   */
  sentry_vision_state_t* GetVisionState(sentry_vision_e vision_type);

  // Advance interface
  /**
   * @brief  update result data from vision sensor, must used after
   *         VisionBegin or VisionSetStatus.
   * @param  vision_type vision type.
   * @param  wait_all_result  true: return if get all input vision type
   *                           false: return if get one of the input vision type
   * @retval the vision type which have been updated
   */
  uint8_t UpdateResult(sentry_vision_e vision_type);
  /**
   * @brief  read result data.
   * @param  vision_type vision type.
   * @param  obj_info  object information
   * @retval information value
   */
  uint8_t read(sentry_vision_e vision_type, sentry_obj_info_e obj_info,
               uint8_t obj_id = 0);
  uint8_t readQrCode(sentry_obj_info_e obj_info);

  // Sensor functions
  //!< @brief  restart MU vision sensor
  uint8_t SensorSetRestart(void);
  //!< @brief  set all register to default value(include baud rate)
  uint8_t SensorSetDefault(void);

  // LED functions
  /**
   * @brief  set led.
   * @param  led led type.
   * @param  manual vision type.
   * @param  hold  object information
   * @retval SENTRY_OK  success
   *         other  error
   */
  uint8_t LedSetMode(sentry_led_e led, bool manual, bool hold);
  /**
   * @brief  set led color.
   * @param  led led type.
   * @param  detected_color led color while sensor detected target.
   * @param  undetected_color led color while sensor undetected target.
   * @param  level  led brightness, form 0(close) to 15
   * @retval SENTRY_OK  success
   *         other  error
   */
  uint8_t LedSetColor(sentry_led_e led, sentry_led_color_e detected_color,
                      sentry_led_color_e undetected_color, uint8_t level = 1);

  // Camera functions
  /**
   * @brief  set camera zoom.
   * @param  camera zoom value.
   * @retval SENTRY_OK  success
   *         other  error
   */
  uint8_t CameraSetZoom(sentry_camera_zoom_e);
  /**
   * @brief  rotate camera.
   * @param  enable true: rotate camera.
   *                 false: default
   * @retval SENTRY_OK  success
   *         other  error
   */
  uint8_t CameraSetRotate(bool enable);
  /**
   * @brief  set camera FPS.
   * @param  camera FPS type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t CameraSetFPS(sentry_camera_fps_e);
  /**
   * @brief  set camera white balance.
   * @param  camera white balance type.
   * @retval SENTRY_OK  success
   *         other  error
   */
  uint8_t CameraSetAwb(sentry_camera_white_balance_e);
  /**
   * @brief  get camera zoom value.
   * @retval camera zoom value
   */
  sentry_camera_zoom_e CameraGetZoom(void);
  /**
   * @brief  get camera AWB type.
   * @retval camera AWB type
   */
  sentry_camera_white_balance_e CameraGetAwb(void);
  /**
   * @brief  get camera rotate state.
   * @retval camera rotate state
   */
  bool CameraGetRotate(void);
  /**
   * @brief  get camera FPS type.
   * @retval camera FPS type
   */
  sentry_camera_fps_e CameraGetFPS(void);

  // Uart functions
  /**
   * @brief  set UART baud rate.
   * @param  UART baud rate.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t UartSetBaudrate(sentry_baudrate_e);

  // Vision functions
  /**
   * @brief  set vision status.
   * @param  vision_type: vision type
   * @param  enable: true: enable vision
   *                 false: disable vision
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionSetStatus(sentry_vision_e vision_type, bool enable);
  /**
   * @brief  set vision configure to default value.
   * @param  vision_type: vision type
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  uint8_t VisionSetDefault(sentry_vision_e vision_type);
  /**
   * @brief  get vision status.
   * @retval vision status
   */
  bool VisionGetStatus(sentry_vision_e vision_type);
  int rows() { return (int)img_h_; }
  int cols() { return (int)img_w_; }

  Sentry(const Sentry&) = delete;
  Sentry& operator=(const Sentry&) = delete;

 private:
  uint8_t SensorInit();
  uint8_t SensorLockReg(bool lock);
  uint8_t ProtocolVersionCheck();
  uint8_t GetImageShape();
  bool malloc_vision_buffer(sentry_vision_e);
  bool free_vision_buffer(sentry_vision_e);

  uint8_t address_ = 0;
  sentry_mode_e mode_ = kUnknownMode;
  SentryStreamBase* stream_ = nullptr;
  sentry_vision_state_t* vision_state_[kVisionMaxType - 1] = {nullptr};
  sentry_qrcode_state_t* qrcode_state_ = nullptr;
  uint16_t img_w_ = 0;
  uint16_t img_h_ = 0;
};

#endif /* SENTRY_H_ */
