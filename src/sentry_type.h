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

#ifndef SENTRY_TYPE_H_
#define SENTRY_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hardware/hw_conf.h"

typedef unsigned char sentry_err_t;

#ifdef BIT
#undef BIT
#endif
#define BIT(x) (0x01<<(x))

#if !defined(SENTRY_MAX_RESULT)
#define SENTRY_MAX_RESULT                 25
#endif

#define SENTRY_DEVICE_ID                  0x04
#define SENTRY_FIRMWARE_VERSION           0xFF
#define visionTypeEnumToMacro(v)      (BIT(v-1))

/* Sentry error type */
#define SENTRY_OK                         0x00
#define SENTRY_FAIL                       0x01
#define SENTRY_WRITE_TIMEOUT              0x02
#define SENTRY_READ_TIMEOUT               0x03
#define SENTRY_CHECK_ERROR                0x04
#define SENTRY_UNSUPPORT_PARAM            0x10
#define SENTRY_UNKNOWN_PROTOCOL           0x11

// typedef enum {
//   kVisionColor            = 1,
//   kVisionBlob             = 2,
//   kVisionAprilTag         = 3,
//   kVisionLine             = 4,
//   kVisionBody             = 5,
//   kVisionCard             = 6,
//   kVisionFace             = 7,
//   kVision20Classes        = 8,
//   kVisionQrCode           = 9,
//   kVisionObjTrack         = 10,
//   kVisionMotionDetect     = 11,
//   kVisionMaxType          ,
// } sentry_vision_e;
typedef enum {
  kLedClose           = 0,
  kLedRed             = 1,
  kLedGreen           = 2,
  kLedYellow          = 3,
  kLedBlue            = 4,
  kLedPurple          = 5,
  kLedCyan            = 6,
  kLedWhite           = 7,
} sentry_led_color_e;
//register address define
typedef enum {
  kRegDeviceId        = 0x01,
  kRegFirmwareVersion = 0x02,
  kRegRestart         = 0x03,
  kRegSensorConfig1   = 0x04,
  kRegLock            = 0x05,
  kRegLedConfig       = 0x06,
  kRegLedLevel        = 0x08,
  kRegUart            = 0x09,
  kRegUSBCongig       = 0x0B,
  kRegHWConfig        = 0x0F,
  kRegCameraConfig1   = 0x10,
  kRegCameraConfig2   = 0x11,
  kRegCameraConfig3   = 0x12,
  kRegCameraConfig4   = 0x13,
  kRegCameraConfig5   = 0x14,
  kRegFrameWidthH     = 0x1B,
  kRegFrameWidthL     = 0x1C,
  kRegFrameHeightH    = 0x1D,
  kRegFrameHeightL    = 0x1E,
  kRegFrameCount      = 0x1F,
  kRegVisionId        = 0x20,
  kRegVisionConfig1   = 0x21,
  kRegVisionConfig2   = 0x22,
  kRegParamNum        = 0x23,
  kRegParamId         = 0x24,
  kRegVisionStatus1   = 0x2A,
  kRegVisionStatus2   = 0x2B,
  kRegVisionDetect1   = 0x30,
  kRegVisionDetect2   = 0x31,
  kRegResultNumber    = 0x34,
  kRegResultId        = 0x35,
  kRegReadStatus1     = 0x36,
  kRegParamValue1H    = 0x70,
  kRegParamValue1L    = 0x71,
  kRegParamValue2H    = 0x72,
  kRegParamValue2L    = 0x73,
  kRegParamValue3H    = 0x74,
  kRegParamValue3L    = 0x75,
  kRegParamValue4H    = 0x76,
  kRegParamValue4L    = 0x77,
  kRegParamValue5H    = 0x78,
  kRegParamValue5L    = 0x79,
  kRegResultData1H    = 0x80,
  kRegResultData1L    = 0x81,
  kRegResultData2H    = 0x82,
  kRegResultData2L    = 0x83,
  kRegResultData3H    = 0x84,
  kRegResultData3L    = 0x85,
  kRegResultData4H    = 0x86,
  kRegResultData4L    = 0x87,
  kRegResultData5H    = 0x88,
  kRegResultData5L    = 0x89,
  kRegSn              = 0xD0,
} sentry_reg_e;

