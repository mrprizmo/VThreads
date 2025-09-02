cmake_minimum_required(VERSION 3.16)

project(VThreads LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

enable_language(ASM)

file(GLOB_RECURSE VTHREADS_SOURCES
        "async/*.cpp"
        "schedulers/schedule_context/schedule_context.cpp"
        "schedulers/task/thread_pool.cpp"
        "schedulers/timer/timer_scheduler.cpp"
        "vthread/context/context.S"
        "vthread/context/context.cpp"
        "vthread/stack/stack.cpp"
        "vthread/stack/stack_alloc.cpp"
        "vthread/coroutine.cpp"
        "vthread/vthread.cpp"
)

add_library(vthreads ${VTHREADS_SOURCES})

target_include_directories(vthreads PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vthreads PRIVATE pthread)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(vthreads_tests tests/vthreads_test.cpp)

target_link_libraries(vthreads_tests PRIVATE vthreads GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(vthreads_tests)