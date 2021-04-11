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
#include "protoc/protocol_types.h"

class SentryI2C : public SentryStreamBase {
 public:
  SentryI2C(uint32_t address);
  virtual ~SentryI2C(void);
  SentryI2C(const SentryI2C&) = delete;
  SentryI2C& operator=(const SentryI2C &) = delete;

  virtual uint32_t I2CRead(uint8_t reg_address, uint8_t* temp) = 0;
  virtual uint32_t I2CWrite(uint8_t reg_address, uint8_t value) = 0;

  virtual sentry_err_t Get(const uint8_t reg_address, uint8_t* value) override;
  virtual sentry_err_t Set(const uint8_t reg_address,
                           const uint8_t value) override;
  virtual sentry_err_t SetParam(sentry_vision_e vision_type,
                                sentry_object_t* vision_result,
                                int result_id) override {}
  virtual sentry_err_t Read(sentry_vision_e vision_type,
                            sentry_vision_state_t* vision_state) override;
  virtual sentry_err_t ReadQrCode(sentry_qrcode_state_t* qrcode) override {}

 private:

 protected:
};

#endif /* SENTRY_I2C_H_ */
