#pragma once

#include <algorithm>
#include <iterator>
#include <new>

class ListHook {
public:
    ListHook() : prev_(this), next_(this) {
    }

    bool IsLinked() const {
        return prev_ != this || next_ != this;
    }

    void Unlink() {
        prev_->next_ = next_;
        next_->prev_ = prev_;
        prev_ = this;
        next_ = this;
    }

    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook &) = delete;

    ListHook *prev_, *next_;

private:
    template <class T>
    friend class List;

    void LinkBefore(ListHook *other) {
        other->prev_->next_ = this;
        prev_ = other->prev_;
        other->prev_ = this;
        next_ = other;
    }
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        explicit Iterator(ListHook *node) : current_(node) {
        }

        Iterator &operator++() {
            current_ = current_->next_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator &operator--() {
            current_ = current_->prev_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        T &operator*() const {
            return *std::launder(static_cast<T *>(current_));
        }

        T *operator->() const {
            return std::launder(static_cast<T *>(current_));
        }

        bool operator==(const Iterator &rhs) const {
            return current_ == rhs.current_;
        }

        bool operator!=(const Iterator &rhs) const {
            return current_ != rhs.current_;
        }

    private:
        ListHook *current_;
    };

    List() {
        dummy_.next_ = &dummy_;
        dummy_.prev_ = &dummy_;
    }

    List(const List &) = delete;

    List(List &&other) noexcept {
        if (!other.IsEmpty()) {
            dummy_.prev_ = other.dummy_.prev_;
            dummy_.next_ = other.dummy_.next_;
            dummy_.prev_->next_ = &dummy_;
            dummy_.next_->prev_ = &dummy_;
            other.dummy_.prev_ = &other.dummy_;
            other.dummy_.next_ = &other.dummy_;
        }
    }

    ~List() {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    List &operator=(const List &) = delete;

    List &operator=(List &&other) noexcept {
        if (this != &other) {
            dummy_.Unlink();
            if (!other.IsEmpty()) {
                dummy_.prev_ = other.dummy_.prev_;
                dummy_.next_ = other.dummy_.next_;
                dummy_.prev_->next_ = &dummy_;
                dummy_.next_->prev_ = &dummy_;
                other.dummy_.prev_ = &other.dummy_;
                other.dummy_.next_ = &other.dummy_;
            }
        }
        return *this;
    }

    [[nodiscard]] bool IsEmpty() const {
        return dummy_.next_ == &dummy_;
    }

    void PushBack(T *elem) {
        elem->LinkBefore(&dummy_);
    }

    void PushFront(T *elem) {
        elem->LinkBefore(dummy_.next_);
    }

    T &Front() {
        return *std::launder(static_cast<T *>(dummy_.next_));
    }

    const T &Front() const {
        return *std::launder(static_cast<const T *>(dummy_.next_));
    }

    T &Back() {
        return *std::launder(static_cast<T *>(dummy_.prev_));
    }

    const T &Back() const {
        return *std::launder(static_cast<const T *>(dummy_.prev_));
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }

    void PopFront() {
        dummy_.next_->Unlink();
    }

    Iterator Begin() {
        return Iterator(dummy_.next_);
    }

    Iterator End() {
        return Iterator(&dummy_);
    }

    Iterator IteratorTo(T *element) {
        return Iterator(static_cast<ListHook *>(element));
    }

private:
    ListHook dummy_;
};