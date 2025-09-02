#include <gtest/gtest.h>
#include <schedulers/schedule_context/schedule_context.hpp>
#include <async/run.hpp>
#include <async/yield.hpp>
#include <async/sleep_for.hpp>
#include <sync/mutex.hpp>
#include <sync/event.hpp>
#include <sync/conditional_variable.hpp>
#include <atomic>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

TEST(VThreads, SimpleTask) {
    schedulers::ScheduleContext sched{4};
    sched.Start();

    std::atomic<int> x = 0;

    async::Run(sched, [&]() {
        x = 1;
    });

    sched.Stop();
    ASSERT_EQ(x, 1);
}

TEST(VThreads, MultipleTasks) {
    schedulers::ScheduleContext sched{4};
    sched.Start();

    std::atomic<int> counter = 0;

    for (int i = 0; i < 3; ++i) {
        async::Run(sched, [&]() {
            counter++;
        });
    }

    sched.Stop();
    ASSERT_EQ(counter, 3);
}

TEST(VThreads, NestedRun) {
    schedulers::ScheduleContext sched{4};
    sched.Start();

    std::atomic<int> counter = 0;

    async::Run(sched, [&]() {
        counter++;
        async::Run([&]() {
            counter++;
            async::Run([&]() {
                counter++;
            });
        });
    });

    std::this_thread::sleep_for(100ms);
    sched.Stop();
    ASSERT_EQ(counter, 3);
}

TEST(VThreads, YieldInterleaving) {
    schedulers::ScheduleContext sched{1};
    sched.Start();

    std::vector<int> execution_order;
    const int ITERS = 5;

    async::Run(sched, [&]() {
        for (int i = 0; i < ITERS; ++i) {
            execution_order.push_back(1);
            async::Yield();
        }
    });

    async::Run(sched, [&]() {
        for (int i = 0; i < ITERS; ++i) {
            execution_order.push_back(2);
            async::Yield();
        }
    });

    std::this_thread::sleep_for(100ms);
    sched.Stop();

    ASSERT_EQ(execution_order.size(), ITERS * 2);
    bool interleaved = false;
    for (std::size_t i = 0; i < execution_order.size() - 1; ++i) {
        if (execution_order[i] != execution_order[i+1]) {
            interleaved = true;
            break;
        }
    }
    ASSERT_TRUE(interleaved);
}

TEST(VThreads, SleepFor) {
    schedulers::ScheduleContext sched{4};
    sched.WithTimers().Start();

    auto start_time = std::chrono::steady_clock::now();

    std::atomic<bool> sleep_flag = false;

    async::Run(sched, [&]() {
        async::SleepFor(100ms);
        sleep_flag.store(true);
    });

    while(!sleep_flag.load()) {
        std::this_thread::yield();
    }
    sched.Stop();

    auto duration = std::chrono::steady_clock::now() - start_time;
    ASSERT_GE(duration, 100ms);
}

TEST(VThreads, Mutex) {
    schedulers::ScheduleContext sched{4};
    sched.Start();

    int unsafe_counter = 0;
    syncr::Mutex mutex;

    for (int i = 0; i < 3; i++) {
        async::Run(sched, [&]() {
            for (int i = 0; i < 1000; ++i) {
                std::lock_guard guard(mutex);
                unsafe_counter++;
            }
        });
    }

    std::this_thread::sleep_for(100ms);
    sched.Stop();

    ASSERT_EQ(unsafe_counter, 3000);
}

TEST(VThreads, Event) {
    schedulers::ScheduleContext sched{4};
    sched.WithTimers().Start();

    syncr::Event event;
    std::atomic<bool> event_fired = false;
    std::atomic<bool> sleep_flag = false;

    async::Run(sched, [&]() {
        event.Wait();
        event_fired = true;
    });

    async::Run(sched, [&]() {
        async::SleepFor(10ms);
        event.Fire();
        sleep_flag.store(true);
    });

    while (!sleep_flag.load()) {
      std::this_thread::yield();
    }
    sched.Stop();

    ASSERT_TRUE(event_fired);
}

TEST(VThreads, CondVarNotifyOne) {
    schedulers::ScheduleContext sched{1};
    sched.WithTimers().Start();

    syncr::Mutex mutex;
    syncr::ConditionalVariable cv;
    bool ready = false;
    std::atomic<bool> done = false;

    // Consumer
    async::Run(sched, [&]() {
        std::unique_lock lock(mutex);
        while (!ready) {
            cv.Wait(mutex);
        }
        done = true;
    });

    // Producer
    async::Run(sched, [&]() {
        async::SleepFor(10ms);
        {
            std::lock_guard guard(mutex);
            ready = true;
        }
        cv.NotifyOne();
    });

    while (!ready) {
      std::this_thread::yield();
    }
    sched.Stop();

    ASSERT_TRUE(done.load());
}

TEST(VThreads, CondVarNotifyAll) {
    schedulers::ScheduleContext sched{4};
    sched.WithTimers().Start();

    syncr::Mutex mutex;
    syncr::ConditionalVariable cv;
    bool ready = false;
    const int consumers_count = 5;
    std::atomic<int> done_count = 0;

    for (int i = 0; i < consumers_count; ++i) {
        async::Run(sched, [&]() {
            std::unique_lock lock(mutex);
            while (!ready) {
                cv.Wait(mutex);
            }
            done_count++;
        });
    }

    async::Run(sched, [&]() {
        async::SleepFor(20ms);
        {
            std::lock_guard guard(mutex);
            ready = true;
        }
        cv.NotifyAll();
    });

    std::this_thread::sleep_for(100ms);
    sched.Stop();
    ASSERT_EQ(done_count.load(), consumers_count);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}