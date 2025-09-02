#pragma once

#include <cstddef>

namespace vthread {

    class StackAllocator;

    class StackView {
    public:
        StackView(void* data, std::size_t size) : data_(data), size_(size) {}

        void* data() const { return data_; }
        std::size_t size() const { return size_; }

    private:
        void* data_;
        std::size_t size_;
    };

    class Stack {
    public:
        Stack() = default;
        ~Stack();

        Stack(const Stack&) = delete;
        Stack& operator=(const Stack&) = delete;

        Stack(Stack&& other) noexcept;
        Stack& operator=(Stack&& other) noexcept;

        StackView MutView() const;

        static constexpr std::size_t kStackSize = 128 * 1024; // 128 KiB
    private:
        friend class StackAllocator;
        explicit Stack(void* data);

        void* data_{nullptr};
    };

}  // namespace vthread