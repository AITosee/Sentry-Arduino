#include "SentryFactory.h"

#include "sentry_type.h"

SentryFactory::SentryFactory(uint32_t address, uint8_t device_id,
                             sentry_vision_state_t **vision_state,
                             const int vision_max_type,
                             const int vision_qrcode_type)
    : address_(address),
      device_id_(device_id),
      vision_max_type_(vision_max_type),
      vision_qrcode_type_(vision_qrcode_type),
      vision_state_(vision_state) {}

SentryFactory::~SentryFactory() {
  if (stream_) {
    stream_ = nullptr;
    delete stream_;
  }
  for (int i = 1; i < vision_max_type_; i++) {
    free_vision_buffer(int(i));
  }
}

uint8_t SentryFactory::SensorStartupCheck() {
  sentry_sensor_conf_t conf1;
  int err_count = 0;
  sentry_err_t err;
  for (;;) {
    if (++err_count > 100) return SENTRY_FAIL;  // set max retry times
    err = stream_->Get(kRegSensorConfig1, &conf1.sensor_config_reg_value);
    if (!err && conf1.start_up) break;
    int32_t delay = 1000000;
    while (delay--)
      ;
  }
  return err;
}

uint8_t SentryFactory::ProtocolVersionCheck() {
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
    if (!err && device_id == device_id_) break;
  }
  return err;
}

