#pragma once

#include <schedulers/task/scheduler.hpp>
#include <schedulers/task/task.hpp>
#include <thread/mpmc.hpp>
#include <cstddef>
#include <vector>
#include <thread>

namespace schedulers::task {

  class ThreadPool final : public IScheduler {
  public:
    explicit ThreadPool(std::size_t num_threads) : size_(num_threads) {
    }

    ~ThreadPool() = default;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void Start();

    void Submit(TaskBase* task) override {
      queue_.Push(task);
    }

    static ThreadPool* Current();

    void Stop() {
      queue_.Close();
      for (auto& thread : threads_) {
        thread.join();
      }
      threads_.clear();
    }

  private:
    std::size_t size_;
    std::vector<std::thread> threads_;
    MPMCQueue<TaskBase> queue_;
  };

}  // namespace schedulers::task