/*
 * @file      SimpleList.h
 * @brief     This is a simple list/queue class for embedded device,
 *            it will not malloc any memory, but u need to create the
 *            node by u self.
 * @author    ysq
 * @version   1.0
 * @copyright Apache License-2.0
 */

#ifndef SIMPLE_LIST_H_
#define SIMPLE_LIST_H_

#include <stdio.h>
#include <string.h>

namespace tosee_sentry {

/*
 * @brief Node for class _SimpleList.
 * @see _SimpleList
 */
template <class element_t>
class _SimpleNode {
 public:
  //!< Default constructor.
  _SimpleNode() : next_(nullptr) {}
  _SimpleNode(element_t& element) : element_(element), next_(nullptr) {}
  _SimpleNode(element_t&& element) : element_(element), next_(nullptr) {}
  ~_SimpleNode() {}
  _SimpleNode& operator=(const _SimpleNode&) = delete;
  _SimpleNode(const _SimpleNode&) = delete;

  element_t element_;            //!< The element in node.
  _SimpleNode<element_t>* next_;  //!< Next node address.
};

/*
 * @brief A simple list class, this class will not malloc any memory,
 *        but u must instantiation the node by yourself.
 * @see _SimpleNode
 */
template <class node_t>
class _SimpleList {
 public:
  //!< Default constructor.
  _SimpleList() {}
  ~_SimpleList() {}
  _SimpleList& operator=(const _SimpleList& list) {
    this->size_ = list.size_;
    this->head_ = list.head_;
    return *this;
  }
  _SimpleList(const _SimpleList& list) {
    this->size_ = list.size_;
    this->head_ = list.head_;
  }

  /*
   * @brief Get the first node pointer in the list.
   * @retval The first node pointer in list, if the list in empty,
   *         return nullptr.
   */
  node_t* front(void) { return head_; }
  /*
   * @brief Get the last node pointer in the list.
   * @retval The last node pointer in list, if the list in empty,
   *         return nullptr.
   */
  __attribute__((noinline)) node_t* back(void) {
    node_t* ret = front();
    if (!ret) {
      // list is empty
      return nullptr;
    }
    while (ret->next_) {
      ret = ret->next_;
    }
    return ret;
  }
  /*
   * @brief Push the node to the front of the list.
   * @param[in] node The node pointer witch need to push.
   * @retval The node pointer witch u push.
   */
  __attribute__((noinline)) node_t* push_front(node_t* node) {
    if (node) {
      if (head_) {
        node_t* temp = head_->next_;
        head_->next_ = node;
        node->next_ = temp;
      } else {
        head_ = node;
      }
      size_++;
    }
    return node;
  }
  /*
   * @brief Pop the first node in the list, once u run pop_front(), the
   *        first node will be remove from the list.
   * @retval The first node pointer in the list.
   */
  __attribute__((noinline)) node_t* pop_front(void) {
    node_t* front = this->front();
    if (front) {
      size_--;
      head_ = front->next_;
    }
    return front;
  }
  /*
   * @brief Push the node to the back of the list.
   * @param[in] node The node pointer witch need to push.
   * @retval The node pointer witch u push.
   */
  __attribute__((noinline)) node_t* push_back(node_t* node) {
    if (node) {
      node_t* back = this->back();
      if (!back) {
        // if the list is empty
        return push_front(node);
      }
      back->next_ = node;
      node->next_ = nullptr;
      size_++;
    }
    return node;
  }
  /*
   * @brief Pop the last node in the list, once u run pop_back(), the
   *        last node will be remove from the list.
   * @retval The last node pointer in the list.
   */
  __attribute__((noinline)) node_t* pop_back(void) {
    node_t* prev = front();
    if (!prev) {
      // the list is empty
      return nullptr;
    }
    node_t* back = prev->next_;
    if (back) {
      while (back->next_) {
        prev = back;
        back = back->next_;
      }
      prev->next_ = nullptr;
      if (back) {
        size_--;
      }
      return back;
    } else {
      clear();
      return prev;
    }
  }
  /*
   * @brief Remove the node from the list.
   * @param[in] node The node u need to remove.
   * @retval The node pointer witch have been removed, return nullptr
   *         if the node is not in the list.
   */
  __attribute__((noinline)) node_t* remove(node_t* node) {
    node_t* prev = front();
    if (!prev) {
      // the list is empty
      return nullptr;
    } else {
      if (prev == node) {
        head_ = node->next_;
        size_--;
        return node;
      }
    }
    node_t* ret = prev->next_;
    while (ret) {
      if (ret == node) {
        prev->next_ = node->next_;
        size_--;
        return node;
      }
      prev = ret;
      ret = ret->next_;
    }
    return nullptr;
  }
  /*
   * @brief Clear the list.
   */
  void clear(void) {
    head_ = nullptr;
    size_ = 0;
  }
  /*
   * @brief Get the size of the list.
   * @retval The size of the list, return 0 if the list is empty.
   */
  size_t size(void) { return size_; }

