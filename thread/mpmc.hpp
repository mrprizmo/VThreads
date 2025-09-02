#pragma once

#include <condition_variable>
#include <intrusive/list.hpp>
#include <mutex>
#include <optional>

template <typename T>
class MPMCQueue {
public:
    void Push(T *task) {
        std::lock_guard lock(mutex_);
        if (is_closed_) {
            return;
        }

        data_.PushBack(task);
        not_empty_.notify_one();
    }

    T *Pop() {
        std::unique_lock lock(mutex_);

        not_empty_.wait(lock, [this]() {
            return !data_.IsEmpty() || is_closed_;
        });

        if (data_.IsEmpty()) {
            return nullptr;
        }

        T *task = &data_.Front();
        data_.PopFront();
        return task;
    }

    bool IsEmpty() const {
        std::lock_guard lock(mutex_);
        return data_.IsEmpty();
    }

    void Close() {
        std::lock_guard lock(mutex_);
        is_closed_ = true;
        not_empty_.notify_all();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    List<T> data_;
    bool is_closed_{false};
};