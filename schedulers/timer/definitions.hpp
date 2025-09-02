#pragma once

#include <chrono>

namespace schedulers::timer {

	using Clock = std::chrono::steady_clock;
	using Duration = Clock::duration;
	using TimePoint = Clock::time_point;

}  // namespace schedulers::timer