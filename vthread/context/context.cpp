#include "context.hpp"
#include <vthread/context/trampoline.hpp>

#include <cassert>
#include <cstdlib>

namespace vthread::context {

	static void TrampolineRunner(void*, void*, void*, void*, void*, void*, void* arg) {
		auto* trampoline = static_cast<ITrampoline*>(arg);
		trampoline->Run();
	}

	void ExecutionContext::Setup(StackView stack, ITrampoline* trampoline) {
		void* stack_bottom = static_cast<char*>(stack.data()) + stack.size();
		rsp_ = SetupMachineContext(stack_bottom, reinterpret_cast<void*>(&TrampolineRunner), trampoline);
	}

	void ExecutionContext::SwitchTo(ExecutionContext& target) {
		SwitchMachineContext(&rsp_, &target.rsp_);
	}

	[[noreturn]] void ExecutionContext::ExitTo(ExecutionContext& target) {
		SwitchTo(target);
		std::abort();
	}

} // namespace vthread::context