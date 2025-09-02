#pragma once

#include <schedulers/view/view.hpp>
#include <schedulers/task/scheduler.hpp>
#include <cassert>

namespace schedulers {

	inline task::IScheduler& Tasks(View v) {
		auto* s = std::get<task::IScheduler*>(v);
		assert(s);
		return *s;
	}

}  // namespace schedulers