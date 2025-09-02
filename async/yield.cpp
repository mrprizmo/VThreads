#include "yield.hpp"
#include <vthread/vthread.hpp>

namespace async {

  void Yield() {
    auto awaiter = YieldAwaiter();
    vthread::VThread::Self().Await(awaiter);
  }

}  // namespace async