#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include "node.h"
#include "traversers.h"

template <typename T, typename Cmp = std::less<T>,
          typename Alloc = std::allocator<T>>
class BSTree {
 public:
  template <typename TraverseType>
  class Iterator;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = Iterator<InorderTraverser<value_type>>;
  using const_iterator = const Iterator<InorderTraverser<value_type>>;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  using key_type = T;
  using key_compare = Cmp;
  using value_compare = key_compare;
  using node_type = Node<value_type>;

  template <typename TraverseType = InorderTraverser<value_type>>
  class Iterator {
    friend class BSTree<T, Cmp, Alloc>;
    static_assert(std::is_base_of_v<TraverserBase<value_type>, TraverseType>,
                  "Invalid traversal type");

   public:
    using value_type = Node<T>;
    using reference = const Node<T>&;
    using pointer = Node<T>*;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    Iterator(const BSTree& tree) : node_(nullptr), traverser_(), tree_(tree){};
    Iterator(pointer node, const BSTree& tree)
        : node_(node), traverser_(), tree_(tree){};
    Iterator(const Iterator& other) : node_(other.node_), tree_(other.tree_){};
    Iterator<TraverseType>& operator=(const Iterator<TraverseType>& other) {
      if (this != &other) {
        node_ = other.node_;
      }
      return *this;
    }
    ~Iterator() = default;

    T operator*() const {
      return node_->value_;
    }
    const T operator->() const {
      return &(node_->value_);
    }

    bool operator==(const Iterator& other) const {
      return node_ == other.node_;
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    Iterator<TraverseType>& operator++() {
      node_ = traverser_.next(node_);
      return *this;
    }

    Iterator<TraverseType>& operator++(int) {
      Iterator<TraverseType>& tmp = *this;
      node_ = traverser_.next(node_);
      return tmp;
    }

    Iterator<TraverseType>& operator--() {
      if (node_ == nullptr) {
        node_ = get_last();
        return *this;
      }
      node_ = traverser_.prev(node_);
      return *this;
    }

    Iterator<TraverseType>& operator--(int) {
      Iterator<TraverseType>& tmp = *this;
      if (node_ == nullptr) {
        node_ = get_last();
        return tmp;
      }
      node_ = traverser_.prev(node_);
      return tmp;
    }

   private:
    pointer node_;
    TraverseType traverser_;
    const BSTree& tree_;
    pointer get_last() {
      if constexpr (std::is_same_v<TraverseType,
                                   PreorderTraverser<T>>) {
        pointer tmp = tree_.root_;
        while (tmp->left_ != nullptr && tmp->right_ != nullptr) {
          while (tmp->left_ != nullptr) {
            tmp = tmp->left_;
          }
          if (tmp->right_ != nullptr) {
            tmp = tmp->right_;
          }
        }
        return tmp;
      }
      if constexpr (std::is_same_v<TraverseType,
                                   InorderTraverser<T>>) {
        if (tree_.root_ == nullptr) return nullptr;
        Node<T>* node = tree_.root_;
        while (node->right_ != nullptr) {
          node = node->right_;
        }
        return node;
      }
      if constexpr (std::is_same_v<TraverseType,
                                   PostorderTraverser<T>>) {
        return tree_.root_;
      }
      return nullptr;
    }
  };

  BSTree() = default;
  template <typename InputIt>
  BSTree(InputIt first, InputIt last, Cmp cmp) {
    comparator_ = cmp;
    while (first != last) {
      insert(*first);
      ++first;
    }
  }
  BSTree(std::initializer_list<value_type> list, Cmp cmp = Cmp())
      : BSTree(list.begin(), list.end(), cmp) {}
  BSTree(const BSTree& other) {
    alloc_ = other.alloc_;
    size_ = other.size_;
    comparator_ = other.comparator_;
    for (auto it = other.cbegin<PreorderTraverser<value_type>>();
         it != other.cend<PreorderTraverser<value_type>>(); ++it) {
      insert(*it);
    }
  };
  BSTree& operator=(const BSTree& other) {
    if (this == &other) {
      return *this;
    }
    alloc_ = other.alloc_;

    comparator_ = other.comparator_;
    clear();
    for (auto it = other.cbegin<PreorderTraverser<value_type>>();
         it != other.cend<PreorderTraverser<value_type>>(); ++it) {
      insert(*it);
    }
  };
  BSTree& operator=(std::initializer_list<value_type> il) {
    clear();
    for (auto it = il.begin(); it != il.end(); ++it) {
      insert(*it);
    }
    return *this;
  }
  ~BSTree() {
    for (auto it = begin<PostorderTraverser<value_type>>();
         it != end<PostorderTraverser<value_type>>();) {
      it = erase(it);
    }
  }

