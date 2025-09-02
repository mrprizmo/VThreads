#pragma once

#include "task.hpp"

namespace schedulers::task {

  struct IScheduler {
    virtual void Submit(TaskBase* task) = 0;
    virtual ~IScheduler() = default;
  };

}  // namespace schedulers::task