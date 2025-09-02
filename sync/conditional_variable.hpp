#pragma once

#include <sync/mutex.hpp>
#include <thread/spinlock.hpp>
#include <vthread/vthread.hpp>
#include <schedulers/schedule_task.hpp>
#include <intrusive/list.hpp>
#include <atomic>
#include <mutex>

namespace syncr {

class ConditionalVariable {
private:
    class CondVarAwaiter {
    public:
        CondVarAwaiter(ConditionalVariable& cv)
            : cv_(cv) {}

        void await_suspend(vthread::VThread* self) {
            cv_.waiters_.PushBack(self);
            cv_.wait_lock_.unlock();
        }

    private:
        ConditionalVariable& cv_;
    };

public:
    ConditionalVariable() = default;

    ConditionalVariable(const ConditionalVariable&) = delete;
    ConditionalVariable& operator=(const ConditionalVariable&) = delete;

    void Wait(Mutex& mutex) {
        uint64_t current_seq = sequence_.load(std::memory_order_acquire);

        mutex.unlock();
        counter_.fetch_add(1, std::memory_order_release);

        while (sequence_.load(std::memory_order_acquire) == current_seq) {
            wait_lock_.lock();
            if (sequence_.load(std::memory_order_acquire) == current_seq) {
                CondVarAwaiter awaiter(*this);
                vthread::VThread::Self().Await(awaiter);
            } else {
                wait_lock_.unlock();
            }
        }

        counter_.fetch_sub(1, std::memory_order_release);
        mutex.lock();
    }

    void NotifyOne() {
        vthread::VThread* to_wake = nullptr;

        sequence_.fetch_add(1, std::memory_order_acq_rel);

        if (counter_.load(std::memory_order_acquire) > 0) {
            std::lock_guard guard(wait_lock_);
            if (!waiters_.IsEmpty()) {
                to_wake = static_cast<vthread::VThread*>(&waiters_.Front());
                waiters_.PopFront();
            }
        }

        if (to_wake) {
            schedulers::ScheduleTask(to_wake->CurrentView(), to_wake);
        }
    }

    void NotifyAll() {
        List<schedulers::task::TaskBase> waking_waiters;

        sequence_.fetch_add(1, std::memory_order_acq_rel);

        if (counter_.load(std::memory_order_acquire) > 0) {
            std::lock_guard guard(wait_lock_);

            while(!waiters_.IsEmpty()) {
                auto* waiter = &waiters_.Front();
                waiters_.PopFront();
                waking_waiters.PushBack(waiter);
            }
        }

        if (waking_waiters.IsEmpty()) {
            return;
        }

        auto view = vthread::VThread::Self().CurrentView();
        while(!waking_waiters.IsEmpty()) {
            auto* waiter = &waking_waiters.Front();
            waking_waiters.PopFront();
            schedulers::ScheduleTask(view, static_cast<vthread::VThread*>(waiter));
        }
    }

private:
    thread::SpinLock wait_lock_;
    List<schedulers::task::TaskBase> waiters_;
    std::atomic<uint64_t> sequence_{0}, counter_{0};
};

} // namespace syncr