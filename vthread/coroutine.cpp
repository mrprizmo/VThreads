#include "coroutine.hpp"
#include <memory>
#include <stdexcept>
#include "stack/stack_alloc.hpp"

namespace vthread {

Coroutine::Coroutine(Func func)
    : stack_(StackAllocator::Instance().Allocate()), func_(std::move(func)) {
    context_.Setup(stack_.MutView(), this);
}

void Coroutine::Resume() {
    if (IsDone()) {
        return;
    }
    caller_context_.SwitchTo(context_);
}

void Coroutine::Suspend() {
    context_.SwitchTo(caller_context_);
}

void Coroutine::Run() noexcept {
    try {
        func_();
    } catch (...) {
    }
    done_ = true;
    context_.ExitTo(caller_context_);
}

bool Coroutine::IsDone() const {
    return done_;
}

}  // namespace vthread