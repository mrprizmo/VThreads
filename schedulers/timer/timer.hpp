#pragma once

#include <schedulers/task/task.hpp>
#include "definitions.hpp"

namespace schedulers::timer {

  struct TimerBase : public schedulers::task::TaskBase {
  public:
    TimePoint deadline{};

    bool operator<(const TimerBase& other) const {
      return deadline > other.deadline;
    }
  };

}  // namespace schedulers::timer