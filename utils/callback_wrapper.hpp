#pragma once

#include <type_traits>
#include <utility>

namespace schedulers {

template <typename Base, typename CallBack>
class CallbackWrapper final : public Base {
public:
    explicit CallbackWrapper(CallBack &&cb) : cb_(std::move(cb)) {
    }

    void Run() noexcept override {
        cb_();
        delete this;
    }

private:
    CallBack cb_;
};

template <typename Base, typename CallBack>
auto *make_callback_wrapper(CallBack &&cb) {
    return new CallbackWrapper<Base, CallBack>(std::move(cb));
}

}  // namespace schedulers