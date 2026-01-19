cmake_minimum_required(VERSION 3.20)
project(monte_carlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    src/main.cpp
    src/monte_carlo_engine.cpp
    src/variance_reduction_module.cpp
    src/cache_aware_module.cpp
)

target_include_directories(main PRIVATE
    src
)
