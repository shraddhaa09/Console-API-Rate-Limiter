cmake_minimum_required(VERSION 3.15)
project(api_rate_limiter CXX)

# Optimization and C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Threading support
find_package(Threads REQUIRED)

# Project Structure:
# Assuming main.cpp is in the project root based on your file tree.
# Headers remain in /include.
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define Executable
# Using 'api_rate_limiter' as the target name and main.cpp in the root.
add_executable(api_rate_limiter 
    cli/main.cpp
)

# Include Directories
# Added ${INCLUDE_DIR} so you can use #include "Trie.h" instead of #include "include/Trie.h"
target_include_directories(api_rate_limiter PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}
)

# Link Libraries
target_link_libraries(api_rate_limiter PRIVATE Threads::Threads)

# Set Output Name and Location
set_target_properties(api_rate_limiter PROPERTIES
    OUTPUT_NAME "api_rate_limiter"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Copy requests.txt to the build directory so the executable can find it
add_custom_command(TARGET api_rate_limiter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/requests.txt"
    "$<TARGET_FILE_DIR:api_rate_limiter>/requests.txt"
)