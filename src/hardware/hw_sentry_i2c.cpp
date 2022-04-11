#include "hw_sentry_i2c.h"
#include <Arduino.h>
#include "debug/debug_tool.h"
#include "sentry_type.h"

namespace tosee_sentry {

HwSentryI2C::HwSentryI2C(hw_i2c_t* i2c_port, uint32_t address)
    : i2c_port_(i2c_port), sentry_address_(address) {
}

HwSentryI2C::~HwSentryI2C() {}

uint32_t HwSentryI2C::I2CRead(uint8_t reg_address, uint8_t* temp) {
  uint8_t ret = SENTRY_OK;
  i2c_port_->beginTransmission((uint8_t)sentry_address_);
  ret = i2c_port_->write(reg_address);
  //Debug Output
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT
  printf("[R:%02x,", reg_address);
#endif
  if (!ret) return SENTRY_READ_TIMEOUT;
  i2c_port_->endTransmission();
  if (i2c_port_->requestFrom(sentry_address_, 1) != 1)
    return SENTRY_READ_TIMEOUT;

  *temp = i2c_port_->read();
  //Debug Output
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT
  printf("%02x],", *temp);
#endif

  return SENTRY_OK;
}

uint32_t HwSentryI2C::I2CWrite(uint8_t reg_address, uint8_t value) {
  uint8_t ret = SENTRY_OK;
  i2c_port_->beginTransmission((uint8_t)sentry_address_);
  ret = i2c_port_->write(reg_address);
  if (!ret) return SENTRY_READ_TIMEOUT;
  ret = i2c_port_->write(value);
  if (!ret) return SENTRY_READ_TIMEOUT;
  i2c_port_->endTransmission();
  //Debug Output
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT
  printf("[W:%02x,%02x],",reg_address,value);
#endif
  return SENTRY_OK;
}

}  // namespace tosee_sentry
