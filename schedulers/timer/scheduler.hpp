#pragma once

#include "definitions.hpp"
#include "timer.hpp"

namespace schedulers::timer {

  struct IScheduler {
    virtual void Schedule(Duration delay, TimerBase* handler) = 0;
    virtual ~IScheduler() = default;
  };

}  // namespace schedulers::timer