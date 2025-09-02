#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <intrusive/list.hpp>
#include <new>

template <typename T, typename Compare = std::less<T>>
class IntrusivePriorityQueue {
     using Node = ListHook;
     Node* root = nullptr;
     Compare comp;

     Node* merge(Node* x, Node* y) noexcept {
          if (!x) {
               return y;
          }
          if (!y) {
               return x;
          }

          if (comp(*std::launder(static_cast<T*>(x)), *std::launder(static_cast<T*>(y)))) {
               std::swap(x, y);
          }

          x->next_ = merge(x->next_, y);
          std::swap(x->prev_, x->next_);

          return x;
     }

public:
     IntrusivePriorityQueue() = default;

     void push(T* item) noexcept {
          Node* node = static_cast<Node*>(item);
          node->prev_ = nullptr;
          node->next_ = nullptr;
          root = merge(root, node);
     }

     T* pop() noexcept {
          if (!root) {
               return nullptr;
          }
          Node* old_root = root;
          root = merge(root->prev_, root->next_);
          old_root->prev_ = nullptr;
          old_root->next_ = nullptr;
          return std::launder(static_cast<T*>(old_root));
     }

     const T* top() const noexcept {
          return root ? std::launder(static_cast<const T*>(root)) : nullptr;
     }

     bool empty() const noexcept {
          return root == nullptr;
     }

     void clear() noexcept {
          root = nullptr;
     }
};