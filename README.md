# High Performance Monte Carlo Simulation for Low-Latency European Option Pricing

## Build Instructions

This project uses:

* MSVC (Visual Studio Build Tools)
* CMake (with presets)
* Ninja build system
* vcpkg for dependency management

---

## 1. Prerequisites

Ensure the following are installed:

* CMake (>= 3.20)
* Visual Studio 2022 **(Desktop development with C++)** or Build Tools
* Git
* vcpkg
* Ninja (see below)

---

## 2. Install Ninja

Ninja is required as the build system.

**Option 1 (Recommended – via package manager):**

```bash
choco install ninja
```

**Option 2 (Manual install):**

* Download from: https://github.com/ninja-build/ninja/releases
* Add the executable to your system PATH

To verify installation:

```bash
ninja --version
```

---

## 3. Install vcpkg (if not already installed)

```bash
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
```

Set the environment variable (required):

```bash
set VCPKG_ROOT=C:\path\to\vcpkg
```

---

## 4. Install dependencies

From the project root:

```bash
vcpkg install
```

Dependencies are defined in `vcpkg.json`.

---

## 5. Important: MSVC Environment

This project requires the MSVC compiler (`cl.exe`), so all commands must be run from:

```
x64 Native Tools Command Prompt for VS 2022
```

---

## 6. Build via Command Line (Recommended)

From the project root:

```bash
cmake --preset msvc-relwithdebinfo
cmake --build --preset msvc-relwithdebinfo
```

---

## 7. Run the Executable

```bash
cd out/build/msvc-relwithdebinfo
main.exe
```

---

## 8. Using VS Code (Optional)

1. Open **x64 Native Tools Command Prompt for VS 2022**
2. Navigate to project:

   ```bash
   cd path/to/project
   ```
3. Launch VS Code:

   ```bash
   code .
   ```

Inside VS Code:

* Press `Ctrl + Shift + P`
* Select:

  ```
  CMake: Select Configure Preset
  ```
* Choose:

  ```
  msvc-relwithdebinfo
  ```

Then run:

```
CMake: Configure
CMake: Build
```

---

## 9. Build Types

* `Debug` → debugging only (slow)
* `Release` → maximum performance
* `RelWithDebInfo` → recommended (performance + debugging symbols)

---

## 10. Performance Notes

This project makes use of:

* AVX2 vectorization (`/arch:AVX2`)
* OpenMP parallelism (`/openmp:llvm`)
* Cache-aware data layout
* SIMD intrinsics (AVX2)

For accurate benchmarking, use:

```
RelWithDebInfo or Release
```

---

## 11. Runtime Notes

* The executable must be run from the build directory:

  ```
  out/build/msvc-relwithdebinfo
  ```
* Ensure any required assets (e.g., fonts) are located relative to the executable.

---

## 12. Project Structure

* `src/` → source files
* `CMakeLists.txt` → build configuration
* `CMakePresets.json` → build presets
* `vcpkg.json` → dependency definitions

---

## Notes for Markers

* The project has been tested on Windows with MSVC + Ninja.
* If build issues occur, ensure:

  * MSVC environment is loaded
  * `VCPKG_ROOT` is set correctly
  * Ninja is installed and available in PATH
  * Dependencies are installed via `vcpkg install`
