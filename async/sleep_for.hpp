#pragma once

#include <chrono>
#include <schedulers/schedule_timer.hpp>
#include <schedulers/timer/definitions.hpp>
#include <schedulers/view/view.hpp>
#include <vthread/vthread.hpp>

namespace async {

class SleepForAwaiter {
private:
    schedulers::timer::Duration delay_;

public:
    explicit SleepForAwaiter(schedulers::timer::Duration delay)
        : delay_(delay) {
    }

    void await_suspend(vthread::VThread *vthread) {
        schedulers::ScheduleTimer(vthread->CurrentView(), delay_, vthread);
    }
};

void SleepFor(schedulers::timer::Duration delay);

}  // namespace async