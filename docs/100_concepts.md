# Vireo concepts

## The Vireo class
The [`Vireo`](classvireo_1_1Vireo.html) class encapsulate the creation of the various objects needed by the graphics API such as
the physical adapter and the logical device.

The starting point for using Vireo is to import the `vireo` modules then
create an instance of this class by calling the 
[`create()`](classvireo_1_1Vireo.html#a182f874336d2992e9cd06e11661e2ddb) static method.
This method takes the [`Backend`](namespacevireo.html#ae046059fd4567480e8f03be6171dafd1)
you want to use, DirectX or Vulkan :

```
import vireo;

...

auto vireo = vireo::Vireo::create(vireo::Backend::VULKAN);

```

This instance needs to be accessible everywhere you want to use Vireo since this class
is used to create all Vireo objects.

When creating the Vireo object, the instance, physical device and logical devices objects are created.

## Backend instances, physical devices and logical devices
The [`Instance`](classvireo_1_1Instance.html), [`PhysicalDevice`](classvireo_1_1PhysicalDevice.html)
and [`Device`](classvireo_1_1Device.html) are opaque classes that encapsulate the
corresponding backends concepts :
- Vulkan : `VkInstance`, `VkPhysicalDevice` and `VkDevice`
- DirectX : `IDXGIFactory4`, `IDXGIAdapter4` and `ID3D12Device`

These classes are responsible for the backend initialization and for selecting the
best GPU available on the host. If no supported GPU is detected an
[`Exception`](classvireo_1_1Exception.html) is raised.