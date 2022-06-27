#include "sentry_uart.h"

#include "debug/debug_tool.h"

namespace tosee_sentry {

SentryUart::SentryUart(HwSentryUart::hw_uart_t hw_port, uint32_t address)
    : SentryStreamBase(),
      SentryPtotocolAnalysis<HwSentryUart, HwSentryUart::hw_uart_t>(hw_port,
                                                                    address) {
  sentry_address_ = address;
}

SentryUart::~SentryUart() {}

sentry_err_t SentryUart::Get(const uint8_t reg_address, uint8_t* value) {
  pkg_t pkg = {2, {SENTRY_PROTOC_COMMADN_GET, reg_address}};
  sentry_err_t err;
  err = transmit(pkg);
  if (err) {
    return err;
  }
  uint8_t try_time = 0;
  for (;;) {
    err = receive();
    if (err) {
      return err;
    }
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (ret_val.buf[0] == SENTRY_PROTOC_OK &&
          ret_val.buf[1] == SENTRY_PROTOC_COMMADN_GET) {
        *value = ret_val.buf[2];
        return SENTRY_OK;
      } else {
        return ret_val.buf[0];
      }
    }
    try_time++;
    if (try_time > 3) {
      return SENTRY_READ_TIMEOUT;
    }
  }
  return SENTRY_OK;
}

sentry_err_t SentryUart::Set(const uint8_t reg_address, const uint8_t value) {
  pkg_t pkg = {3, {SENTRY_PROTOC_COMMADN_SET, reg_address, value}};
  sentry_err_t err;
  err = transmit(pkg);
  if (err) return err;
  uint8_t try_time = 0;
  for (;;) {
    try_time++;
    if (try_time > 3) {
      return SENTRY_READ_TIMEOUT;
    }
    err = receive();
    if (err) {
      return err;
    }
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (ret_val.buf[1] != SENTRY_PROTOC_COMMADN_SET ||
          ret_val.buf[2] != reg_address) {
        /* Not the return of CMD SET! */
        continue;
      }
      if (SENTRY_PROTOC_OK == ret_val.buf[0])
      {
        return SENTRY_OK;
      }
      return ret_val.buf[0];
    }
  }
  return SENTRY_OK;
}

sentry_err_t SentryUart::SetParam(int vision_type,
                                  sentry_object_t* param, int param_id) {
  sentry_err_t err = SENTRY_OK;
  int try_time = 0;

  pkg_t pkg;
  pkg.len = 14;
  pkg.buf[0] = SENTRY_PROTOC_SET_PARAM;
  pkg.buf[1] = vision_type;
  pkg.buf[2] = (uint8_t)param_id;
  pkg.buf[3] = (uint8_t)param_id;
  pkg.buf[4] = (param->result_data1 >> 8) & 0xFF;
  pkg.buf[5] = param->result_data1 & 0xFF;
  pkg.buf[6] = (param->result_data2 >> 8) & 0xFF;
  pkg.buf[7] = param->result_data2 & 0xFF;
  pkg.buf[8] = (param->result_data3 >> 8) & 0xFF;
  pkg.buf[9] = param->result_data3 & 0xFF;
  pkg.buf[10] = (param->result_data4 >> 8) & 0xFF;
  pkg.buf[11] = param->result_data4 & 0xFF;
  pkg.buf[12] = (param->result_data5 >> 8) & 0xFF;
  pkg.buf[13] = param->result_data5 & 0xFF;

  err = transmit(pkg);
  if (err) return err;
  for (;;) {
    err = receive();
    if (err) return err;
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (ret_val.buf[0] == SENTRY_PROTOC_OK) {
        if (ret_val.buf[1] == SENTRY_PROTOC_SET_PARAM) {
          return SENTRY_OK;
        } else {
          return SENTRY_UNSUPPORT_PARAM;
        }
      }
      return ret_val.buf[0];
    }
    try_time++;
    if (try_time > 3) {
      return SENTRY_READ_TIMEOUT;
    }
  }

  return err;
}

sentry_err_t SentryUart::Read(int vision_type,
                              sentry_vision_state_t* vision_state) {
  int try_time = 0;
  sentry_err_t err = SENTRY_OK;
  pkg_t pkg = {4, {SENTRY_PROTOC_GET_RESULT, (uint8_t)vision_type, 1, SENTRY_MAX_RESULT}};
  vision_state->detect = 0;
  err = transmit(pkg);
  if (err) return err;
  for (;;) {
    err = receive();
    if (err) return err;
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (ret_val.buf[0] == SENTRY_PROTOC_OK ||
          ret_val.buf[0] == SENTRY_PROTOC_RESULT_NOT_END ||
          ret_val.buf[3] == vision_type) {
        if (ret_val.buf[1] == SENTRY_PROTOC_GET_RESULT) {
          vision_state->frame = ret_val.buf[2];
          uint8_t start_id = ret_val.buf[4];
          uint8_t stop_id = ret_val.buf[5];
          uint8_t* presult = &ret_val.buf[6];
          if (stop_id == 0) return SENTRY_OK;
          for (uint8_t i = start_id - 1, j = 0; i < stop_id; i++, j++) {
            vision_state->vision_result[i].x_value =
                presult[10 * j + 0] << 8 | presult[10 * j + 1];
            vision_state->vision_result[i].y_value =
                presult[10 * j + 2] << 8 | presult[10 * j + 3];
            vision_state->vision_result[i].width =
                presult[10 * j + 4] << 8 | presult[10 * j + 5];
            vision_state->vision_result[i].height =
                presult[10 * j + 6] << 8 | presult[10 * j + 7];
            vision_state->vision_result[i].label =
                presult[10 * j + 8] << 8 | presult[10 * j + 9];
            vision_state->detect++;
          }
          if (ret_val.buf[0] == SENTRY_PROTOC_OK) {
            return SENTRY_OK;
          } else {
            continue;
          }
        } else {
          EPRINTF("Error command: 0x%x\n", ret_val.buf[1]);
          return SENTRY_UNSUPPORT_PARAM;
        }
      } else {
        return SENTRY_UNSUPPORT_PARAM;
      }
    }
    try_time++;
    if (try_time > 3) {
      return SENTRY_READ_TIMEOUT;
    }
  }
  return SENTRY_OK;
}

