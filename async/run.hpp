#pragma once

#include <schedulers/view/view.hpp>
#include <utils/func.hpp>

namespace async {

void Run(schedulers::View view, vthread::Func func);
void Run(vthread::Func func);

}  // namespace async