#pragma once

#include <mutex>
#include <queue>
#include <schedulers/schedule_task.hpp>
#include <schedulers/view/view.hpp>
#include <thread/spinlock.hpp>
#include <vthread/vthread.hpp>

namespace syncr {

class Event {
    class EventAwaiter {
    public:
        explicit EventAwaiter(Event &event) : event_(event) {
        }

        void await_suspend(vthread::VThread *vthread) {
            event_.waiters_.push(vthread);
            event_.spin_lock_.Unlock();
        }

    private:
        Event &event_;
    };

public:
    void Wait() {
        spin_lock_.Lock();
        if (fired_) {
            spin_lock_.Unlock();
            return;
        }

        auto awaiter = EventAwaiter(*this);
        vthread::VThread::Self().Await(awaiter);
    }

    void Fire() {
        std::queue<vthread::VThread *> waiters_to_run;

        {
            std::lock_guard guard(spin_lock_);
            if (fired_) {
                return;
            }
            fired_ = true;
            waiters_to_run.swap(waiters_);
        }

        if (waiters_to_run.empty()) {
            return;
        }

        auto view = vthread::VThread::Self().CurrentView();
        while (!waiters_to_run.empty()) {
            schedulers::ScheduleTask(view, waiters_to_run.front());
            waiters_to_run.pop();
        }
    }

private:
    thread::SpinLock spin_lock_;
    bool fired_{false};
    std::queue<vthread::VThread *> waiters_;
};

}  // namespace syncr