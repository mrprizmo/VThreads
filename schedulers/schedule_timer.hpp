#pragma once

#include <schedulers/timer/timer.hpp>
#include <schedulers/view/timers.hpp>
#include <utils/callback_wrapper.hpp>

namespace schedulers {

template <typename T>
void ScheduleTimer(View sc, timer::Duration delay, T handler) {
    if constexpr (std::is_base_of_v<
                      timer::TimerBase, std::remove_pointer_t<T>>) {
        Timers(sc).Schedule(delay, handler);
    } else {
        auto *callback =
            make_callback_wrapper<timer::TimerBase>(std::move(handler));
        Timers(sc).Schedule(delay, callback);
    }
}

}  // namespace schedulers