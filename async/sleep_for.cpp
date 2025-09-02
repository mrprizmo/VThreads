#include "sleep_for.hpp"
#include <vthread/vthread.hpp>

namespace async {

void SleepFor(schedulers::timer::Duration delay) {
    auto &self = vthread::VThread::Self();
    auto awaiter = SleepForAwaiter(delay);
    self.Await(awaiter);
}

}  // namespace async