  template <typename TraverseType = InorderTraverser<value_type>>
  Iterator<TraverseType> begin() const {
    return cbegin<TraverseType>();
  }
  template <typename TraverseType = InorderTraverser<value_type>>
  Iterator<TraverseType> end() const {
    return cend<TraverseType>();
  }
  template <typename TraverseType = InorderTraverser<value_type>>
  Iterator<TraverseType> cbegin() const {
    if constexpr (std::is_same_v<TraverseType, PreorderTraverser<value_type>>) {
      return Iterator<TraverseType>(root_, *this);
    }
    if constexpr (std::is_same_v<TraverseType, InorderTraverser<value_type>>) {
      if (root_ == nullptr) return Iterator<TraverseType>(nullptr, *this);
      Node<value_type>* tmp = root_;
      while (tmp->left_ != nullptr) {
        tmp = tmp->left_;
      }
      return Iterator<TraverseType>(tmp, *this);
    }
    if constexpr (std::is_same_v<TraverseType,
                                 PostorderTraverser<value_type>>) {
      if (root_ == nullptr) return Iterator<TraverseType>(nullptr, *this);
      Node<value_type>* tmp = root_;
      while (tmp->left_ != nullptr && tmp->right_ != nullptr) {
        while (tmp->left_ != nullptr) {
          tmp = tmp->left_;
        }
        if (tmp->right_ != nullptr) {
          tmp = tmp->right_;
        }
      }
      return Iterator<TraverseType>(tmp, *this);
    }
  }
  template <typename TraverseType = InorderTraverser<value_type>>
  Iterator<TraverseType> cend() const {
    return Iterator<TraverseType>(nullptr, *this);
  }

  template<typename TraverseType = InorderTraverser<value_type>>
  std::reverse_iterator<Iterator<TraverseType>> crend() const {
    return std::reverse_iterator(cbegin<TraverseType>());
  }
  template<typename TraverseType = InorderTraverser<value_type>>
  std::reverse_iterator<Iterator<TraverseType>> crbegin() const {
    return std::reverse_iterator(cend<TraverseType>());
  }

  template<typename TraverseType = InorderTraverser<value_type>>
  auto rend() {
    return crend<TraverseType>();
  }
  template<typename TraverseType = InorderTraverser<value_type>>
  auto rbegin() {
    return crbegin<TraverseType>();
  }

  bool operator==(const BSTree& other) const {
    if (size() != other.size()) return false;
    auto it = cbegin(), other_it = other.cbegin();
    for (; it != cend() && other_it != other.cend(); ++it) {
      if (*it != *other_it) return false;
      ++other_it;
      ++it;
    }
    if (it != cend() || other_it != other.cend()) return false;
    return true;
  }
  bool operator!=(const BSTree& other) const;
  void swap(BSTree& other) {
    std::swap(root_, other.root_);
    std::swap(alloc_, other.alloc_);
    std::swap(size_, other.size_);
    std::swap(comparator_, other.comparator_);
  }
  size_type size() const {
    return size_;
  }
  size_type max_size() const {
    return std::numeric_limits<size_type>::max();
  }
  bool empty() const {
    return size_ == 0;
  }

