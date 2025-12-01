cmake_minimum_required(VERSION 4.0)
project(Final_Year_Project)

set(CMAKE_CXX_STANDARD 20)

add_executable(Final_Year_Project
        src/main.cpp
        src/monte_carlo_simulation.cpp
        src/monte_carlo_simulation.h)

target_include_directories(Final_Year_Project PRIVATE include)