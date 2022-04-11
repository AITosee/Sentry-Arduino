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

#ifndef SENTRY_I2C_H_
#define SENTRY_I2C_H_

#include "sentry_stream_base.h"
#include "hardware/hw_sentry_i2c.h"

namespace tosee_sentry {

class SentryI2C : public SentryStreamBase, HwSentryI2C {
 public:
  SentryI2C(HwSentryI2C::hw_i2c_t* i2c_port, uint32_t address);
  virtual ~SentryI2C();
  SentryI2C(const SentryI2C&) = delete;
  SentryI2C& operator=(const SentryI2C&) = delete;

  sentry_err_t Get(const uint8_t reg_address, uint8_t* value) override;
  sentry_err_t Set(const uint8_t reg_address, const uint8_t value) override;
  sentry_err_t SetParam(int vision_type, sentry_object_t* param,
                        int param_id) override;
  sentry_err_t Read(int vision_type,
                    sentry_vision_state_t* vision_state) override;
  sentry_err_t ReadQrCode(int vision_type,
                          sentry_qrcode_state_t* qrcode) override;
  sentry_err_t Write(int vision_type,
                     const sentry_vision_state_t* vision_state) override;

 private:
 protected:
};

}  // namespace tosee_sentry

#endif /* SENTRY_I2C_H_ */
