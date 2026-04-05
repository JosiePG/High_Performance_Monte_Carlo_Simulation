# High Performance Monte Carlo Simulation for Low-Latency European Option Pricing

## Build Instructions

### 1. Install vcpkg
git clone https://github.com/microsoft/vcpkg

### 2. Install dependencies
vcpkg install

### 3. Configure project
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake

### 4. Build
cmake --build build

### 5. Run
./build/main

## Information about this repository

This is the repository that you are going to use **individually** for developing your project. Please use the resources provided in the module to learn about **plagiarism** and how plagiarism awareness can foster your learning.

Regarding the use of this repository, once a feature (or part of it) is developed and **working** or parts of your system are integrated and **working**, define a commit and push it to the remote repository. You may find yourself making a commit after a productive hour of work (or even after 20 minutes!), for example. Choose commit message wisely and be concise.

Please choose the structure of the contents of this repository that suits the needs of your project but do indicate in this file where the main software artefacts are located.
