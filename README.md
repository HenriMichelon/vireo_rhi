# Vireo Rendering Hardware Interface

Vireo RHI is an open-source library written in modern C++ that implements a common abstraction layer over multiple 3D graphics APIs. It provides a low-level, high-performance interface designed to mimic modern graphics APIs like Vulkan and DirectX 12 while hiding their boilerplate and complexity.

Please refer to the [Documentation](https://henrimichelon.github.io/Vireo/) for detailed instructions on how to integrate and use the RHI.

## Key Features

- **Multi-API Support**: 
    - **Vulkan 1.3**
    - **DirectX 12**
- **Modern C++ Architecture**: Built with C++23 and utilizes C++ Modules for faster compilation and clean code.
- **Low-Level Abstraction**: Abstraction of core modern graphics concepts:
    - Pipelines (Graphic & Compute)
    - Resource Management (Buffers, Images, Samplers)
    - Command Recording (Command Lists, Allocators)
    - Synchronization (Fences, Semaphores, Barriers)
    - Descriptor Management (Layouts & Sets)
    - Presentation (Swap Chains)
- **Validation & Debugging**: Integrated support for graphics API validation and debug layers.
- **Lua Bindings**: Optional Lua 5.4+ bindings for high-level logic and rapid prototyping.
- **Cross-Platform**: Designed for Windows (DirectX 12 & Vulkan), Linux (Vulkan) and potentially other platforms supporting Vulkan.

**Note**: This library is a work-in-progress and functionalities are added as needed by dependent projects (such as [Lysa Engine](https://github.com/LysaEngine)).

## Documentation

### Official documentation
- **User Documentation**: [https://henrimichelon.github.io/Vireo](https://henrimichelon.github.io/Vireo)
- **Hello Triangle Tutorial**: [https://henrimichelon.github.io/Vireo/gs_020_tutorial_1.html](https://henrimichelon.github.io/Vireo/gs_020_tutorial_1.html)

### Samples

Check out the [Vireo Samples](https://github.com/HenriMichelon/vireo_samples) repository for practical examples.

### Articles

| Article                                        | Language |
|:-----------------------------------------------| :--- |
| What is Vireo RHI?                             | [🇫🇷 French](https://henrimichelon.github.io/Articles/fr/vireo_rhi.html) / [🇺🇸 English](https://henrimichelon.github.io/Articles/en/vireo_rhi.html) 
| Example of TAA implementation with Vireo RHI   | [🇫🇷 French](https://henrimichelon.github.io/Articles/fr/taa_vireo_rhi.html) / [🇺🇸 English](https://henrimichelon.github.io/Articles/en/taa_vireo_rhi.html) |

## License

This project is licensed under the **MIT License** - see the [LICENSE.txt](LICENSE.txt) file for details.
