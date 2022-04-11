#ifndef PROTOCOL_ANALYSIS_H_
#define PROTOCOL_ANALYSIS_H_

#include "protocol_types.h"

namespace tosee_sentry {

class PtotocolAnalysis {
 public:
  PtotocolAnalysis(port_addr_t uart,
                   dev_addr_t address);
  virtual ~PtotocolAnalysis();
  PtotocolAnalysis(const PtotocolAnalysis&) = delete;
  PtotocolAnalysis& operator=(const PtotocolAnalysis &) = delete;

  void portListShow(void);

  virtual uint8_t receive() = 0;
  virtual uint8_t transmit(const pkg_t& pkg) = 0;
  size_t size() { return device_node_.element_.data_q.size(); }
  pkg_t& popPackage() { return device_node_.element_.data_q.pop(); }
  pkg_t& peekPackage() { return device_node_.element_.data_q.peek(); }
  void flush() { device_node_.element_.data_q.clear(); }

 private:
  void registerPort(port_addr_t uart,
                    dev_addr_t address);
  void releasePort();

 protected:
  void pushPackage(dev_addr_t dev_addr, const pkg_t& pkg);
  static port_list_t port_list_;
  port_node_t* port_node_ = nullptr;
  device_node_t device_node_;
  dev_addr_t address_;
};

}  // namespace tosee_sentry

#endif /* PROTOCOL_ANALYSIS_H_ */