  /*
   * @brief Get the node from the list if the function isNode() return
   *        true.
   * @param[in] isNode The function pointer.
   * @retval The node pointer witch isNode return true.
   */
  __attribute__((noinline)) node_t* get_node_if(bool (*isNode)(node_t*)) {
    node_t* ret = front();
    while (ret) {
      if (isNode(ret)) {
        return ret;
      }
      ret = ret->next_;
    }
    return ret;
  }

 private:
 protected:
  node_t* head_ = nullptr;  //!< The head node.
  size_t size_ = 0;         //!< List size.
};

template <class element_t, size_t DEEP,
          template <class> class node_t = _SimpleNode>
class SimpleQueue : public _SimpleList<node_t<element_t>> {
 public:
  SimpleQueue() {}
  ~SimpleQueue() {}
  SimpleQueue& operator=(const SimpleQueue&) = delete;
  SimpleQueue(const SimpleQueue&) = delete;

  /*
   * @brief Get the first element in the queue, use the function size()
   *        to make sure the queue is not empty before u use this
   *        function.
   * @retval The first element in the list.
   * @see size()
   */
  element_t& peek(void) {
    return _SimpleList<node_t<element_t>>::front()->element_;
  };
  /*
   * @brief Pop the first element in the queue, once u run pop(), the
   *        first element will be remove from the queue, use the
   *        function size() to make sure the queue is not empty before
   *        u use this function.
   * @retval The first element in the queue.
   * @see size()
   */
  __attribute__((noinline)) element_t& pop(void) {
    node_t<element_t>* node = _SimpleList<node_t<element_t>>::pop_front();
    if (node) {
      first_++;
      if (first_ >= DEEP) {
        first_ = 0;
      }
    }
    return node->element_;
  }
  /*
   * @brief Push the element to the back of the list, if the queue is
   *        full, this function will pop the first element from the
   *        queue.
   * @param[in] elem The element reference witch need to push.
   * @retval The element reference witch u push.
   */
  __attribute__((noinline)) element_t& push(const element_t& elem) {
    if (_SimpleList<node_t<element_t>>::size() >= DEEP) {
      pop();
    }
    memcpy(&node_[last_].element_, &elem, sizeof(element_t));
    _SimpleList<node_t<element_t>>::push_back(&node_[last_]);
    last_++;
    if (last_ >= DEEP) {
      last_ = 0;
    }
    return node_[last_].element_;
  }
  void showMessage() {
    printf("first_ = %u, last_ = %u, size = %u\n", first_, last_,
           _SimpleList<node_t<element_t>>::size());
  }
  void clear(void) {
    _SimpleList<node_t<element_t>>::clear();
    first_ = 0;
    last_ = 0;
  }

 protected:
  node_t<element_t> node_[DEEP];  //!< The node in the queue.
  size_t first_ = 0;              //!< First element offset.
  size_t last_ = 0;               //!< Last element offset.
};

// template<typename element_t>
// _SimpleList<element_t>::_SimpleList() {
//}
//
// template<typename element_t>
// _SimpleList<element_t>::~_SimpleList() {
//}

// int main () {
//    int element = 100;
//    _SimpleNode<int> node1(&element);
//    int element2 = 101;
//    _SimpleNode<int> node2(&element2);
//    int element3 = 102;
//    _SimpleNode<int> node3(&element3);
//    int element4 = 104;
//    _SimpleNode<int> node4(&element4);
//    int element5 = 105;
//    _SimpleNode<int> node5(&element5);
//    _SimpleNode<int>* elem;
//    _SimpleList<_SimpleNode<int>> list;
//    list.push_front(&node1);
//    list.push_front(&node2);
//    list.push_front(&node3);
//    list.push_back(&node1);
//    list.push_back(&node2);
//    list.push_back(&node3);
//    // list.push_back(&node4);
//    // list.push_back(&node5);
//    elem = list.front();
//    while (elem) {
//        cout<<elem->element_<<":"<<*elem->element_<<", ";
//        elem = elem->next_;
//    }
//    cout<<endl;
//    cout<<"hello world";
//    return 0;
//}

}  // namespace tosee_sentry

#endif
