#pragma once
#include <schedulers/view/view.hpp>
#include <schedulers/task/thread_pool.hpp>
#include <schedulers/timer/timer_scheduler.hpp>
#include <cstddef>

namespace schedulers {

  class ScheduleContext {
  public:

    explicit ScheduleContext(std::size_t num_workers)
    : thread_pool_(num_workers), timer_scheduler_(&thread_pool_) {
    }

    operator View() {
      return {&thread_pool_, &timer_scheduler_};
    }

    ScheduleContext& WithTimers() {
      with_timers_ = true;
      return *this;
    }

    void Start();
    void Stop();

  private:
    task::ThreadPool thread_pool_;
    timer::TimerScheduler timer_scheduler_;
    bool with_timers_{false};
  };

} // namespace schedulers