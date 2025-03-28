cmake_minimum_required(VERSION 3.29)

project(dx12)
set(MY_TARGET ${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()
include(cmake/compile_options.cmake)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

#######################################################
set(PROJECT_FILES
        ${SRC_DIR}/app.cppm
)
add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
)
compile_options(${MY_TARGET})
target_include_directories(${MY_TARGET} PUBLIC ${INCLUDE_DIR})

#######################################################

target_sources(${MY_TARGET}
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        ${PROJECT_FILES}
)

include(FetchContent)
include(cmake/std.cmake)
include(cmake/libraries.cmake)
