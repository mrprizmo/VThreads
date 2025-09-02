#pragma once

#include <schedulers/task/task.hpp>
#include <schedulers/view/tasks.hpp>
#include <utils/callback_wrapper.hpp>

namespace schedulers {

template <typename T>
void ScheduleTask(View sc, T task) {
    if constexpr (std::is_base_of_v<task::TaskBase, std::remove_pointer_t<T>>) {
        Tasks(sc).Submit(task);
    } else {
        auto *callback = make_callback_wrapper<task::TaskBase>(std::move(task));
        Tasks(sc).Submit(callback);
    }
}

}  // namespace schedulers