cmake_minimum_required(VERSION 3.20)
project(monte_carlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    src/main.cpp
    src/simulation_helper.cpp
    src/imgui_setup.cpp
)

target_include_directories(main PRIVATE
    src
)

target_compile_options(main PRIVATE /arch:AVX2 /openmp:llvm )



find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(sleef CONFIG REQUIRED)


target_link_libraries(main PRIVATE
    glfw
    glad::glad
    imgui::imgui
    implot::implot
    sleef::sleef
)

if (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()