//type define
typedef enum {
  kLed1,
  kLed2,
  kLedAll,
} sentry_led_e;
typedef enum {
  kSerialMode,
  kI2CMode,
  kUnknownMode,
} sentry_mode_e;
typedef enum {
  kBaud9600     = 0x00,
  kBaud19200    = 0x01,
  kBaud38400    = 0x02,
  kBaud57600    = 0x03,
  kBaud115200   = 0x04,
  kBaud921600   = 0x05,
  kBaud1152000  = 0x06,
  kBaud2000000  = 0x07,
} sentry_baudrate_e;
typedef enum {
  kStatus,        //!< whether the target is detected
  kXValue,        //!< target horizontal position
  kYValue,        //!< target vertical position
  kWidthValue,    //!< target width
  kHeightValue,   //!< target height
  kLabel,         //!< target label
  kRValue,        //!< R channel value
  kGValue,        //!< G channel value
  kBValue,        //!< B channel value
} sentry_obj_info_e;
typedef enum {
  kZoomDefault  = 0,
  kZoom1        = 1,
  kZoom2        = 2,
  kZoom3        = 3,
  kZoom4        = 4,
  kZoom5        = 5,
} sentry_camera_zoom_e;
typedef enum {
  kFPSNormal        = 0,          //!< 25FPS mode
  kFPSHigh          = 1,          //!< 50FPS mode
} sentry_camera_fps_e;
typedef enum {
  kAutoWhiteBalance       = 0,    //!< auto white balance mode
  kLockWhiteBalance       = 1,    //!< lock white balance with current value, the entire process takes about 100ms
  kWhiteLight             = 2,    //!< white light mode
  kYellowLight            = 3,    //!< yellow light mode
  kWhiteBalanceCalibrating,
} sentry_camera_white_balance_e;
/* register type */
typedef union {
  struct {
    unsigned char start_up :1;
    unsigned char auto_save :1;
    unsigned char default_setting :1;  //!< set 1 reset all config
    unsigned char disable_vison :1;
  };
  unsigned char sensor_config_reg_value;
} sentry_sensor_conf_t;
typedef union {
  struct {
    sentry_baudrate_e baudrate :3;
  };
  unsigned char uart_reg_value;
} sentry_uart_conf_t;
typedef union {
  struct {
    unsigned char manual:1;
    sentry_led_color_e detected_color:3;
    unsigned char reserved:1;
    sentry_led_color_e undetected_color:3;
  };
  unsigned char led_reg_value;
} sentry_led_conf_t;
typedef union {
  struct {
    sentry_camera_zoom_e zoom:3;
    unsigned char rotate:1;
    sentry_camera_fps_e fps:1;
    sentry_camera_white_balance_e white_balance:3;
  };
  unsigned char camera_reg_value;
} sentry_camera_conf1_t;
typedef union {
  struct {
    unsigned char status :1;
    unsigned char default_setting :1;  //!< set 1 to reset vision configuration
  };
  unsigned char vision_config_reg_value;
} sentry_vision_conf1_t;

typedef struct {
  union {
    uint16_t result_data1;
    uint16_t x_value;
    uint16_t color_r_value;
    uint16_t top_x_value;
  };
  union {
    uint16_t result_data2;
    uint16_t y_value;
    uint16_t color_g_value;
    uint16_t top_y_value;
  };
  union {
    uint16_t result_data3;
    uint16_t width;
    uint16_t color_b_value;
    uint16_t bot_x_value;
  };
  union {
    uint16_t result_data4;
    uint16_t height;
    uint16_t bot_y_value;
  };
  union {
    uint16_t result_data5;
    uint16_t color;
    uint16_t label;
  };
} sentry_object_t;

typedef struct {
  uint16_t x_value;
  uint16_t y_value;
  uint16_t width;
  uint16_t height;
  uint16_t length;
  char str[34 + 1];
} sentry_qrcode_t;

typedef struct {
  unsigned char frame;
  unsigned char detect;
  sentry_object_t vision_result[SENTRY_MAX_RESULT];
} sentry_vision_state_t;

typedef struct {
  unsigned char frame;
  unsigned char detect;
  sentry_qrcode_t qrcode_result[1];
} sentry_qrcode_state_t;

#ifdef __cplusplus
}
#endif

#endif /* SENTRY_TYPE_H_ */
