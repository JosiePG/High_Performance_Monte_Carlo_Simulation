# High Performance Monte Carlo Simulation for Low-Latency European Option Pricing

## Build Instructions

This project uses:

* MSVC (Visual Studio Build Tools)
* CMake presets
* Ninja as the build system
* vcpkg for dependencies


## 1. Prerequisites

Make sure you have installed:

* CMake
* Visual Studio Build Tools
* Ninja build system
* vcpkg

---

## 2. How to install vcpkg (if not already)

```bash
git clone https://github.com/microsoft/vcpkg
```

Then bootstrap:

```bash
./vcpkg/bootstrap-vcpkg.bat
```

---

## 3. Install dependencies

From the project root:

```bash
vcpkg install
```

Dependencies are defined in `vcpkg.json`.

---

## 4. Opening project

You must open VS Code with the MSVC environment loaded:

1. Open:

   ```
   x64 Native Tools Command Prompt for VS 2022
   ```

2. Navigate to your project:

```bash
cd path/to/your/project
```

3. Launch VS Code:

```bash
code .
```

---

## 5. Configure project (using presets)

In VS Code:

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
```

---

## 6. Build

```
CMake: Build
```

Or via terminal:

```bash
cmake --build out/build/msvc-relwithdebinfo
```

---

## 7. Run

Executable will be located in:

```
out/build/msvc-relwithdebinfo/
```

Run:

```bash
./main.exe
```

---

##  Build Types

* `Debug` → debugging only (slow)
* `Release` → maximum performance
* `RelWithDebInfo` →  recommended (fast + debuggable)

---

## Performance Notes

This project enables:

* AVX2 vectorization (`/arch:AVX2`)
* OpenMP parallelism (`/openmp:llvm`)

For accurate performance measurements, use:

```
RelWithDebInfo or Release
```

---

## Project Structure

* `src/` → source files
* `CMakeLists.txt` → build configuration
* `CMakePresets.json` → build presets
* `vcpkg.json` → dependency definitions
