#pragma once

#include <schedulers/timer/scheduler.hpp>
#include <schedulers/task/scheduler.hpp>
#include <schedulers/timer/definitions.hpp>
#include <intrusive/priority_queue.hpp>
#include <schedulers/timer/timer.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

namespace schedulers::timer {

    class TimerScheduler final : public IScheduler {
    public:
        explicit TimerScheduler(task::IScheduler* executor);

        void Schedule(Duration delay, TimerBase* timer) override;

        void Start();

        void Stop();

    private:
        void RunLoop();

    private:
        task::IScheduler* executor_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
        bool stop_{false};

        IntrusivePriorityQueue<TimerBase> timers_;
    };

}  // namespace schedulers::timer