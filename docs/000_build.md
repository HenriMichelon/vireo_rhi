Building & using Vireo RHI
===========================================================================

Dependencies
---------------------------------------------------------------------------
Most of the dependencies used by Vireo RHI are automatically configured
by the `CMakeLists.txt` of the engine except the Vulkan SDK and the build tools.

**Install the Vulkan SDK**
- [Vulkan SDK 1.3.x+](https://vulkan.lunarg.com/)

**Install the build tools**

For Microsoft Windows :
- [MSVC 19+ (VS 2022)](https://visualstudio.microsoft.com/fr/)
- [CMake 3.29+](https://cmake.org/download/) ([Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)) with a [build tool](https://github.com/ninja-build/ninja/releases)

Standalone build
---------------------------------------------------------------------------

- Clone the [ZeroZero repository](https://github.com/HenriMichelon/zero_zero)
- `cmake -B build -G Ninja -D CMAKE_BUILD_TYPE=Release` (adapt for your build tool)
- `cmake --build build`

