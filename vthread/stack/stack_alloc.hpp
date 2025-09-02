#pragma once

#include <vector>
#include <mutex>

namespace vthread {

	class Stack;

	class StackAllocator {
	public:
		static StackAllocator& Instance();

		Stack Allocate();
		void Deallocate(void* stack_ptr);

	private:
		StackAllocator() = default;
		~StackAllocator();

		StackAllocator(const StackAllocator&) = delete;
		void operator=(const StackAllocator&) = delete;

		std::mutex mutex_;
		std::vector<void*> pool_;
	};

} // namespace vthread