cmake_minimum_required(VERSION 3.15)

project(ConsoleAPIRateLimiter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(rate_limiter
    cli/main.cpp
    src/TokenBucketLimiter.cpp
    src/FixedWindowLimiter.cpp
    src/SlidingWindowCounterLimiter.cpp
    src/SlidingWindowLogLimiter.cpp
)

target_link_libraries(rate_limiter PRIVATE Threads::Threads)