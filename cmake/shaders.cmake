##### compile HLSL sources files into DXIL & SPIR-V
get_filename_component(VULKAN_DIR "${Vulkan_INCLUDE_DIR}" DIRECTORY)
set(DXC_EXECUTABLE "${VULKAN_DIR}/bin/dxc.exe")
#find_program(DXC_EXECUTABLE NAMES dxc PATHS "${VULKAN_DIR}/bin" NO_DEFAULT_PATH)
#if(NOT DXC_EXECUTABLE)
#    message(FATAL_ERROR "DXC not found!")
#endif()

function(add_shaders TARGET_NAME BUILD_DIR SHADER_INCLUDE_DIR)
    set(SHADER_SOURCE_FILES ${ARGN}) # the rest of arguments to this function will be assigned as shader source files
    set(SHADER_BINARIES ${BUILD_DIR})

    # Validate that source files have been passed
    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if(FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot create a shaders target without any source files")
    endif()

    set(SHADER_COMMANDS)
    set(SHADER_PRODUCTS)
    file(MAKE_DIRECTORY ${SHADER_BINARIES})

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE STEM SHADER_NAME)
        # Vertex shader to DXIL
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS "${DXC_EXECUTABLE}")
        list(APPEND SHADER_COMMANDS "-T")
        list(APPEND SHADER_COMMANDS "vs_5_0")
        list(APPEND SHADER_COMMANDS "-E")
        list(APPEND SHADER_COMMANDS "VSMain")
        list(APPEND SHADER_COMMANDS "-I")
        list(APPEND SHADER_COMMANDS "${SHADER_INCLUDE_DIR}")
        list(APPEND SHADER_COMMANDS "-Fo")
        list(APPEND SHADER_COMMANDS "${SHADER_BINARIES}/${SHADER_NAME}_vert.dxil")
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        # Vertex shader to SPIR-V
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS "${DXC_EXECUTABLE}")
        list(APPEND SHADER_COMMANDS "-spirv")
        list(APPEND SHADER_COMMANDS "-T")
        list(APPEND SHADER_COMMANDS "vs_5_0")
        list(APPEND SHADER_COMMANDS "-E")
        list(APPEND SHADER_COMMANDS "VSMain")
        list(APPEND SHADER_COMMANDS "-I")
        list(APPEND SHADER_COMMANDS "${SHADER_INCLUDE_DIR}")
        list(APPEND SHADER_COMMANDS "-Fo")
        list(APPEND SHADER_COMMANDS "${SHADER_BINARIES}/${SHADER_NAME}_vert.spv")
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        # Vertex shader to DXIL
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS "${DXC_EXECUTABLE}")
        list(APPEND SHADER_COMMANDS "-T")
        list(APPEND SHADER_COMMANDS "ps_5_0")
        list(APPEND SHADER_COMMANDS "-E")
        list(APPEND SHADER_COMMANDS "PSMain")
        list(APPEND SHADER_COMMANDS "-I")
        list(APPEND SHADER_COMMANDS "${SHADER_INCLUDE_DIR}")
        list(APPEND SHADER_COMMANDS "-Fo")
        list(APPEND SHADER_COMMANDS "${SHADER_BINARIES}/${SHADER_NAME}_frag.dxil")
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        # Vertex shader to SPIR-V
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS "${DXC_EXECUTABLE}")
        list(APPEND SHADER_COMMANDS "-spirv")
        list(APPEND SHADER_COMMANDS "-T")
        list(APPEND SHADER_COMMANDS "ps_5_0")
        list(APPEND SHADER_COMMANDS "-E")
        list(APPEND SHADER_COMMANDS "PSMain")
        list(APPEND SHADER_COMMANDS "-I")
        list(APPEND SHADER_COMMANDS "${SHADER_INCLUDE_DIR}")
        list(APPEND SHADER_COMMANDS "-Fo")
        list(APPEND SHADER_COMMANDS "${SHADER_BINARIES}/${SHADER_NAME}_frag.spv")
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        # Add products
        list(APPEND SHADER_PRODUCTS "${SHADER_BINARIES}/${SHADER_NAME}_vert.dxil")
        list(APPEND SHADER_PRODUCTS "${SHADER_BINARIES}/${SHADER_NAME}_vert.spv")
        list(APPEND SHADER_PRODUCTS "${SHADER_BINARIES}/${SHADER_NAME}_frag.dxil")
        list(APPEND SHADER_PRODUCTS "${SHADER_BINARIES}/${SHADER_NAME}_frag.spv")
    endforeach()

    add_custom_target(${TARGET_NAME} ALL
            ${SHADER_COMMANDS}
            COMMENT "Compiling Shaders [${TARGET_NAME}]"
            SOURCES ${SHADER_SOURCE_FILES}
            BYPRODUCTS ${SHADER_PRODUCTS}
    )

endfunction()
