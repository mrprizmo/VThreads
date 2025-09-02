#pragma once

#include <schedulers/schedule_task.hpp>
#include <schedulers/view/view.hpp>
#include <vthread/vthread.hpp>

namespace async {

class YieldAwaiter {
public:
    void await_suspend(vthread::VThread *vthread) {
        schedulers::ScheduleTask(vthread->CurrentView(), vthread);
    }
};

void Yield();

}  // namespace async