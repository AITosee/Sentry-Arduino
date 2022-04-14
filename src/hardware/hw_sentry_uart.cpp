#include <stdio.h>

#include "debug/debug_tool.h"
#include "hw_sentry_uart.h"

namespace tosee_sentry {

HwSentryUart::HwSentryUart(hw_uart_t hw_port)
    : hw_port_(hw_port) {
}

HwSentryUart::~HwSentryUart() {
}

size_t HwSentryUart::available(void) {
  return hw_port_->available();
}

size_t HwSentryUart::read(uint8_t* buf, size_t length) {
  size_t ret = hw_port_->readBytes(buf, length);
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT
  for (unsigned int i = 0; i < ret; ++i) {
    printf("0x%02x,", buf[i]);
  }
#endif
  return ret;
}

size_t HwSentryUart::write(uint8_t* buf, size_t length) {
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT
  for (unsigned int i = 0; i < length; ++i) {
    printf("%02x,", buf[i]);
  }
#endif
  return hw_port_->write(buf, length);
}

}  // namespace tosee_sentry
