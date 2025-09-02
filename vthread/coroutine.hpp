#pragma once

#include <utils/func.hpp>
#include "stack/stack.hpp"
#include "context/trampoline.hpp"
#include "context/context.hpp"

namespace vthread {

  class Coroutine final : private ITrampoline {
  public:
    explicit Coroutine(Func func);

    void Resume();
    void Suspend();
    bool IsDone() const;

  private:
    void Run() noexcept override;

  private:
    Stack stack_;
    context::ExecutionContext context_;
    context::ExecutionContext caller_context_;
    Func func_;
    bool done_{false};
  };

}  // namespace vthread