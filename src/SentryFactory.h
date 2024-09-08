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

#ifndef SENTRY_FACTORY_H_
#define SENTRY_FACTORY_H_

#include "sentry_i2c.h"
#include "sentry_stream_base.h"
#include "sentry_uart.h"

namespace tosee_sentry {

class SentryFactory {
 public:
  /**
   * @brief  Construct class SentryFactory.
   * @param  address: Sentry device address.
   * @param  device_id: Sentry device address.
   * @retval none
   */
  SentryFactory(uint32_t address, uint8_t device_id,
                sentry_vision_state_t** vision_state, const int vision_max_type,
                const int vision_qrcode_type = 0);
  virtual ~SentryFactory();

  /**
   * @brief  Sentry begin with Serial.
   * @param  communication_port: uart port.
   * @retval SENTRY_OK: begin success.
   *         other: protocol assert fail.
   */
  virtual uint8_t begin(HwSentryUart::hw_uart_t communication_port,
                        bool set_default = true);
  /**
   * @brief  Sentry begin with I2c.
   * @param  communication_port: i2c port.
   * @retval SENTRY_OK: begin success.
   *         other: protocol assert fail.
   */
  virtual uint8_t begin(HwSentryI2C::hw_i2c_t* communication_port,
                        bool set_default = true);

  // Based interface
  /**
   * @brief  begin vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t VisionBegin(int vision_type);
  /**
   * @brief  end vision.
   * @param  vision_type: vision type.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t VisionEnd(int vision_type);
  /**
   * @brief  get vision result data, this function will update vision
   *         result automatically.
   * @param  vision_type: vision type.
   * @param  obj_info:  object information
   * @retval information value
   */
  virtual int GetValue(int vision_type, sentry_obj_info_e obj_info,
                       int obj_id = 1);
  virtual char* GetQrCodeValue() {
    if (qrcode_state_) {
      return qrcode_state_->qrcode_result[0].str;
    }
    return nullptr;
  }

  /**
   * @brief  Get version max detect object number.
   * @param  vision_type: vision type.
   * @retval -1:  fail
   *         other:  max detect object number
   */
  virtual int GetParamNum(int vision_type);

  /**
   * @brief  Set max detect object number.
   * @param  vision_type: vision type.
   * @param  max_num:  max number
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t SetParamNum(int vision_type, int max_num);

  /**
   * @brief  write vision parameter.
   * @param  vision_type: vision type.
   * @param  param:  vision parameter
   * @param  param_id:  parameter id
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t SetParam(int vision_type, sentry_object_t* param,
                           int param_id = 1) {
    if (param_id <= 0 || param_id > SENTRY_MAX_RESULT) {
      return SENTRY_FAIL;
    }
    return stream_->SetParam(vision_type, param, param_id);
  }
  /**
   * @brief  get vision result buffer pointer.
   *         this function WILL NOT update vision result, please use
   *         function `UpdateResult([vision_type])` or
   *         `GetValue([vision_type], kStatus)` to update vision result before
   *         this function
   * @param  vision_type: vision type.
   * @retval vision result buffer pointer,
   *         return `nullptr` if the vision type is not `begin` or not supported
   */
  virtual const sentry_vision_state_t* GetVisionState(int vision_type);
  virtual uint8_t SetVisionState(int vision_type, sentry_vision_state_t& state);

  // Advance interface
  /**
   * @brief  update result data from vision sensor, must used after
   *         VisionBegin or VisionSetStatus.
   * @param  vision_type vision type.
   * @param  wait_all_result  true: return if get all input vision type
   *                           false: return if get one of the input vision type
   * @retval the vision type which have been updated
   */
  virtual uint8_t UpdateResult(int vision_type);
  /**
   * @brief  read result data.
   * @param  vision_type vision type.
   * @param  obj_info  object information
   * @retval information value
   */
  virtual int read(int vision_type, sentry_obj_info_e obj_info,
                   uint8_t obj_id = 1);
  virtual int readQrCode(sentry_obj_info_e obj_info);

  // Sensor functions
  //!< @brief  restart Sentry
  virtual uint8_t SensorSetRestart(void);
  //!< @brief  set all register to default value(include baud rate)
  virtual uint8_t SensorSetDefault(bool vision_default_only = true);
  /**
   * @brief  Set result coordinate type.
   * @param  type coordinate type.
   * @retval SENTRY_OK  success
   *         other  error
   */
  virtual uint8_t SeneorSetCoordinateType(sentry_coordinate_type_e type);

  // LED functions
  /**
   * @brief  Set led color, if detected_color == undetected_color, LED will
   * always on.
   * @param  detected_color led color while sensor detected target.
   * @param  undetected_color led color while sensor undetected target.
   * @param  level  led brightness, form 0(close) to 15
   * @retval SENTRY_OK  success
   *         other  error
   */
  virtual uint8_t LedSetColor(sentry_led_color_e detected_color,
                              sentry_led_color_e undetected_color,
                              uint8_t level = 1);

