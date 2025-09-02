#include "thread_pool.hpp"

namespace schedulers::task {

  thread_local ThreadPool* current_thread_pool_ = nullptr;

  ThreadPool* ThreadPool::Current() {
    return current_thread_pool_;
  }

  void ThreadPool::Start() {
    for (std::size_t i = 0; i < size_; ++i) {
      threads_.emplace_back([this]() noexcept {
        current_thread_pool_ = this;
        while (auto* task = queue_.Pop()) {
          try {
            task->Run();
          } catch (...) {
            // Task threw exception, ignore
          }
        }
      });
    }
  }

}  // namespace schedulers::task