sentry_err_t SentryUart::ReadQrCode(int vision_type,
                                    sentry_qrcode_state_t* qrcode) {
  int try_time = 0;
  sentry_err_t err = SENTRY_OK;
  pkg_t pkg = {4, {SENTRY_PROTOC_GET_RESULT, (uint8_t)vision_type, 0, 0}};
  qrcode->detect = 0;
  err = transmit(pkg);
  if (err) return err;
  for (;;) {
    err = receive();
    if (err) return err;
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (ret_val.buf[0] == SENTRY_PROTOC_OK ||
          ret_val.buf[3] == vision_type) {
        if (ret_val.buf[1] == SENTRY_PROTOC_GET_RESULT) {
          qrcode->frame = ret_val.buf[2];
          qrcode->detect = 0;
          if (ret_val.buf[5] == 0) return SENTRY_OK;
          qrcode->detect = (ret_val.buf[5] - ret_val.buf[4] + 1) > 0;
          if (!qrcode->detect) return SENTRY_OK;
          uint8_t* presult = &ret_val.buf[6];
          qrcode->qrcode_result[0].x_value = presult[0] << 8 | presult[1];
          qrcode->qrcode_result[0].y_value = presult[2] << 8 | presult[3];
          qrcode->qrcode_result[0].width = presult[4] << 8 | presult[5];
          qrcode->qrcode_result[0].height = presult[6] << 8 | presult[7];
          qrcode->qrcode_result[0].length = presult[8] << 8 | presult[9];
          for (uint16_t i = 0; i < qrcode->qrcode_result[0].length; ++i) {
            qrcode->qrcode_result[0].str[i] = presult[11 + 2 * i];
          }
          qrcode->qrcode_result[0].str[qrcode->qrcode_result[0].length] = 0;
          return SENTRY_OK;
        } else {
          EPRINTF("Error command: 0x%x\n", ret_val.buf[1]);
          return SENTRY_UNSUPPORT_PARAM;
        }
      } else {
        return SENTRY_UNSUPPORT_PARAM;
      }
    }
    try_time++;
    if (try_time > 3) {
      return SENTRY_READ_TIMEOUT;
    }
  }
  return SENTRY_OK;
}

sentry_err_t SentryUart::Write(int vision_type,
                               const sentry_vision_state_t* vision_state) {
  sentry_err_t err = SENTRY_OK;
  pkg_t pkg;

  for (size_t i = 0; i < vision_state->detect; i++) {
    pkg.buf[0] = SENTRY_PROTOC_SET_RESULT;
    pkg.buf[1] = vision_type;
    pkg.buf[2] = i + 1;
    pkg.buf[3] = i + 1;
    pkg.buf[4] = (vision_state->vision_result[i].result_data1 >> 8) & 0xFF;
    pkg.buf[5] = vision_state->vision_result[i].result_data1 & 0xFF;
    pkg.buf[6] = (vision_state->vision_result[i].result_data2 >> 8) & 0xFF;
    pkg.buf[7] = vision_state->vision_result[i].result_data2 & 0xFF;
    pkg.buf[8] = (vision_state->vision_result[i].result_data3 >> 8) & 0xFF;
    pkg.buf[9] = vision_state->vision_result[i].result_data3 & 0xFF;
    pkg.buf[10] = (vision_state->vision_result[i].result_data4 >> 8) & 0xFF;
    pkg.buf[11] = vision_state->vision_result[i].result_data4 & 0xFF;
    pkg.buf[12] = (vision_state->vision_result[i].result_data5 >> 8) & 0xFF;
    pkg.buf[13] = vision_state->vision_result[i].result_data5 & 0xFF;
    pkg.len = 14;
    err = transmit(pkg);
    if (err) return err;
    err = receive();
    if (err) return err;
    /* TODO: 此处未考虑总线设备接收到其他传感器指令的情况，应读取多次 */
    if (size()) {
      pkg_t& ret_val = popPackage();
      if (3 == ret_val.len && SENTRY_PROTOC_OK == ret_val.buf[0] &&
          SENTRY_PROTOC_SET_RESULT == ret_val.buf[1] &&
          vision_type == int(ret_val.buf[2])) {
        return err;
      } else {
        return SENTRY_FAIL;
      }
    }
  }

  return err;
}

}  // namespace tosee_sentry
