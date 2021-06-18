#include "Sentry.h"

#include "sentry_type.h"

Sentry::Sentry(uint32_t address) : address_(address) {}

Sentry::~Sentry() {
  if (stream_) {
    stream_ = nullptr;
    delete stream_;
  }
  for (int i = 1; i < kVisionMaxType; i++) {
    free_vision_buffer(sentry_vision_e(i));
  }
}

uint8_t Sentry::ProtocolVersionCheck() {
  uint8_t device_id = 0;
  uint8_t firmware_version = 0;
  int err_count = 0;
  sentry_err_t err;
  for (;;) {
    if (++err_count > 3) return SENTRY_UNKNOWN_PROTOCOL;
    err = stream_->Get(kRegDeviceId, &device_id);
    // TODO: 版本号遵循 MMP(Major.Minor.Patch) 规范，库本身只检查
    // Major.Minor，若版本不对发出警告。 err =
    // stream_->Get(kRegFirmwareVersion, &firmware_version); if (!err &&
    // firmware_version > SENTRY_FIRMWARE_VERSION) {
    //   printf("Hardware firmware version is latter than library support
    //   firmware version,\n"
    //          "it may cause");
    // }
    if (!err && device_id == SENTRY_DEVICE_ID) break;
  }
  return err;
}

uint8_t Sentry::GetImageShape() {
  sentry_err_t err;
  uint8_t tmp[2] = {0};

  err = stream_->Get(kRegFrameWidthL, tmp);
  if (err) return err;
  err = stream_->Get(kRegFrameWidthH, tmp + 1);
  if (err) return err;
  img_w_ = tmp[1] << 8 | tmp[0];
  err = stream_->Get(kRegFrameHeightL, tmp);
  if (err) return err;
  err = stream_->Get(kRegFrameHeightH, tmp + 1);
  if (err) return err;
  img_h_ = tmp[1] << 8 | tmp[0];

  return SENTRY_OK;
}

uint8_t Sentry::SensorInit() {
  sentry_err_t err;

  /* Check sentry protocol version */
  err = ProtocolVersionCheck();
  if (err) return err;
  /* Sensor set default if version is correction. */
  err = SensorSetDefault();
  /* Get sensor image shape. */
  err = GetImageShape();
  if (err) return err;

  return SENTRY_OK;
}

uint8_t Sentry::begin(HwSentryUart::hw_uart_t communication_port) {
  if (mode_ == kSerialMode) {
    return SENTRY_OK;
  }
  if (stream_) {
    delete stream_;
    stream_ = nullptr;
  }

  mode_ = kSerialMode;
  stream_ =
      new SentryUart((HwSentryUart::hw_uart_t)communication_port, address_);

  return SensorInit();
}

uint8_t Sentry::begin(hw_i2c_t *communication_port) {
  if (mode_ == kI2CMode) {
    return SENTRY_OK;
  }
  if (stream_) {
    delete stream_;
    stream_ = nullptr;
  }

  mode_ = kI2CMode;
  stream_ = new HwSentryI2C(communication_port, address_);

  return SensorInit();
}

// Advance interface
uint8_t Sentry::VisionBegin(sentry_vision_e vision_type) {
  sentry_err_t err;
  err = VisionSetStatus(vision_type, true);
  if (err) return err;
  return SENTRY_OK;
}

uint8_t Sentry::VisionEnd(sentry_vision_e vision_type) {
  return VisionSetStatus(vision_type, false);
}

int Sentry::GetValue(sentry_vision_e vision_type, sentry_obj_info_e obj_info,
                     int obj_id) {
  if (obj_info == kStatus) {
    while (UpdateResult(vision_type))
      ;
  }

  if (kVisionQrCode == vision_type) {
    return (int)readQrCode(obj_info);
  } else {
    return (int)read(vision_type, obj_info, obj_id);
  }
}

uint8_t Sentry::SetParamNum(sentry_vision_e vision_type, int max_num) {
  sentry_err_t err;

  err = stream_->Set(kRegVisionId, vision_type);
  if (err) return err;

  err = stream_->Set(kRegParamNum, max_num);

  return err;
}

sentry_vision_state_t *Sentry::GetVisionState(sentry_vision_e vision_type) {
  return vision_state_[vision_type - 1];
}

uint8_t Sentry::VisionSetStatus(sentry_vision_e vision_type, bool enable) {
  sentry_err_t err;
  sentry_vision_conf1_t vision_config1;

  err = stream_->Set(kRegVisionId, vision_type);
  if (err) return err;
  err =
      stream_->Get(kRegVisionConfig1, &vision_config1.vision_config_reg_value);
  if (err) return err;
  if (vision_config1.status != enable) {
    vision_config1.status = enable;
    err =
        stream_->Set(kRegVisionConfig1, vision_config1.vision_config_reg_value);
    if (err) return err;
  }
  if (enable) {
    malloc_vision_buffer(vision_type);
  } else {
    free_vision_buffer(vision_type);
  }

  return SENTRY_OK;
}