  key_compare key_comp() const {
    return comparator_;
  };
  value_compare value_comp() const {
    return comparator_;
  }
  std::pair<iterator, bool> insert(const value_type& value) {
    bool is_inserted = false;
    Node<value_type>* node = new Node<value_type>(value);  // TODO new
    if (root_ == nullptr) {
      root_ = node;
      ++size_;
      return std::make_pair(iterator(node, *this), true);
    }
    Node<value_type>* tmp = root_;
    while (true) {
      if (comparator_(node->value_, tmp->value_)) {
        if (tmp->left_ == nullptr) {
          tmp->left_ = node;
          node->parent_ = tmp;
          ++size_;
          is_inserted = true;
          break;
        } else {
          tmp = tmp->left_;
        }
      } else if (comparator_(tmp->value_, node->value_)) {
        if (tmp->right_ == nullptr) {
          tmp->right_ = node;
          node->parent_ = tmp;
          ++size_;
          is_inserted = true;
          break;
        } else {
          tmp = tmp->right_;
        }
      } else {
        delete node;  // TODO delete
        return std::make_pair(end(), false);
      }
    }
    return std::make_pair(iterator(node, *this), is_inserted);
  }
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
      insert(*it);
    }
  }
  void insert(std::initializer_list<value_type> il) {
    insert(il.begin(), il.end());
  }
  node_type extract(key_type key) {
    auto it = find(key);
    if (it == end()) {
      return node_type();
    }
    return extract(it);
  }
  node_type extract(iterator position) {
    node_type res = *position;  // allocate, assume existing node
    node_type* node = position.node_;
    node_type* parent = node->parent_;
    if (node->left_ == nullptr && node->right_ == nullptr) {  // leaf
      if (parent == nullptr) {
        root_ = nullptr;
      } else if (parent->left_ == node) {
        parent->left_ = nullptr;
      } else {
        parent->right_ = nullptr;
      }
    }

    if (node->right_ != nullptr && node->left_ != nullptr) {  // both children
      node_type* tmp = node->right_;
      while (tmp->left_ != nullptr) {
        tmp = tmp->left_;
      }
      node->value_ = tmp->value_;
      if (tmp->right_ != nullptr) {
        tmp->parent_->left_ = tmp->right_;
      } else {
        tmp->parent_->left_ = nullptr;
      }
      delete tmp;  // TODO delete
    }

    if (node->right_ == nullptr) {  // only left child
      if (parent == nullptr) {
        root_ = node->left_;
      } else if (parent->left_ == node) {
        parent->left_ = node->left_;
      } else {
        parent->right_ = node->left_;
      }
    }
    if (node->left_ == nullptr) {  // only right child
      if (parent == nullptr) {
        root_ = node->right_;
      } else if (parent->left_ == node) {
        parent->left_ = node->right_;
      } else {
        parent->right_ = node->right_;
      }
    }
    delete node;  // TODO delete
    --size_;
    return res;
  }
  void merge(BSTree& other) {
    insert(other.begin(), other.end());
  }
  size_type erase(key_type key) {
    auto it = find(key);
    if (it == end()) {
      return 0;
    }

    return erase(it) == end() ? 1 : 0;
  }
  template <typename Iter = Iterator<InorderTraverser<value_type>>>
  Iter erase(Iter position) {
    node_type* node = position.node_;
    ++position;
    if (node == nullptr) return position;
    node_type* parent = node->parent_;
    if (node->left_ == nullptr && node->right_ == nullptr) {  // leaf
      if (parent == nullptr) {
        root_ = nullptr;
      } else if (parent->left_ == node) {
        parent->left_ = nullptr;
      } else {
        parent->right_ = nullptr;
      }
      delete node;  // TODO delete
      --size_;
      return position;
    }

    if (node->right_ != nullptr && node->left_ != nullptr) {  // both children
      node_type* tmp = node->right_;
      while (tmp->left_ != nullptr) {
        tmp = tmp->left_;
      }
      node->value_ = tmp->value_;
      if (tmp->right_ != nullptr) {
        tmp->parent_->left_ = tmp->right_;
      } else {
        tmp->parent_->left_ = nullptr;
      }
      delete tmp;   // TODO delete
      delete node;  // TODO delete
      --size_;
      return position;
    }

    if (node->right_ == nullptr) {  // only left child
      if (parent == nullptr) {
        root_ = node->left_;
      } else if (parent->left_ == node) {
        parent->left_ = node->left_;
      } else {
        parent->right_ = node->left_;
      }
    }
    if (node->left_ == nullptr) {  // only right child
      if (parent == nullptr) {
        root_ = node->right_;
      } else if (parent->left_ == node) {
        parent->left_ = node->right_;
      } else {
        parent->right_ = node->right_;
      }
    }
    delete node;  // TODO delete
    --size_;
    return position;
  }
  template <typename Iter>
  iterator erase(Iter first, Iter last) {
    iterator it = end();
    while (first != last) {
      it = erase(first++);
    }
    return it;
  }
  void clear() {
    erase(begin(), end());
  }
  iterator find(key_type key) const {
    if (root_ == nullptr) {
      return end();
    }
    node_type* node = root_;
    while (node != nullptr) {
      if (comparator_(key, node->value_)) {
        node = node->left_;
      } else if (comparator_(node->value_, key)) {
        node = node->right_;
      } else {
        return iterator(node, *this);
      }
    }
    return end();
  }
  size_type count(key_type key) const {
    auto it = find(key);
    return it != end() ? 1 : 0;
  }
  bool contains(key_type key) const {
    return find(key) != end();
  }
  iterator lower_bound(key_type key) const {
    return find(key);
  }
  iterator upper_bound(key_type key) const {
    return ++find(key);
  }

 private:
  Node<value_type>* root_ = nullptr;
  Alloc alloc_ = Alloc();
  Cmp comparator_ = Cmp();
  size_type size_ = 0;
};
