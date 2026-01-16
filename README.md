# Vireo Rendering Hardware Interface

Vireo RHI is an open-source library written in modern C++ that implements a common abstraction layer over multiple 3D graphics APIs. It provides a low-level, high-performance interface designed to mimic modern graphics APIs like Vulkan and DirectX 12 while hiding their boilerplate and complexity.

## Key Features

- **Multi-API Support**: 
    - **Vulkan 1.3**
    - **DirectX 12**
- **Modern C++ Architecture**: Built with **C++23** and utilizes **C++ Modules** for faster compilation and clean code.
- **Low-Level Abstraction**: Abstraction of core modern graphics concepts:
    - Pipelines (Graphic & Compute)
    - Resource Management (Buffers, Images, Samplers)
    - Command Recording (Command Lists, Allocators)
    - Synchronization (Fences, Semaphores, Barriers)
    - Descriptor Management (Layouts & Sets)
    - Presentation (Swap Chains)
- **Validation & Debugging**: Integrated support for graphics API validation and debug layers.
- **Lua Bindings**: Optional Lua 5.4+ bindings for high-level logic and rapid prototyping.
- **Cross-Platform**: Designed for Windows (DirectX 12 & Vulkan) and potentially other platforms supporting Vulkan.

**Note**: This library is a work-in-progress and functionalities are added as needed by dependent projects (such as [Lysa Engine](https://github.com/LysaEngine)).

## Getting Started

### Prerequisites

- **C++23 Compatible Compiler** (MSVC 19.38+, Clang 16+, GCC 13+)
- **CMake 3.29+**
- **Vulkan SDK** (1.3+)

### Building

Vireo RHI is intended to be used as a submodule or a CMake subdirectory.

```bash
git clone https://github.com/HenriMichelon/vireo_rhi.git
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/vireo_rhi)
target_link_libraries(your_target PUBLIC vireo_rhi)
```

### Configuration Options

- `DIRECTX_BACKEND`: Enable/Disable DirectX 12 support (Default: `ON` on Windows).
- `LUA_BINDING`: Enable/Disable Lua bindings (Default: `OFF`).

## Documentation

- **User Documentation**: [https://henrimichelon.github.io/Vireo](https://henrimichelon.github.io/Vireo)
- **Hello Triangle Tutorial**: [https://henrimichelon.github.io/Vireo/gs_020_tutorial_1.html](https://henrimichelon.github.io/Vireo/gs_020_tutorial_1.html)

## Samples

Check out the [Vireo Samples](https://github.com/HenriMichelon/vireo_samples) repository for practical examples.

## License

This project is licensed under the **MIT License** - see the [LICENSE.txt](LICENSE.txt) file for details.