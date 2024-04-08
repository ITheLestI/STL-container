#pragma once

template<typename T>
class Node {
 public:
  T value_;

  Node<T>* parent_ = nullptr;
  Node<T>* left_ = nullptr;
  Node<T>* right_ = nullptr;
  bool is_empty_ = true;
  Node() = default;
  Node(const T& value) : value_(value), is_empty_(false) {}
  Node(const Node<T>& other) = delete;
  Node<T>& operator=(const Node<T>& other) = delete;
  ~Node() {
    delete left_;
    delete right_;
  }
};
