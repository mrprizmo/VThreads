#pragma once

#include <intrusive/list.hpp>

namespace schedulers::task {

  struct ITask {
    virtual void Run() noexcept {};
    virtual ~ITask() = default;
  };

  struct TaskBase : ITask, ListHook {
  };

}  // namespace schedulers::task