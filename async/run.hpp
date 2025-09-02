#pragma once

#include <utils/func.hpp>
#include <schedulers/view/view.hpp>

namespace async {

	void Run(schedulers::View view, vthread::Func func);
	void Run(vthread::Func func);

}  // namespace async