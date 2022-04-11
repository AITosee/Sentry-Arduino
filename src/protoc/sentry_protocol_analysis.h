#ifndef SENTRY_PROTOCOL_ANALYSIS_H_
#define SENTRY_PROTOCOL_ANALYSIS_H_

#include "protocol_analysis.h"
#include "sentry_type.h"

namespace tosee_sentry {

// Protocol
#define SENTRY_PROTOC_START 0xFF
#define SENTRY_PROTOC_END 0xED
#define SENTRY_PROTOC_COMMADN_SET 0x01
#define SENTRY_PROTOC_COMMADN_GET 0x02
#define SENTRY_PROTOC_MESSAGE 0x11
#define SENTRY_PROTOC_SET_PARAM 0x21
#define SENTRY_PROTOC_GET_RESULT 0x23
#define SENTRY_PROTOC_SET_RESULT 0x25

template <class STREAM, class HW_T>
class SentryPtotocolAnalysis : public PtotocolAnalysis, protected STREAM {
 public:
  SentryPtotocolAnalysis(HW_T uart, dev_addr_t address)
      : PtotocolAnalysis((port_addr_t)uart, address), STREAM(uart) {}
  virtual ~SentryPtotocolAnalysis() {}
  SentryPtotocolAnalysis(const SentryPtotocolAnalysis&) = delete;
  SentryPtotocolAnalysis& operator=(const SentryPtotocolAnalysis&) = delete;

  __attribute__((noinline)) virtual uint8_t receive(void) override;
  __attribute__((noinline)) virtual uint8_t transmit(const pkg_t& pkg) override;

 private:
  __attribute__((noinline)) uint8_t headMatch(uint8_t* buf);
  __attribute__((noinline)) uint8_t bodyMatch(uint8_t* buf);
  __attribute__((noinline)) uint8_t check(uint8_t* buf, size_t size);

 protected:
};

template <class STREAM, class HW_T>
uint8_t SentryPtotocolAnalysis<STREAM, HW_T>::receive(void) {
  pkg_t data_buf;
  uint8_t* buf = data_buf.buf;
  dev_addr_t dev_addr = 0;
  // get protocol head
  uint8_t err = headMatch(buf);
  // get protocol body
  if (!err) {
    err = bodyMatch(buf);
  }
  if (err) {
    return err;
  }
  dev_addr = buf[2];
  data_buf.len = buf[1] - 5;
  memcpy(buf, buf + 3, data_buf.len);
  pushPackage(dev_addr, data_buf);
  return err;
}

template <class STREAM, class HW_T>
uint8_t SentryPtotocolAnalysis<STREAM, HW_T>::transmit(const pkg_t& pkg) {
  if (pkg.len + 5 > PROTOCOL_SINGLE_BUFFER_SIZE) {
    return SENTRY_FAIL;
  }  // buffer too large
  uint8_t buffer[PROTOCOL_SINGLE_BUFFER_SIZE];
  buffer[0] = SENTRY_PROTOC_START;
  buffer[1] = 5 + pkg.len;
  buffer[2] = address_;
  memcpy(buffer + 3, pkg.buf, pkg.len);
  buffer[3 + pkg.len] = check(buffer, buffer[1] - 2);
  buffer[4 + pkg.len] = SENTRY_PROTOC_END;
  STREAM::write(buffer, buffer[1]);
  return SENTRY_OK;
}

template <class STREAM, class HW_T>
uint8_t SentryPtotocolAnalysis<STREAM, HW_T>::headMatch(uint8_t* buf) {
  while (1) {
    size_t len = STREAM::read(buf, 1);
    if (len) {
      if (buf[0] == SENTRY_PROTOC_START) {
        return SENTRY_OK;
      } else {
        continue;
      }
    } else {
      return SENTRY_READ_TIMEOUT;
    }
  }
}

template <class STREAM, class HW_T>
uint8_t SentryPtotocolAnalysis<STREAM, HW_T>::bodyMatch(uint8_t* buf) {
  size_t len = STREAM::read(buf + 1, 1);
  if (len) {
    len = STREAM::read(buf + 2, buf[1] - 2);
    if (len == uint8_t(buf[1] - 2)) {
      if (buf[buf[1] - 1] != SENTRY_PROTOC_END) {
        // not end with 0xED
        return SENTRY_FAIL;
      }
      if (buf[buf[1] - 2] != check(buf, buf[1] - 2)) {
        // check error
        return SENTRY_CHECK_ERROR;
      }
      return SENTRY_OK;
    } else {
      // not enough data or data lost
      return SENTRY_READ_TIMEOUT;
    }
  } else {
    return SENTRY_READ_TIMEOUT;
  }
}
template <class STREAM, class HW_T>
uint8_t SentryPtotocolAnalysis<STREAM, HW_T>::check(uint8_t* buf, size_t size) {
  uint32_t sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += buf[i];
  }
  return (uint8_t)(sum & 0xFF);
}

}  // namespace tosee_sentry

#endif /* SENTRY_PROTOCOL_ANALYSIS_H_ */
