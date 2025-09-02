#include "stack_alloc.hpp"
#include "stack.hpp"
#include <cstdlib>
#include <new>
#include <mutex>

namespace vthread {

	StackAllocator& StackAllocator::Instance() {
		static StackAllocator instance;
		return instance;
	}

	Stack StackAllocator::Allocate() {
		std::lock_guard guard(mutex_);
		if (!pool_.empty()) {
			void* ptr = pool_.back();
			pool_.pop_back();
			return Stack(ptr);
		}

		void* new_stack = std::malloc(Stack::kStackSize);
		if (!new_stack) {
			throw std::bad_alloc();
		}
		return Stack(new_stack);
	}

	void StackAllocator::Deallocate(void* stack_ptr) {
		std::lock_guard guard(mutex_);
		pool_.push_back(stack_ptr);
	}

	StackAllocator::~StackAllocator() {
		for (void* ptr : pool_) {
			std::free(ptr);
		}
	}

} // namespace vthread