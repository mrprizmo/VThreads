#pragma once

#include <utils/func.hpp>
#include "coroutine.hpp"

#include <memory>
#include <schedulers/view/view.hpp>
#include <schedulers/timer/timer.hpp>
#include <functional>
#include <thread>

namespace vthread {

  class VThread final : public schedulers::timer::TimerBase {
  public:
    VThread(schedulers::View view, Func func);

    ~VThread() = default;

    void Run() noexcept override;

    template <typename Awaiter>
    void Await(Awaiter& awaiter) {
      await_suspend_ = [&](VThread* vthread) {
        awaiter.await_suspend(vthread);
      };
      coro_.Suspend();
    }

    static VThread& Self();
    schedulers::View CurrentView() const;

  private:
    schedulers::View view_;
    Coroutine coro_;
    std::function<void(VThread*)> await_suspend_;
  };

}  // namespace vthread