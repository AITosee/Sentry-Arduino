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

#ifndef SENTRY_COMMUNICATION_BASE_H_
#define SENTRY_COMMUNICATION_BASE_H_

#include <stdint.h>
#include "sentry_type.h"

namespace tosee_sentry {

class SentryStreamBase {
 public:
  SentryStreamBase(void) {}
  virtual ~SentryStreamBase(void) {}
  SentryStreamBase(const SentryStreamBase&) = delete;
  SentryStreamBase& operator=(const SentryStreamBase&) = delete;

  virtual sentry_err_t Get(const uint8_t reg_address, uint8_t* value) = 0;
  virtual sentry_err_t Set(const uint8_t reg_address, const uint8_t value) = 0;
  virtual sentry_err_t SetParam(int vision_type,
                                sentry_object_t* param,
                                int param_id) = 0;
  virtual sentry_err_t Read(int vision_type,
                            sentry_vision_state_t* vision_state) = 0;
  virtual sentry_err_t ReadQrCode(int vision_type,
                                  sentry_qrcode_state_t* qrcode) = 0;
  virtual sentry_err_t Write(int vision_type,
                             const sentry_vision_state_t* vision_state) = 0;

 private:
 protected:
  uint32_t sentry_address_ = 0;
};

}

#endif /* SENTRY_COMMUNICATION_BASE_H_ */
