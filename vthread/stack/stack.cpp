#include "stack.hpp"
#include "stack_alloc.hpp"

namespace vthread {

	Stack::Stack(void* data) : data_(data) {}

	Stack::~Stack() {
		if (data_) {
			StackAllocator::Instance().Deallocate(data_);
		}
	}

	Stack::Stack(Stack&& other) noexcept : data_(other.data_) {
		other.data_ = nullptr;
	}

	Stack& Stack::operator=(Stack&& other) noexcept {
		if (this != &other) {
			if (data_) {
				StackAllocator::Instance().Deallocate(data_);
			}
			data_ = other.data_;
			other.data_ = nullptr;
		}
		return *this;
	}

	StackView Stack::MutView() const {
		return {data_, kStackSize};
	}

} // namespace vthread