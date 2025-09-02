#pragma once

namespace vthread {

	struct ITrampoline {
		virtual void Run() noexcept = 0;
		virtual ~ITrampoline() = default;
	};

}  // namespace vthread