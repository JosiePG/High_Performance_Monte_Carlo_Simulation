# High Performance Monte Carlo Simulation for Low-Latency European Option Pricing

## Download Executable
If you do not wish to build the project from source, you can download the pre-compiled executable:

## How to Run
1. Click on Release tab in Gitlab
2. Click on the OneDrive Link Monte Carlo Engine Release v1.0.0 attached to this release (Must use University of Leicester email).
3. Download the "jg535_release " file
4. Extract the contents to a folder on your computer.
5. Ensure that `glfw3.dll` and the `fonts/` directory are in the same folder as `main.exe`.
6. Double-click `main.exe` to launch the application.

## Prerequisites
- Windows 10/11
- Microsoft Visual C++ (if missing, download from [Microsoft](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist))

---

## Build Instructions

This project uses:

* MSVC (Visual Studio Build Tools)
* CMake
* vcpkg for dependency management

---

## 1. Prerequisites

Ensure the following are installed:

* CMake (>= 3.20)
* Visual Studio 2022 / 2026 **(Desktop development with C++)**
* Git
* vcpkg

### CPU Requirement (AVX2)

This executable requires a CPU with **AVX2 support**.

### Check Support (PowerShell)

```powershell
Get-CimInstance Win32_Processor | Select-Object Name
```

Search your CPU model online and confirm that AVX2 is listed, most modern Intel & AMD CPUs since 2013 support it.

---


## 2. Install vcpkg (if not already installed)

```bash
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
```

Set the environment variable:

```bash
set VCPKG_ROOT=C:\path\to\vcpkg
```

---

## 3. Install dependencies

From the project root:

```bash
vcpkg install
```

Dependencies are defined in `vcpkg.json`.

---

## 4. Important: MSVC Environment

All commands **must** be run from:

```
x64 Native Tools Command Prompt for Visual Studio
```

This ensures the MSVC compiler (`cl.exe`) is available.

---

# 5. Build via Command Line (Recommended)

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 6. Run the Executable

```bash
cd build/Release
main.exe
```

---

# 7. Using VS Code (Optional)

## Step 1: Open correct environment

1. Open:

   ```
   x64 Native Tools Command Prompt for Visual Studio
   ```

2. Navigate to the project:

```bash
cd path/to/project
```

3. Launch VS Code:

```bash
code .
```

---

## Step 2: Select compiler

In VS Code:

* Press `Ctrl + Shift + P`
* Search:

  ```
  CMake: Scan for Kits
  ```
* Then:

  ```
  CMake: Select a Kit
  ```
* Choose:

  ```
  Visual Studio (MSVC) - x64
  ```

---

## Step 3: Configure and build

* Press `Ctrl + Shift + P`
* Run:

  ```
  CMake: Configure
  ```
* Then:

  ```
  CMake: Build
  ```

---

## Step 4: Run

The executable will be in:

```
build/Debug/
```

or

```
build/Release/
```

Run:

```bash
./main.exe
```

---

## 8. Build Types

* `Debug` → debugging only (slow)
* `Release` → maximum performance
* `RelWithDebInfo` → recommended (performance + debug symbols)

Example:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo
```

---

## 9. Performance Notes

This project includes:

* AVX2 vectorization (`/arch:AVX2`)
* OpenMP parallelism (`/openmp`)

For benchmarking, use:

```
Release or RelWithDebInfo
```

---

## 10. Runtime Notes

* The executable must be run from the build directory:

  ```
  build/Release
  ```
* Fonts and assets are automatically copied next to the executable during build.
* Do not move the executable outside the build folder.

---

## 11. Project Structure

* `src/` → source files
* `fonts/` → runtime assets
* `CMakeLists.txt` → build configuration
* `vcpkg.json` → dependency definitions

---

## Notes for Markers

* The project has been tested using:

  * MSVC (Visual Studio Build Tools)
  * CMake (default Visual Studio generator)
  * vcpkg (manifest mode)

If build issues occur:

* Ensure you are using:

  ```
  x64 Native Tools Command Prompt
  ```
* Ensure `VCPKG_ROOT` is set correctly
* Run:

  ```bash
  vcpkg install
  ```

---

## Quick Start (Recommended)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cd build/Release
main.exe
```
