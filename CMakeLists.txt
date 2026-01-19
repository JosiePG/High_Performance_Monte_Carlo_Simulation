cmake_minimum_required(VERSION 3.20)
project(monte_carlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    src/main.cpp
    src/monte_carlo_engine.cpp
    src/variance_reduction_module.cpp
    src/cache_aware_module.cpp
    src/imgui_setup.cpp
)

target_include_directories(main PRIVATE
    src
)
# Find packages installed by vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Link libraries to your executable
target_link_libraries(main PRIVATE
    glfw
    glad::glad
    imgui::imgui
)

# Link OpenGL on Windows (add this if you're on Windows)
if (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()
