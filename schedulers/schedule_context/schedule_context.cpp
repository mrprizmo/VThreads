#include "schedule_context.hpp"

namespace schedulers {

  void ScheduleContext::Start() {
    thread_pool_.Start();
    if (with_timers_) {
      timer_scheduler_.Start();
    }
  }

  void ScheduleContext::Stop() {
    if (with_timers_) {
      timer_scheduler_.Stop();
    }
    thread_pool_.Stop();
  }

}  // namespace schedulers