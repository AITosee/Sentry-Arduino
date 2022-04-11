#include "protocol_analysis.h"
#include "debug/debug_tool.h"

namespace tosee_sentry {

port_list_t PtotocolAnalysis::port_list_;

PtotocolAnalysis::PtotocolAnalysis(port_addr_t port_addr,
                                   dev_addr_t address)
    : address_(address) {
  registerPort(port_addr, address);
}

PtotocolAnalysis::~PtotocolAnalysis() {
  releasePort();
}

void PtotocolAnalysis::portListShow(void) {
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT > 1
  printf("port_list_:0x%x\n", (size_t)&port_list_);
  printf("  ├─size:%u\n", port_list_.size());
  port_node_t* port_node = port_list_.front();
  while (port_node) {
    printf("  └─port_node:0x%x\n", (size_t)port_node);
    printf("  │ ├─port_addr:0x%x\n", port_node->element_.port_addr);
    printf("  │ └─device_list:0x%x\n",
           (size_t)&port_node->element_.device_list);
    printf("  │   ├─size:%u\n", port_node->element_.device_list.size());
    device_node_t* dev_node = port_node->element_.device_list.front();
    while (dev_node) {
      printf("  │   └─dev_node:0x%x\n", (size_t)dev_node);
      printf("  │   │ ├─port_addr:0x%x\n", dev_node->element_.dev_addr);
      printf("  │   │ └─data_q:0x%x\n", (size_t)&dev_node->element_.data_q);
      printf("  │   │   ├─size:%u\n  │   │   └─",
             dev_node->element_.data_q.size());
      dev_node->element_.data_q.showMessage();
      dev_node = dev_node->next_;
    }
    port_node = port_node->next_;
  }
#endif
}

void PtotocolAnalysis::pushPackage(dev_addr_t dev_addr, const pkg_t& pkg) {
  device_node_t* dev_node = nullptr;
  if (dev_addr == address_) {
    // dev_addr == this device address
    dev_node = &this->device_node_;
  } else {
    // search for the device on the current port
    WPRINTF("received data from other device: 0x%x.\n", dev_addr);
    dev_node = port_node_->element_.device_list.front();
    while (dev_node && dev_node->element_.dev_addr != dev_addr) {
      dev_node = dev_node->next_;
    }
  }
  if (dev_node) {
    // if the device has already registered
    dev_node->element_.data_q.push(pkg);
  }
}

void PtotocolAnalysis::registerPort(port_addr_t port_addr,
                                    dev_addr_t address) {
  port_node_t* port_node = port_list_.front();
  while (port_node && port_node->element_.port_addr != port_addr) {
    port_node = port_node->next_;
  }
  if (port_node) {
    // port all ready registered
    port_node_ = port_node;
    // device node initialize
    device_node_.element_.dev_addr = address;
    port_node_->element_.device_list.push_front(&device_node_);
    IPRINTF("port_node: 0x%x already exist\n", (size_t)port_node);
  } else {
    // register port in port list
    // port node initialize
    port_node_ = new port_node_t;
    port_node_->element_.port_addr = port_addr;
    // device node initialize
    device_node_.element_.dev_addr = address;
    port_node_->element_.device_list.push_front(&device_node_);
    port_list_.push_front(port_node_);
    IPRINTF("register port: 0x%x to port list: 0x%x\n",
            (size_t)port_node_, (size_t)&port_list_);
  }
  IPRINTF("register device: 0x%x\n", address);
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT > 1
  PtotocolAnalysis::portListShow();
#endif
}

void PtotocolAnalysis::releasePort() {
  if (port_node_) {
    port_node_->element_.device_list.remove(&device_node_);
    if (port_node_->element_.device_list.front() == nullptr) {
      port_list_.remove(port_node_);
      delete port_node_;
      IPRINTF("port_node: 0x%x delete\n", (size_t)port_node_);
    }
#if SENTRY_DEBUG_ENABLE && LOG_OUTPUT > 1
    PtotocolAnalysis::portListShow();
#endif
  }
}

}  // namespace tosee_sentry
