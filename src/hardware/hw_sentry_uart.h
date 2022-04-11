#ifndef HW_SENTRY_UART_H_
#define HW_SENTRY_UART_H_

#include <Stream.h>

namespace tosee_sentry {

class HwSentryUart {
 public:
  typedef Stream* hw_uart_t;
  HwSentryUart(hw_uart_t hw_port);
  virtual ~HwSentryUart();
  HwSentryUart(const HwSentryUart&) = delete;
  HwSentryUart& operator=(const HwSentryUart &) = delete;

  virtual size_t available(void);
  virtual size_t read(uint8_t* buf, size_t length);
  virtual size_t write(uint8_t* buf, size_t length);

 private:
 protected:
  hw_uart_t hw_port_;
};

}  // namespace tosee_sentry

#endif /* HW_SENTRY_UART_H_ */
