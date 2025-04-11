#
# Copyright (c) 2024-2025 Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
###### Detect Vulkan SDK
message(NOTICE "Searching for Vulkan SDK")
find_package(Vulkan REQUIRED)
target_include_directories(${MY_TARGET} PUBLIC ${Vulkan_INCLUDE_DIRS})

message(NOTICE "Fetching DirectX-Headers from https://github.com/microsoft/DirectX-Headers.git...")
FetchContent_Declare(
        DirectX-Headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG main
)
FetchContent_MakeAvailable(DirectX-Headers)
set(DirectXH_DIR ${CMAKE_BINARY_DIR}/_deps/directx-headers-src)
target_include_directories(${MY_TARGET} PUBLIC ${DirectXH_DIR}/include/directx)
target_link_libraries(${MY_TARGET} DirectX-Headers)

###### Using GLM for maths
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(GLM_GTC_constants)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
message(NOTICE "Fetching GLM from https://github.com/g-truc/glm ...")
FetchContent_Declare(
        fetch_glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(fetch_glm)
set(GLM_DIR ${CMAKE_BINARY_DIR}/_deps/fetch_glm-src/glm)
# compile GLM as a module
message(NOTICE "Building glm C++ module...")
add_library(glm-modules STATIC)
target_sources(glm-modules
  PUBLIC
    FILE_SET moduleStd
    TYPE CXX_MODULES
    BASE_DIRS ${GLM_DIR}
    FILES
      ${GLM_DIR}/glm.cppm)
target_link_libraries(glm-modules glm::glm)
target_link_libraries(${MY_TARGET} glm::glm glm-modules)
if(MSVC)
    target_precompile_headers(${MY_TARGET} PRIVATE ${GLM_DIR}/glm.hpp ${GLM_DIR}/gtx/quaternion.hpp ${GLM_DIR}/gtx/matrix_decompose.hpp)
endif()



