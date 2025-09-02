#pragma once

#include <atomic>
#include <thread>

namespace thread {

  class SpinLock {
  public:
    void Lock() {
      while (true) {
        if (!lock_.exchange(true, std::memory_order_acquire)) {
          break;
        }
        while (lock_.load(std::memory_order_relaxed)) {
          std::this_thread::yield();
        }
      }
    }

    bool TryLock() {
      return !lock_.exchange(true, std::memory_order_acquire);
    }

    void Unlock() {
      lock_.store(false, std::memory_order_release);
    }

    void lock() {
      Lock();
    }

    bool try_lock() {
      return TryLock();
    }

    void unlock() {
      Unlock();
    }

  private:
    std::atomic<bool> lock_{false};
  };

}  // namespace thread