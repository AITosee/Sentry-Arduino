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

#ifndef HW_CONF_H_
#define HW_CONF_H_

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
/* 1kb SRAM device */
#define SENTRY_MAX_RESULT   1
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
/* 2kb SRAM device */
#define SENTRY_MAX_RESULT   5
#endif

#endif /* HW_CONF_H_ */
