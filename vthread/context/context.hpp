#pragma once

#include <vthread/context/trampoline.hpp>
#include <vthread/stack/stack.hpp>

extern "C" {
	void* SetupMachineContext(void* stack, void* trampoline, void* arg);
	void SwitchMachineContext(void** from_rsp, void** to_rsp);
}

namespace vthread::context {

	class ExecutionContext {
	public:
		ExecutionContext() : rsp_(nullptr) {}

		ExecutionContext(const ExecutionContext&) = delete;
		ExecutionContext& operator=(const ExecutionContext&) = delete;
		ExecutionContext(ExecutionContext&&) = delete;
		ExecutionContext& operator=(ExecutionContext&&) = delete;

		void Setup(StackView stack, ITrampoline* trampoline);
		void SwitchTo(ExecutionContext& target);
		[[noreturn]] void ExitTo(ExecutionContext& target);

	private:
		void* rsp_;
	};

}  // namespace vthread::context