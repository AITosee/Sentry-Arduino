#include "sentry_i2c.h"

namespace tosee_sentry {

SentryI2C::SentryI2C(HwSentryI2C::hw_i2c_t* i2c_port, uint32_t address)
    : SentryStreamBase(), HwSentryI2C(i2c_port, address) {
  SentryStreamBase::sentry_address_ = address;
}

SentryI2C::~SentryI2C() {}

sentry_err_t SentryI2C::Get(const uint8_t reg_address, uint8_t* value) {
  return I2CRead(reg_address, value);
}

sentry_err_t SentryI2C::Set(const uint8_t reg_address, const uint8_t value) {
  return I2CWrite(reg_address, value);
}

sentry_err_t SentryI2C::Read(int vision_type,
                             sentry_vision_state_t* vision_state) {
  sentry_err_t err;
  uint8_t result_data_tmp[2];
  err = I2CWrite(kRegVisionId, vision_type);
  if (err) return err;
  err = I2CRead(kRegFrameCount, &vision_state->frame);
  if (err) return err;
  err = I2CRead(kRegResultNumber, &vision_state->detect);
  if (err) return err;
  if (!vision_state->detect) return SENTRY_OK;
  vision_state->detect = SENTRY_MAX_RESULT < vision_state->detect
                             ? SENTRY_MAX_RESULT
                             : vision_state->detect;
  for (uint8_t i = 0; i < vision_state->detect; ++i) {
    err = I2CWrite(kRegResultId, i + 1);
    if (err) return err;
    I2CRead(kRegResultData1L, result_data_tmp);
    I2CRead(kRegResultData1H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data1 =
        result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData2L, result_data_tmp);
    I2CRead(kRegResultData2H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data2 =
        result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData3L, result_data_tmp);
    I2CRead(kRegResultData3H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data3 =
        result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData4L, result_data_tmp);
    I2CRead(kRegResultData4H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data4 =
        result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData5L, result_data_tmp);
    I2CRead(kRegResultData5H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data5 =
        result_data_tmp[1] << 8 | result_data_tmp[0];
  }
  return SENTRY_OK;
}

sentry_err_t SentryI2C::SetParam(int vision_type,
                                 sentry_object_t* param, int param_id) {
  sentry_err_t err;
  uint8_t result_data_tmp[2];

  err = I2CWrite(kRegVisionId, vision_type);
  if (err) return err;
  err = I2CWrite(kRegParamId, param_id);
  if (err) return err;
  I2CWrite(kRegParamValue1H, (param->result_data1 >> 8) & 0xFF);
  I2CWrite(kRegParamValue1L, param->result_data1 & 0xFF);
  I2CWrite(kRegParamValue2H, (param->result_data2 >> 8) & 0xFF);
  I2CWrite(kRegParamValue2L, param->result_data2 & 0xFF);
  I2CWrite(kRegParamValue3H, (param->result_data3 >> 8) & 0xFF);
  I2CWrite(kRegParamValue3L, param->result_data3 & 0xFF);
  I2CWrite(kRegParamValue4H, (param->result_data4 >> 8) & 0xFF);
  I2CWrite(kRegParamValue4L, param->result_data4 & 0xFF);
  I2CWrite(kRegParamValue5H, (param->result_data5 >> 8) & 0xFF);
  I2CWrite(kRegParamValue5L, param->result_data5 & 0xFF);

  return SENTRY_OK;
}

sentry_err_t SentryI2C::ReadQrCode(int vision_type,
                                   sentry_qrcode_state_t* qrcode) {
  sentry_err_t err;
  uint8_t result_data_tmp[2];

  err = I2CWrite(kRegVisionId, vision_type);
  if (err) return err;
  err = I2CRead(kRegFrameCount, &qrcode->frame);
  if (err) return err;
  err = I2CRead(kRegResultNumber, &qrcode->detect);
  if (err) return err;
  if (!qrcode->detect) return SENTRY_OK;
  err = I2CWrite(kRegResultId, 1);
  if (err) return err;
  I2CRead(kRegResultData1L, result_data_tmp);
  I2CRead(kRegResultData1H, result_data_tmp + 1);
  qrcode->qrcode_result[0].x_value =
      result_data_tmp[1] << 8 | result_data_tmp[0];
  I2CRead(kRegResultData2L, result_data_tmp);
  I2CRead(kRegResultData2H, result_data_tmp + 1);
  qrcode->qrcode_result[0].y_value =
      result_data_tmp[1] << 8 | result_data_tmp[0];
  I2CRead(kRegResultData3L, result_data_tmp);
  I2CRead(kRegResultData3H, result_data_tmp + 1);
  qrcode->qrcode_result[0].width = result_data_tmp[1] << 8 | result_data_tmp[0];
  I2CRead(kRegResultData4L, result_data_tmp);
  I2CRead(kRegResultData4H, result_data_tmp + 1);
  qrcode->qrcode_result[0].height =
      result_data_tmp[1] << 8 | result_data_tmp[0];
  I2CRead(kRegResultData5L, result_data_tmp);
  I2CRead(kRegResultData5H, result_data_tmp + 1);
  qrcode->qrcode_result[0].length =
      result_data_tmp[1] << 8 | result_data_tmp[0];
  for (uint16_t i = 0; i < qrcode->qrcode_result[0].length; ++i) {
    uint8_t result_id = i / 5 + 2;
    uint8_t offset = i % 5;
    if (0 == i % 5) {
      err = I2CWrite(kRegResultId, result_id);
    }
    if (err) return err;
    I2CRead(kRegResultData1L + 2 * offset, (uint8_t*)(qrcode->qrcode_result[0].str + i));
  }
  qrcode->qrcode_result[0].str[qrcode->qrcode_result[0].length] = 0;

  return SENTRY_OK;
}

sentry_err_t SentryI2C::Write(int vision_type,
                              const sentry_vision_state_t* vision_state) {
  sentry_err_t err = SENTRY_OK;

  if (vision_state->detect >= SENTRY_MAX_RESULT) {
    return SENTRY_FAIL;
  }

  err = I2CWrite(kRegVisionId, vision_type);
  if (err) return err;
  err = I2CWrite(kRegResultNumber, vision_state->detect);
  if (err) return err;
  for (uint8_t i = 0; i < vision_state->detect; ++i) {
    I2CWrite(kRegResultId, i + 1);
    I2CWrite(kRegResultData1L, vision_state->vision_result[i].result_data1 & 0xFF);
    I2CWrite(kRegResultData1H, (vision_state->vision_result[i].result_data1 >> 8) & 0xFF);
    I2CWrite(kRegResultData2L, vision_state->vision_result[i].result_data2 & 0xFF);
    I2CWrite(kRegResultData2H, (vision_state->vision_result[i].result_data2 >> 8) & 0xFF);
    I2CWrite(kRegResultData3L, vision_state->vision_result[i].result_data3 & 0xFF);
    I2CWrite(kRegResultData3H, (vision_state->vision_result[i].result_data3 >> 8) & 0xFF);
    I2CWrite(kRegResultData4L, vision_state->vision_result[i].result_data4 & 0xFF);
    I2CWrite(kRegResultData4H, (vision_state->vision_result[i].result_data4 >> 8) & 0xFF);
    I2CWrite(kRegResultData5L, vision_state->vision_result[i].result_data5 & 0xFF);
    I2CWrite(kRegResultData5H, (vision_state->vision_result[i].result_data5 >> 8) & 0xFF);
  }

  return err;
}

}  // namespace tosee_sentry