  // Camera functions
  /**
   * @brief  set camera zoom.
   * @param  camera zoom value.
   * @retval SENTRY_OK  success
   *         other  error
   */
  virtual uint8_t CameraSetZoom(sentry_camera_zoom_e);
  /**
   * @brief  set camera white balance.
   * @param  camera white balance type.
   * @retval SENTRY_OK  success
   *         other  error
   */
  virtual uint8_t CameraSetAwb(sentry_camera_white_balance_e);
  /**
   * @brief  get camera zoom value.
   * @retval camera zoom value
   */
  virtual sentry_camera_zoom_e CameraGetZoom(void);
  /**
   * @brief  get camera AWB type.
   * @retval camera AWB type
   */
  virtual sentry_camera_white_balance_e CameraGetAwb(void);
  /**
   * @brief  get camera rotate state.
   * @retval camera rotate state
   */
  virtual bool CameraGetRotate(void);
  /**
   * @brief  get camera FPS type.
   * @retval camera FPS type
   */
  virtual sentry_camera_fps_e CameraGetFPS(void);

  // Uart functions
  /**
   * @brief  set UART baud rate.
   * @param  UART baud rate.
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t UartSetBaudrate(sentry_baudrate_e);

  /**
   * @brief Start to take a snapshot from camera/screen to SD
   * card/UART/USB/WIFI. Receive Image data by ImageReceive functions
   * @param image_dest Send image to SD or UART or USB or WIFI ports
   * @param image_src Image capture from camera or screen
   * @param image_type Snapshot image format
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t Snapshot(
      uint8_t image_dest, sentry_snapshot_src_e image_src = kSnapshotFromCamera,
      sentry_snapshot_type_e image_type = kSnapshotTypeJPEG);

  // Screen functions
  /**
   * @brief User image coordinate config.
   * @param image_id Image ID, 1~8
   * @param x_value X value
   * @param y_value Y value
   * @param width Image width
   * @param height Image height
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t UserImageCoordinateConfig(uint8_t image_id, uint16_t x_value,
                                            uint16_t y_value, uint16_t width,
                                            uint16_t height);
  /**
   * @brief Screen config
   * @param enable Enable/Disable screen
   * @param only_user_image Only display user image and don't display image from
   * camera
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t ScreenConfig(bool enable, bool only_user_image = false);
  /**
   * @brief Show user image(from SD card) on screen.
   * @param image_id Image ID
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t ScreenShow(uint8_t image_id, uint8_t auto_reload = true);
  /**
   * @brief Show user image(from flash) on screen.
   * @param image_id Image ID
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t ScreenShowFromFlash(uint8_t image_id,
                                      uint8_t auto_reload = true);
  /**
   * @brief Fill the screen with colored(RGB) block.
   * @param image_id Image ID
   * @param r Red channel value
   * @param g Green channel value
   * @param b Blue channel value
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t ScreenFill(uint8_t image_id, uint8_t r, uint8_t g, uint8_t b,
                             uint8_t auto_reload = true);

  // Vision functions
  /**
   * @brief  set vision status.
   * @param  vision_type: vision type
   * @param  enable: true: enable vision
   *                 false: disable vision
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t VisionSetStatus(int vision_type, bool enable);
  /**
   * @brief  set vision configure to default value.
   * @param  vision_type: vision type
   * @retval SENTRY_OK:  success
   *         other:  error
   */
  virtual uint8_t VisionSetDefault(int vision_type);
  /**
   * @brief  get vision status.
   * @retval vision status
   */
  virtual bool VisionGetStatus(int vision_type);
  /**
   * @brief  set vision mode.
   * @retval vision mode
   */
  virtual uint8_t VisionSetMode(int vision_type, int mode);
  /**
   * @brief  Get vision mode.
   * @retval vision mode
   */
  virtual uint8_t VisionGetMode(int vision_type, int *mode);
  /**
   * @brief  set vision level.
   * @retval vision Level
   */
  virtual uint8_t VisionSetLevel(int vision_type, sentry_vision_level_e level);
  /**
   * @brief  get vision level.
   * @retval vision Level
   */
  virtual uint8_t VisionGetLevel(int vision_type, sentry_vision_level_e *level);
  virtual int rows() { return (int)img_h_; }
  virtual int cols() { return (int)img_w_; }

  SentryFactory(const SentryFactory&) = delete;
  SentryFactory(SentryFactory&&) = delete;
  SentryFactory& operator=(const SentryFactory&) = delete;

 protected:
  uint8_t SensorInit(bool set_default = true);
  uint8_t SensorLockReg(bool lock);
  uint8_t SensorStartupCheck();
  uint8_t ProtocolVersionCheck();
  uint8_t GetImageShape();
  bool malloc_vision_buffer(int);
  bool free_vision_buffer(int);

  sentry_mode_e mode_ = kUnknownMode;
  const uint8_t device_id_;
  uint8_t address_ = 0;
  uint16_t img_w_ = 0;
  uint16_t img_h_ = 0;
  const int vision_max_type_;
  const int vision_qrcode_type_;
  SentryStreamBase* stream_ = nullptr;
  sentry_vision_state_t** vision_state_ = nullptr;
  sentry_qrcode_state_t* qrcode_state_ = nullptr;
};

}  // namespace tosee_sentry

#endif /* SENTRY_FACTORY_H_ */