uint8_t SentryFactory::GetImageShape() {
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

uint8_t SentryFactory::SensorInit() {
  sentry_err_t err;

  /* Check sensor startup*/
  err = SensorStartupCheck();
  if (err) return err;
  /* Check sentry protocol version */
  err = ProtocolVersionCheck();
  if (err) return err;
  /* Sensor set default if version is correction. */
  err = SensorSetDefault();
  if (err) return err;
  /* Get sensor image shape. */
  err = GetImageShape();
  if (err) return err;

  return SENTRY_OK;
}

uint8_t SentryFactory::begin(HwSentryUart::hw_uart_t communication_port) {
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

uint8_t SentryFactory::begin(HwSentryI2C::hw_i2c_t *communication_port) {
  if (mode_ == kI2CMode) {
    return SENTRY_OK;
  }
  if (stream_) {
    delete stream_;
    stream_ = nullptr;
  }

  mode_ = kI2CMode;
  stream_ = new SentryI2C(communication_port, address_);

  return SensorInit();
}

// Advance interface
uint8_t SentryFactory::VisionBegin(int vision_type) {
  sentry_err_t err;
  uint8_t max_num;

  /* Set Max Result */
  max_num = GetParamNum(vision_type);
  if (0 > max_num) {
    return SENTRY_FAIL;
  } else if (SENTRY_MAX_RESULT < max_num || 0 == max_num) {
    err = SetParamNum(vision_type, SENTRY_MAX_RESULT);
  }
  err = VisionSetStatus(vision_type, true);
  if (err) return err;

  return SENTRY_OK;
}

uint8_t SentryFactory::VisionEnd(int vision_type) {
  return VisionSetStatus(vision_type, false);
}

int SentryFactory::GetValue(int vision_type, sentry_obj_info_e obj_info,
                            int obj_id) {
  if (obj_info == kStatus) {
    if ((vision_qrcode_type_ == vision_type && qrcode_state_ == NULL) ||
        vision_state_[vision_type - 1] == NULL) {
      /* Vison not enable */
      return SENTRY_FAIL;
    }
    while (UpdateResult(vision_type))
      ;
  }

  if (vision_qrcode_type_ == vision_type) {
    return (int)readQrCode(obj_info);
  } else {
    return (int)read(vision_type, obj_info, obj_id);
  }
}

int SentryFactory::GetParamNum(int vision_type) {
  sentry_err_t err;
  uint8_t max_num;

  err = stream_->Set(kRegVisionId, vision_type);
  if (err) return -1;
  err = stream_->Get(kRegParamNum, &max_num);
  if (err) return -1;

  return (int)max_num;
}

uint8_t SentryFactory::SetParamNum(int vision_type, int max_num) {
  sentry_err_t err;

  max_num = max_num > SENTRY_MAX_RESULT ? SENTRY_MAX_RESULT : max_num;
  err = stream_->Set(kRegVisionId, vision_type);
  if (err) return err;

  err = stream_->Set(kRegParamNum, max_num);

  return err;
}

sentry_vision_state_t *SentryFactory::GetVisionState(int vision_type) {
  return vision_state_[vision_type - 1];
}

uint8_t SentryFactory::VisionSetStatus(int vision_type, bool enable) {
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

uint8_t SentryFactory::VisionSetDefault(int vision_type) {
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

bool SentryFactory::VisionGetStatus(int vision_type) {
  uint8_t vision_status1 = 0;

  stream_->Get(kRegVisionConfig1, &vision_status1);

  return (0x01 << vision_type) & vision_status1;
}

uint8_t SentryFactory::UpdateResult(int vision_type) {
  sentry_err_t err;
  uint8_t frame;

  /* Must make sure register is unlock! */
  while(SENTRY_OK != SensorLockReg(false));
  err = stream_->Get(kRegFrameCount, &frame);
  if (err) return SENTRY_FAIL;
  if (vision_qrcode_type_ == vision_type && qrcode_state_) {
    if (frame != qrcode_state_->frame) {
      sentry_qrcode_state_t qrcode_state;
      while(SENTRY_OK != SensorLockReg(true));
      err = stream_->ReadQrCode(vision_qrcode_type_, &qrcode_state);
      while(SENTRY_OK != SensorLockReg(false));
      if (err) return err;
      memcpy(qrcode_state_, &qrcode_state, sizeof(sentry_qrcode_state_t));
    } else {
      /* Result not update */
      return SENTRY_FAIL;
    }
  } else if (vision_state_[vision_type - 1]) {
    if (frame != vision_state_[vision_type - 1]->frame) {
      sentry_vision_state_t vision_state;
      while(SENTRY_OK != SensorLockReg(true));
      err = stream_->Read(vision_type, &vision_state);
      while(SENTRY_OK != SensorLockReg(false));
      if (err) return err;
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

  return SENTRY_OK;
}

uint8_t SentryFactory::read(int vision_type, sentry_obj_info_e obj_info,
                            uint8_t obj_id) {
  uint8_t vision_idx = vision_type - 1;

  obj_id = obj_id > SENTRY_MAX_RESULT ? SENTRY_MAX_RESULT : obj_id;
  if (!vision_state_[vision_idx] || vision_idx >= vision_max_type_)
    return 0;
  switch (obj_info) {
    case kStatus:
      return vision_state_[vision_idx]->detect;
    case kXValue:
      return vision_state_[vision_idx]->vision_result[obj_id].x_value *
             100 / img_w_;
    case kYValue:
      return vision_state_[vision_idx]->vision_result[obj_id].y_value *
             100 / img_h_;
    case kWidthValue:
      return vision_state_[vision_idx]->vision_result[obj_id].width * 100 /
             img_w_;
    case kHeightValue:
      return vision_state_[vision_idx]->vision_result[obj_id].height * 100 /
             img_h_;
    case kLabel:
      return vision_state_[vision_idx]->vision_result[obj_id].label;
    case kGValue:
      return vision_state_[vision_idx]->vision_result[obj_id].color_g_value;
    case kRValue:
      return vision_state_[vision_idx]->vision_result[obj_id].color_r_value;
    case kBValue:
      return vision_state_[vision_idx]->vision_result[obj_id].color_b_value;
    default:
      return 0;
  }
}

uint8_t SentryFactory::readQrCode(sentry_obj_info_e obj_info) {
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

uint8_t SentryFactory::SensorSetRestart(void) {
  sentry_err_t err;
  err = stream_->Set(kRegRestart, 1);
  return err;
}

uint8_t SentryFactory::SensorSetDefault(void) {
  sentry_sensor_conf_t sensor_config1;
  sentry_err_t err;
  err = stream_->Get(kRegSensorConfig1, &sensor_config1.sensor_config_reg_value);
  if (err) return err;
  // XXX: now, set default only disable all visions
  // sensor_config1.default_setting = 1;
  sensor_config1.disable_vison = 1;
  err = stream_->Set(kRegSensorConfig1, sensor_config1.sensor_config_reg_value);
  while (sensor_config1.disable_vison) {
    err = stream_->Get(kRegSensorConfig1,
                       &sensor_config1.sensor_config_reg_value);
    if (err) return err;
  }
  return err;
}

uint8_t SentryFactory::SensorLockReg(bool lock) {
  sentry_err_t err = SENTRY_OK;
  uint8_t status;
  for (;;) {
    stream_->Get(kRegLock, &status);
    if (err) return err;
    if (status == lock) {
      return SENTRY_OK;
    }
    err = stream_->Set(kRegLock, lock);
    if (err) return err;
  }
  return err;
}

// LED functions
uint8_t SentryFactory::LedSetColor(sentry_led_color_e detected_color,
                                   sentry_led_color_e undetected_color,
                                   uint8_t level) {
  sentry_led_conf_t led_config;
  sentry_err_t err;
  uint8_t led_level;
  uint8_t manual;

  /* Set LED brightness level */
  err = stream_->Get(kRegLedLevel, &led_level);
  if (err) return err;
  err = stream_->Set(kRegLedLevel, (led_level & 0xF0) | (level & 0x0F));
  if (err) return err;
  /* Set LED color */
  err = stream_->Get(kRegLedConfig, &led_config.led_reg_value);
  if (err) return err;
  if (detected_color == undetected_color) {
    manual = 1;
  } else {
    manual = 0;
  }
  if (led_config.detected_color != detected_color ||
      led_config.undetected_color != undetected_color ||
      led_config.manual != manual) {
    led_config.detected_color = detected_color;
    led_config.undetected_color = undetected_color;
    led_config.manual = manual;
    err = stream_->Set(kRegLedConfig, led_config.led_reg_value);
    if (err) return err;
  }

  return err;
}

// Camera functions
uint8_t SentryFactory::CameraSetZoom(sentry_camera_zoom_e zoom) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.zoom != zoom) {
    camera_config1.zoom = zoom;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t SentryFactory::CameraSetRotate(bool enable) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.rotate != enable) {
    camera_config1.rotate = enable;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t SentryFactory::CameraSetFPS(sentry_camera_fps_e fps) {
  sentry_camera_conf1_t camera_config1;
  sentry_err_t err;
  err = stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  if (camera_config1.fps != fps) {
    camera_config1.fps = fps;
    err = stream_->Set(kRegCameraConfig1, camera_config1.camera_reg_value);
  }
  return err;
}

uint8_t SentryFactory::CameraSetAwb(sentry_camera_white_balance_e awb) {
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

sentry_camera_zoom_e SentryFactory::CameraGetZoom(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.zoom;
}

sentry_camera_white_balance_e SentryFactory::CameraGetAwb(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.white_balance;
}

bool SentryFactory::CameraGetRotate(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.rotate;
}

sentry_camera_fps_e SentryFactory::CameraGetFPS(void) {
  sentry_camera_conf1_t camera_config1;
  stream_->Get(kRegCameraConfig1, &camera_config1.camera_reg_value);
  return camera_config1.fps;
}

// Uart functions
uint8_t SentryFactory::UartSetBaudrate(sentry_baudrate_e baud) {
  sentry_err_t err;
  sentry_uart_conf_t uart_config;
  err = stream_->Get(kRegUart, &uart_config.uart_reg_value);
  if (uart_config.baudrate != baud) {
    uart_config.baudrate = baud;
    stream_->Set(kRegUart, uart_config.uart_reg_value);
  }
  return err;
}

bool SentryFactory::malloc_vision_buffer(int vision_type) {
  if (vision_type && vision_type < vision_max_type_) {
    if (vision_qrcode_type_ == vision_type && nullptr == qrcode_state_) {
      qrcode_state_ = new sentry_qrcode_state_t;
      return qrcode_state_;
    } else if (vision_state_[vision_type - 1] == nullptr) {
      vision_state_[vision_type - 1] = new sentry_vision_state_t;
      return vision_state_[vision_type - 1];
    }
  }
  return false;
}

bool SentryFactory::free_vision_buffer(int vision_type) {
  if (vision_type && vision_type < vision_max_type_) {
    if (vision_qrcode_type_ == vision_type && nullptr == qrcode_state_) {
      delete qrcode_state_;
      qrcode_state_ = nullptr;
    } else if (vision_state_[vision_type - 1]) {
      delete vision_state_[vision_type - 1];
      vision_state_[vision_type - 1] = nullptr;
    }
  }
  return true;
}
