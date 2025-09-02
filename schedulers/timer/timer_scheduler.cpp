#include "timer_scheduler.hpp"

namespace schedulers::timer {
	TimerScheduler::TimerScheduler(task::IScheduler* executor) : executor_(executor) {
	}

	void TimerScheduler::Schedule(Duration delay, TimerBase* timer) {
		timer->deadline = Clock::now() + delay;

		{
			std::lock_guard lock(mutex_);
			if (stop_) {
				return;
			}
			timers_.push(timer);
		}

		cv_.notify_one();
	}

	void TimerScheduler::Start() {
		thread_ = std::thread([this] {
		  RunLoop();
		});
	}

	void TimerScheduler::Stop() {
		{
			std::lock_guard lock(mutex_);
			stop_ = true;
		}

		cv_.notify_all();
		thread_.join();
	}

	void TimerScheduler::RunLoop() {
		while (true) {
			std::unique_lock lock(mutex_);

			if (stop_) {
				break;
			}

			if (timers_.empty()) {
				cv_.wait(lock);
				continue;
			}

			auto next_deadline_tp = timers_.top()->deadline;

			if (Clock::now() >= next_deadline_tp) {
				auto* timer = timers_.pop();
				lock.unlock();
				executor_->Submit(timer);
			} else {
				cv_.wait_until(lock, next_deadline_tp);
			}
		}
	}
} // namespace schedulers::timer