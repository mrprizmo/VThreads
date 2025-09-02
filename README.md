# VThreads

VThreads is a C++17 library for virtual threads, implemented using stackful coroutines.

## Prerequisites

*   C++17 compatible compiler (GCC, Clang, MSVC)
*   CMake 3.16 or later
*   GoogleTest (fetched automatically by CMake)

## Roadmap

1.  **I/O Operations**: Add support for asynchronous socket operations.
2.  **Testing**: Increase code coverage with more unit tests and add stress tests to validate performance and stability.
3.  **Scheduler Optimization**: Enhance scheduler performance by introducing thread-local queues and replacing blocking data structures with lock-free alternatives.
4.  **Stackless Coroutines**: Add support for C++20 stackless coroutines.

## Supported Operations and Primitives

### Asynchronous Operations
*   **`async::Run`**: Launches a new virtual coroutine.
*   **`async::Yield`**: Cooperatively yields control to the scheduler, allowing other coroutines to run.
*   **`async::SleepFor`**: Suspends a coroutine's execution for a specified duration without blocking an OS thread.

### Synchronization Primitives
*   **`syncr::Mutex`**: A mutex implementation with lock-free guarantee.
*   **`syncr::Event`**: Allows one or more coroutines to wait until an event is signaled.
*   **`syncr::ConditionalVariable`**: Provides a mechanism to wait until a specific condition is met.

## Usage Example

Here's a simple example demonstrating how to create and run VThreads:

```cpp
#include <iostream>
#include <schedulers/schedule_context/schedule_context.hpp>
#include <async/run.hpp>
#include <async/yield.hpp>

int main() {
    // Create a scheduler with 1 worker threads
    schedulers::ScheduleContext sched{1};
    sched.Start();

    // Run a VThread
    async::Run(sched, []() {
        std::cout << "Hello from VThread 1!" << std::endl;
        async::Yield();
        std::cout << "VThread 1 resumes." << std::endl;
    });

    // Run another VThread
    async::Run(sched, []() {
        std::cout << "Hello from VThread 2!" << std::endl;
    });

    // Stop the scheduler
    sched.Stop();

    return 0;
}
```
