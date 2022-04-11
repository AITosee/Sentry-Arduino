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

#ifndef HW_SENTRY_I2C_H_
#define HW_SENTRY_I2C_H_

#include <Wire.h>

namespace tosee_sentry {

// if u want to use software wire in arduino, change include and hw_i2c_t type here
//#include <SoftwareWire.h>
//@type define I2C type
//typedef SoftwareWire hw_i2c_t;


// @Must public inheritance class SentryI2C
class HwSentryI2C {
 public:
  //@type define I2C type
  typedef TwoWire hw_i2c_t;
  HwSentryI2C(hw_i2c_t* i2c_port, uint32_t address);
  virtual ~HwSentryI2C();
  HwSentryI2C(const HwSentryI2C&) = delete;
  HwSentryI2C& operator=(const HwSentryI2C &) = delete;

/**
  * @brief  I2C read byte.
  * @param  reg_address: register address.
  * @param  temp: register value.
  * @retval 0: read success
  *         not 0: error
  */
  virtual uint32_t I2CRead(uint8_t reg_address, uint8_t* temp);
/**
  * @brief  I2C write byte.
  * @param  reg_address: register address.
  * @param  value: the value write to register.
  * @retval 0: write success
  *         not 0: error
  */
  virtual uint32_t I2CWrite(uint8_t reg_address, uint8_t value);

 private:
  hw_i2c_t* i2c_port_ = nullptr;
  int sentry_address_;

 protected:
};

}  // namespace tosee_sentry

#endif /* HW_SENTRY_I2C_H_ */
