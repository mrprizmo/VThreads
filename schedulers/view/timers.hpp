#pragma once

#include <schedulers/view/view.hpp>
#include <schedulers/timer/scheduler.hpp>
#include <cassert>

namespace schedulers {

	inline timer::IScheduler& Timers(View v) {
		auto* s = std::get<timer::IScheduler*>(v);
		assert(s);
		return *s;
	}

}  // namespace schedulers