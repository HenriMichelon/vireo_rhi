# Vireo Rendering Hardware Interface

Vireo RHI is an open source library written in modern C++ that implements a common abstraction layer over multiple 3D graphics APIs.

Currently implemented backends are :
- Vulkan 1.3
- DirectX 12

Key features :
- A low-level abstraction layer mimicking low-level graphics APIs such as Vulkan & DirectX 12
- Abstraction of modern graphics API concepts such as pipelines, barriers, descriptors, submit queues, commands list, ...
- Graphic and compute pipelines support
- Support of graphics API validation/debug layers for easy debugging

**Note** this library is not intended to be a full-featured abstraction of the graphics API.
Functionalities are added when needed.

[User documentation](https://henrimichelon.github.io/Vireo/)

["Hello triangle" tutorial](https://henrimichelon.github.io/Vireo/md_001_tutorial_triangle.html)

[Samples repository](https://github.com/HenriMichelon/vireo_samples)