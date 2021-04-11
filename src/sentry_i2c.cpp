#include "sentry_i2c.h"

SentryI2C::SentryI2C(uint32_t address)
    : SentryStreamBase() {
  sentry_address_ = address;
}

SentryI2C::~SentryI2C() {}

sentry_err_t SentryI2C::Get(const uint8_t reg_address,
                             uint8_t* value) {
	return I2CRead(reg_address, value);
}

sentry_err_t SentryI2C::Set(const uint8_t reg_address,
                            const uint8_t value) {
  return I2CWrite(reg_address, value);
}

sentry_err_t SentryI2C::Read(sentry_vision_e vision_type,
                             sentry_vision_state_t* vision_state) {
  sentry_err_t err;
  err = I2CWrite(kRegVisionId, vision_type);
  if (err) return err;
  err = I2CRead(kRegResultNumber, &vision_state->detect);
  if (err) return err;
  if (!vision_state->detect) return SENTRY_OK;
  err = I2CRead(kRegFrameCount, &vision_state->frame);
  if (err) return err;
  vision_state->detect = SENTRY_MAX_RESULT < vision_state->detect
                             ? SENTRY_MAX_RESULT
                             : vision_state->detect;
  for (uint8_t i = 0; i < vision_state->detect; ++i) {
    uint8_t result_data_tmp[2];
    err = I2CWrite(kRegResultId, i + 1);
    if (err) return err;
    I2CRead(kRegResultData1L, result_data_tmp);
    I2CRead(kRegResultData1H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data1 = result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData2L, result_data_tmp);
    I2CRead(kRegResultData2H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data2 = result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData3L, result_data_tmp);
    I2CRead(kRegResultData3H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data3 = result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData4L, result_data_tmp);
    I2CRead(kRegResultData4H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data4 = result_data_tmp[1] << 8 | result_data_tmp[0];
    I2CRead(kRegResultData5L, result_data_tmp);
    I2CRead(kRegResultData5H, result_data_tmp + 1);
    vision_state->vision_result[i].result_data5 = result_data_tmp[1] << 8 | result_data_tmp[0];
  }
  return SENTRY_OK;
}





