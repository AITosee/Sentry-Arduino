#ifndef PROTOCOL_TYPES_H_
#define PROTOCOL_TYPES_H_

#include "SimpleList.h"
#include <stdint.h>
#include "../hardware/hw_conf.h"

namespace tosee_sentry {

#ifdef SENTRY_MAX_RESULT
  /* QRCode = (cmd + frame + version_id + start_id + stop_id + (x,y,w,h,num_code) * sizeof(uint16) + ALIGN_UP(num_code * sizeof(uint16), 10)) + protocol_size */
  #define SENTRY_QRCODE_BUFFER_MAX_SIZE ((5 + 10 + 34 * 2 + 2) + 6)
  /* Object = (cmd + frame + version_id + start_id + stop_id + max_result * ((x,y,w,h,l) * sizeof(uint16))) + protocol_size */
  #define SENTRY_OBJECT_BUFFER_MAX_SIZE ((5 + SENTRY_MAX_RESULT * 10) + 6)
  #if SENTRY_QRCODE_BUFFER_MAX_SIZE > SENTRY_OBJECT_BUFFER_MAX_SIZE
    #define PROTOCOL_SINGLE_BUFFER_SIZE SENTRY_QRCODE_BUFFER_MAX_SIZE
  #else
    #define PROTOCOL_SINGLE_BUFFER_SIZE SENTRY_OBJECT_BUFFER_MAX_SIZE
  #endif /* SENTRY_OBJECT_BUFFER_MAX_SIZE ((5 + SENTRY_MAX_RESULT * 10) + 6) */
#else
  #define PROTOCOL_SINGLE_BUFFER_SIZE 256
#endif /* SENTRY_MAX_RESULT */
#if PROTOCOL_SINGLE_BUFFER_SIZE > 256
  #undef PROTOCOL_SINGLE_BUFFER_SIZE
  #define PROTOCOL_SINGLE_BUFFER_SIZE 256
#endif /* PROTOCOL_SINGLE_BUFFER_SIZE > 256 */
#define PROTOCOL_BUFFER_DEEP 1

/* Protocol Error Type */
#define SENTRY_PROTOC_OK                    0xE0
#define SENTRY_PROTOC_FAIL                  0xE1
#define SENTRY_PROTOC_UNKNOWN               0xE2
#define SENTRY_PROTOC_TIMEOUT               0xE3
#define SENTRY_PROTOC_CHECK_ERROR           0xE4
#define SENTRY_PROTOC_LENGTH_ERROR          0xE5
#define SENTRY_PROTOC_UNSUPPORT_COMMAND     0xE6
#define SENTRY_PROTOC_UNSUPPORT_REG_ADDRESS 0xE7
#define SENTRY_PROTOC_UNSUPPORT_REG_VALUE   0xE8
#define SENTRY_PROTOC_READ_ONLY             0xE9
#define SENTRY_PROTOC_RESTART_ERROR         0xEA
#define SENTRY_PROTOC_RESULT_NOT_END        0xEC

typedef size_t port_addr_t;
typedef uint8_t dev_addr_t;

struct pkg_t {
  size_t len;
  uint8_t buf[PROTOCOL_SINGLE_BUFFER_SIZE];
};

typedef SimpleQueue<pkg_t, PROTOCOL_BUFFER_DEEP> data_queue_t;
struct device_t {
  dev_addr_t dev_addr;
  data_queue_t data_q;
};

typedef _SimpleNode<device_t> device_node_t;
typedef _SimpleList<device_node_t> device_list_t;
struct port_t {
  port_addr_t port_addr;
  device_list_t device_list;
};
typedef _SimpleNode<port_t> port_node_t;
typedef _SimpleList<port_node_t> port_list_t;

}  // namespace tosee_sentry

#endif /* PROTOCOL_TYPES_H_ */
