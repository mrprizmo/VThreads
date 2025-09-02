#pragma once

#include <tuple>

namespace schedulers::task {
    struct IScheduler;
}

namespace schedulers::timer {
    struct IScheduler;
}

namespace schedulers {

    using View = std::tuple<task::IScheduler*, timer::IScheduler*>;

}  // namespace schedulers