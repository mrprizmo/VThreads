#pragma once

#include <atomic>
#include <schedulers/schedule_task.hpp>
#include <schedulers/task/task.hpp>
#include <schedulers/view/view.hpp>
#include <vthread/vthread.hpp>

namespace syncr {

class Mutex {
private:
    class MutexAwaiter {
    public:
        explicit MutexAwaiter(Mutex &mutex) : mutex_(mutex) {
        }

        void await_suspend(vthread::VThread *vthread) {
            auto *waiter = static_cast<schedulers::task::TaskBase *>(vthread);

            while (true) {
                schedulers::task::TaskBase *old_head =
                    mutex_.state_.load(std::memory_order_acquire);

                if (old_head == &mutex_.dummy_) {
                    if (mutex_.state_.compare_exchange_strong(
                            old_head, nullptr, std::memory_order_acquire,
                            std::memory_order_relaxed
                        )) {
                        schedulers::ScheduleTask(
                            vthread->CurrentView(), vthread
                        );
                        return;
                    }

                    continue;
                }

                waiter->next_ = old_head;
                if (mutex_.state_.compare_exchange_weak(
                        old_head, waiter, std::memory_order_release,
                        std::memory_order_relaxed
                    )) {
                    return;
                }
            }
        }

    private:
        Mutex &mutex_;
    };

public:
    Mutex() {
        state_.store(&dummy_, std::memory_order_release);
    }

    void Lock() {
        auto *unlocked_state = &dummy_;
        if (state_.compare_exchange_strong(
                unlocked_state, nullptr, std::memory_order_acquire,
                std::memory_order_relaxed
            )) {
            return;
        }

        auto awaiter = MutexAwaiter(*this);
        vthread::VThread::Self().Await(awaiter);
    }

    bool TryLock() {
        auto *unlocked_state = &dummy_;
        return state_.compare_exchange_strong(
            unlocked_state, nullptr, std::memory_order_acquire,
            std::memory_order_relaxed
        );
    }

    void Unlock() {
        if (local_list_.load(std::memory_order_acquire)) {
            auto *new_owner = local_list_.load(std::memory_order_relaxed);
            local_list_.store(
                static_cast<schedulers::task::TaskBase *>(new_owner->next_),
                std::memory_order_release
            );

            new_owner->prev_ = new_owner->next_ = nullptr;
            schedulers::ScheduleTask(
                vthread::VThread::Self().CurrentView(), new_owner
            );
            return;
        }

        auto *waiters_stack =
            state_.exchange(nullptr, std::memory_order_acq_rel);

        if (waiters_stack == nullptr) {
            schedulers::task::TaskBase *expected = nullptr;

            if (state_.compare_exchange_strong(
                    expected, &dummy_, std::memory_order_release,
                    std::memory_order_relaxed
                )) {
                return;
            }

            waiters_stack = state_.exchange(nullptr, std::memory_order_acq_rel);
        }

        if (waiters_stack == nullptr) {
            return;
        }

        auto *head = waiters_stack;
        schedulers::task::TaskBase *prev = nullptr;
        while (head != nullptr) {
            auto *next = static_cast<schedulers::task::TaskBase *>(head->next_);
            head->next_ = prev;
            prev = head;
            head = next;
        }
        local_list_.store(prev, std::memory_order_release);

        auto *new_owner = local_list_.load(std::memory_order_relaxed);
        if (new_owner) {
            local_list_.store(
                static_cast<schedulers::task::TaskBase *>(new_owner->next_),
                std::memory_order_release
            );
            new_owner->prev_ = new_owner->next_ = nullptr;
            schedulers::ScheduleTask(
                vthread::VThread::Self().CurrentView(), new_owner
            );
        }
    }

    void lock() {
        Lock();
    }

    bool try_lock() {
        return TryLock();
    }

    void unlock() {
        Unlock();
    }

private:
    std::atomic<schedulers::task::TaskBase *> state_{nullptr};
    schedulers::task::TaskBase dummy_{};
    std::atomic<schedulers::task::TaskBase *> local_list_{nullptr};
};

}  // namespace syncr