uint8_t Sentry::VisionSetDefault(sentry_vision_e vision_type) {
  sentry_err_t err;
  sentry_vision_conf1_t vision_config1;

  err = stream_->Set(kRegVisionId, vision_type);
  if (err) return err;
  err =
      stream_->Get(kRegVisionConfig1, &vision_config1.vision_config_reg_value);
  if (err) return err;
  vision_config1.default_setting = 1;
  err = stream_->Set(kRegVisionConfig1, vision_config1.vision_config_reg_value);
  if (err) return err;
  while (vision_config1.default_setting) {
    err = stream_->Get(kRegVisionConfig1,
                       &vision_config1.vision_config_reg_value);
    if (err) return err;
  }

  return SENTRY_OK;
}

bool Sentry::VisionGetStatus(sentry_vision_e vision_type) {
  uint8_t vision_status1 = 0;

  stream_->Get(kRegVisionConfig1, &vision_status1);

  return (0x01 << vision_type) & vision_status1;
}

uint8_t Sentry::UpdateResult(sentry_vision_e vision_type) {
  sentry_err_t err;
  uint8_t frame;

  err = stream_->Get(kRegFrameCount, &frame);
  if (err) return SENTRY_FAIL;
  if (kVisionQrCode == vision_type && qrcode_state_) {
    if (frame != qrcode_state_->frame) {
      sentry_qrcode_state_t qrcode_state;
      SensorLockReg(true);
      err = stream_->ReadQrCode(&qrcode_state);
      if (err) return err;
      SensorLockReg(false);
      memcpy(qrcode_state_, &qrcode_state, sizeof(sentry_qrcode_state_t));
    } else {
      /* Result not update */
      return SENTRY_FAIL;
    }
  } else if (vision_state_[vision_type - 1]) {
    if (frame != vision_state_[vision_type - 1]->frame) {
      sentry_vision_state_t vision_state;
      SensorLockReg(true);
      err = stream_->Read(vision_type, &vision_state);
      if (err) return err;
      SensorLockReg(false);
      memcpy(vision_state_[vision_type - 1], &vision_state,
             sizeof(sentry_vision_state_t));
    } else {
      /* Result not update */
      return SENTRY_FAIL;
    }
  } else {
    /* Vision uninitialize */
    return SENTRY_FAIL;
  }

  // return vision_type_output;
  return SENTRY_OK;
}

uint8_t Sentry::read(sentry_vision_e vision_type, sentry_obj_info_e obj_info,
                     uint8_t obj_id) {
  uint8_t vision_pointer = vision_type - 1;

  obj_id = obj_id > SENTRY_MAX_RESULT ? SENTRY_MAX_RESULT : obj_id;
  if (!vision_state_[vision_pointer] || vision_pointer >= kVisionMaxType)
    return 0;
  switch (obj_info) {
    case kStatus:
      return vision_state_[vision_pointer]->detect;
    case kXValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].x_value *
             100 / img_w_;
    case kYValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].y_value *
             100 / img_h_;
    case kWidthValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].width * 100 /
             img_w_;
    case kHeightValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].height * 100 /
             img_h_;
    case kLabel:
      return vision_state_[vision_pointer]->vision_result[obj_id].label;
    case kGValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].color_g_value;
    case kRValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].color_r_value;
    case kBValue:
      return vision_state_[vision_pointer]->vision_result[obj_id].color_b_value;
    default:
      return 0;
  }
}

uint8_t Sentry::readQrCode(sentry_obj_info_e obj_info) {
  switch (obj_info) {
    case kStatus:
      return qrcode_state_->detect;
    case kXValue:
      return qrcode_state_->qrcode_result[0].x_value * 100 / img_w_;
    case kYValue:
      return qrcode_state_->qrcode_result[0].y_value * 100 / img_h_;
    case kWidthValue:
      return qrcode_state_->qrcode_result[0].width * 100 / img_w_;
    case kHeightValue:
      return qrcode_state_->qrcode_result[0].height * 100 / img_h_;
    case kLabel:
    case kGValue:
    case kRValue:
    case kBValue:
      /* not support */
      return 0;
    default:
      return 0;
  }
}

uint8_t Sentry::SensorSetRestart(void) {
  sentry_err_t err;
  err = stream_->Set(kRegRestart, 1);
  return err;
}

uint8_t Sentry::SensorSetDefault(void) {
  sentry_sensor_conf_t sensor_config1;
  sentry_err_t err;
  sensor_config1.sensor_config_reg_value = 0;
  sensor_config1.default_setting = 1;
  err = stream_->Set(kRegSensorConfig1, sensor_config1.sensor_config_reg_value);
  while (sensor_config1.default_setting) {
    err = stream_->Get(kRegSensorConfig1,
                       &sensor_config1.sensor_config_reg_value);
    if (err) return err;
  }
  return err;
}

uint8_t Sentry::SensorLockReg(bool lock) {
  sentry_err_t err;
  err = stream_->Set(kRegLock, lock);
  return err;
}

