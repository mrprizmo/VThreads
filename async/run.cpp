#include "run.hpp"

#include <vthread/vthread.hpp>
#include <schedulers/schedule_task.hpp>

namespace async {

  void Run(schedulers::View view, vthread::Func func) {
    auto* vthread = new vthread::VThread(view, std::move(func));
    schedulers::ScheduleTask(view, vthread);
  }

  void Run(vthread::Func func) {
    auto& self = vthread::VThread::Self();
    Run(self.CurrentView(), std::move(func));
  }

}