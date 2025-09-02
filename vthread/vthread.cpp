#include "vthread.hpp"

#include <schedulers/schedule_task.hpp>
#include <schedulers/schedule_timer.hpp>

namespace vthread {

  thread_local VThread* current_vthread_ = nullptr;

  VThread::VThread(schedulers::View view, Func func)
      : view_(view), coro_(std::move(func)) {
  }

  schedulers::View VThread::CurrentView() const {
    return view_;
  }

  void VThread::Run() noexcept {
    current_vthread_ = this;
    coro_.Resume();
    current_vthread_ = nullptr;

    if (coro_.IsDone()) {
      delete this;
      return;
    }

    if (await_suspend_) {
      await_suspend_(this);
    }
  }

  VThread& VThread::Self() {
    return *current_vthread_;
  }

}  // namespace vthread