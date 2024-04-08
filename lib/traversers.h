#pragma once

#include <iostream>

#include "node.h"

template<typename T>
struct TraverserBase {
  virtual Node<T>* next(Node<T>* node) = 0;
  virtual Node<T>* prev(Node<T>* node) = 0;
};

template<typename T> 
struct PreorderTraverser : public TraverserBase<T> {
  Node<T>* next(Node<T>* node) override final {
    // std::cout << "Preorder next\n";
    if (node == nullptr) return nullptr;
    if (node->left_ != nullptr) return node->left_;
    if (node->right_ != nullptr) return node->right_;
    if (node->parent_ != nullptr) {
      if (node->parent_->left_ == node) {
        if (node->parent_->right_ != nullptr) return node->parent_->right_;
      }
    }
    while (node->parent_ != nullptr) {
      Node<T>* old_node = node;
      node = node->parent_;
      if (node->left_ == old_node && node->right_ != nullptr) return node->right_;
    }
    return nullptr;
  }
  Node<T>* prev(Node<T>* node) override final {
    // std::cout << "Preorder prev\n";
    if (node == nullptr) return nullptr;
    if (node->parent_ == nullptr) return nullptr;
    
    if (node->parent_->left_ != nullptr && node->parent_->right_ == node) {
      node = node->parent_->left_;
      while (node->right_ != nullptr) {
        node = node->right_;
      }
      return node;
    };
    return node->parent_;
    
  }
};

template<typename T> 
struct InorderTraverser : public TraverserBase<T> {
  Node<T>* next(Node<T>* node) override final {
    // std::cout << "Inorder next\n";
    if (node == nullptr) return nullptr;
    if (node->right_ != nullptr) {
      node = node->right_;
      while (node->left_ != nullptr) {
        node = node->left_;
      }
      return node;
    }
    while (node->parent_ != nullptr) {
      // Node<T>* parent = node->parent_;
      if (node->parent_->left_ == node) return node->parent_;
      node = node->parent_;
    }
    return nullptr;
  }
  Node<T>* prev(Node<T>* node) override final {
    // std::cout << "Inorder prev\n";
    if (node == nullptr) return nullptr;
    if (node->left_ != nullptr) {
      node = node->left_;
      while (node->right_ != nullptr) {
        node = node->right_;
      }
      return node;
    }
    while (node->parent_ != nullptr) {
      Node<T>* old_node = node->parent_;
      node = node->parent_;
      if (node->right_ == old_node) return node;
    }
    return nullptr;
  }
};

template<typename T> 
struct PostorderTraverser : public TraverserBase<T> {
  Node<T>* next(Node<T>* node) override final {
    // std::cout << "Postorder next\n";
    if (node == nullptr) return nullptr;
    if (node->parent_ == nullptr) return nullptr;

    if (node->parent_->right_ == node) {
      return node->parent_;
    }
    if (node->parent_->right_ == nullptr) {
      return node->parent_;
    }
    node = node->parent_->right_;
    while (node->left_ != nullptr) {
      node = node->left_;
    }
    
    return node;
  }
  Node<T>* prev(Node<T>* node) override final {
    // std::cout << "Postorder prev\n";
    if (node == nullptr) return nullptr;
    if (node->right_ != nullptr) return node->right_;
    if (node->left_ != nullptr) return node->left_;
    if (node->parent_ != nullptr) {
      if (node->parent_->right_ == node && node->parent_->left_ != nullptr) return node->parent_->left_;
      node = node->parent_;
      while (node->parent_ != nullptr) {
        if (node->parent_->right_ == node && node->parent_->left_ != nullptr) return node->parent_->left_;
        node = node->parent_;
      }
    }
    return nullptr;
  }
};