// LED functions
uint8_t Sentry::LedSetMode(sentry_led_e led, bool manual, bool hold) {
  sentry_led_conf_t led_config;
  sentry_reg_e address;
  sentry_err_t err;
  switch (led) {
    case kLed1:
      address = kRegLed1;
      break;
    case kLed2:
      address = kRegLed2;
      break;
    case kLedAll:
      err = this->LedSetMode(kLed1, manual, hold);
      if (err) return err;
      err = this->LedSetMode(kLed2, manual, hold);
      return err;
    default:
      return SENTRY_UNSUPPORT_PARAM;
  }
  err = stream_->Get(address, &led_config.led_reg_value);
  if (err) return err;
  if (led_config.manual != manual || led_config.hold != hold) {
    led_config.manual = manual;
    led_config.hold = hold;
    err = stream_->Set(address, led_config.led_reg_value);
    if (err) return err;
  }

  return err;
}

uint8_t Sentry::LedSetColor(sentry_led_e led, sentry_led_color_e detected_color,
                            sentry_led_color_e undetected_color,
                            uint8_t level) {
  sentry_led_conf_t led_config;
  sentry_reg_e address;
  sentry_err_t err;
  uint8_t led_level;
  // set LED brightness level
  stream_->Get(kRegLedLevel, &led_level);
  switch (led) {
    case kLed1:
      address = kRegLed1;
      led_level = (led_level & 0xF0) | (level & 0x0F);
      stream_->Set(kRegLedLevel, led_level);
      break;
    case kLed2:
      address = kRegLed2;
      led_level = (led_level & 0x0F) | ((level & 0x0F) << 4);
      stream_->Set(kRegLedLevel, led_level);
      break;
    case kLedAll:
      err = this->LedSetColor(kLed1, detected_color, undetected_color, level);
      if (err) return err;
      err = this->LedSetColor(kLed2, detected_color, undetected_color, level);
      return err;
    default:
      return SENTRY_UNSUPPORT_PARAM;
  }
  // set LED color
  err = stream_->Get(address, &led_config.led_reg_value);
  if (err) return err;
  if (led_config.detected_color != detected_color ||
      led_config.undetected_color != undetected_color) {
    led_config.detected_color = detected_color;
    led_config.undetected_color = undetected_color;
    err = stream_->Set(address, led_config.led_reg_value);
    if (err) return err;
  }

  return err;
}

// Camera functions
uint8_t Sentry::CameraSetZoom(sentry_camera_zoom_e zoom) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.zoom != zoom) {
    camera_config1.zoom = zoom;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t Sentry::CameraSetRotate(bool enable) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.rotate != enable) {
    camera_config1.rotate = enable;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t Sentry::CameraSetFPS(sentry_camera_fps_e fps) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.fps != fps) {
    camera_config1.fps = fps;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t Sentry::CameraSetAwb(sentry_camera_white_balance_e awb) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  /* Waiting for camera white balance calibrating */
  do {
    err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  } while (camera_config1.white_balance >= kWhiteBalanceCalibrating);
  if (awb == kLockWhiteBalance) {
    camera_config1.white_balance = awb;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
    // waiting for lock white balance
    do {
      err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
    } while (camera_config1.white_balance >= kWhiteBalanceCalibrating);
  } else if (camera_config1.white_balance != awb) {
    camera_config1.white_balance = awb;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

sentry_camera_zoom_e Sentry::CameraGetZoom(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.zoom;
}

sentry_camera_white_balance_e Sentry::CameraGetAwb(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.white_balance;
}

bool Sentry::CameraGetRotate(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.rotate;
}

sentry_camera_fps_e Sentry::CameraGetFPS(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.fps;
}

// Uart functions
uint8_t Sentry::UartSetBaudrate(sentry_baudrate_e baud) {
  sentry_err_t err;
  sentry_uart_conf_t uart_config;
  err = stream_->Get(kRegUart, &uart_config.uart_reg_value);
  if (uart_config.baudrate != baud) {
    uart_config.baudrate = baud;
    stream_->Set(kRegUart, uart_config.uart_reg_value);
  }
  return err;
}

bool Sentry::malloc_vision_buffer(sentry_vision_e vision_type) {
  if (vision_type && vision_type < kVisionMaxType) {
    if (kVisionQrCode == vision_type && nullptr == qrcode_state_) {
      qrcode_state_ = new sentry_qrcode_state_t;
      return qrcode_state_;
    } else if (vision_state_[vision_type - 1] == nullptr) {
      vision_state_[vision_type - 1] = new sentry_vision_state_t;
      return vision_state_[vision_type - 1];
    }
  }
  return false;
}

bool Sentry::free_vision_buffer(sentry_vision_e vision_type) {
  if (vision_type && vision_type < kVisionMaxType) {
    if (kVisionQrCode == vision_type && nullptr == qrcode_state_) {
      delete qrcode_state_;
      qrcode_state_ = nullptr;
    } else if (vision_state_[vision_type - 1]) {
      delete vision_state_[vision_type - 1];
      vision_state_[vision_type - 1] = nullptr;
    }
  }
  return true;
}
