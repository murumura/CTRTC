cmake_minimum_required(VERSION 3.16)
# Enable C++20
set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

######### Options ###########################
option(BUILD_TESTS "Build unit tests" OFF)
option(RENDER_STATIC "Perform render at compile-time" OFF)
set(RENDER_CHAPTER "" CACHE STRING "Chapter to render")
#############################################
if (RENDER_STATIC)
    list(APPEND COMPILE_DEFINITIONS "-DCOMPILETIME")
endif()
# Define project name
project(RayTracerTop)
list(APPEND COMPILE_DEFINITIONS "-std=c++2a")


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND COMPILE_DEFINITIONS "-fconstexpr-steps=4247483647")
    list(APPEND COMPILE_DEFINITIONS "-O3")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND COMPILE_DEFINITIONS "-fconstexpr-ops-limit=9147483647")
endif()

list(APPEND COMPILE_DEFINITIONS "-fconstexpr-depth=999999")
include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/shapes"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/primitives"
    "${CMAKE_SOURCE_DIR}/include/utils"
    "${CMAKE_SOURCE_DIR}/include/shapes"
    "${CMAKE_SOURCE_DIR}/test"
    "${CMAKE_SOURCE_DIR}/scene"
)

set(project_headers
    ${CMAKE_SOURCE_DIR}/include/primitives/vec.hh
    ${CMAKE_SOURCE_DIR}/include/primitives/matrix.hh
    ${CMAKE_SOURCE_DIR}/include/primitives/static_base.hh
    ${CMAKE_SOURCE_DIR}/include/primitives/static_vector.hh
    ${CMAKE_SOURCE_DIR}/include/primitives/primitive_traits.hh
    ${CMAKE_SOURCE_DIR}/include/utils/math.hh
    ${CMAKE_SOURCE_DIR}/include/transform.hh
    ${CMAKE_SOURCE_DIR}/include/canvas.hh
    ${CMAKE_SOURCE_DIR}/include/world.hh
    ${CMAKE_SOURCE_DIR}/include/ray.hh
    ${CMAKE_SOURCE_DIR}/include/camera.hh
    ${CMAKE_SOURCE_DIR}/include/primitives.hh
)
if (BUILD_TESTS)
    add_subdirectory(test)
endif()

if (NOT (RENDER_CHAPTER STREQUAL ""))
    message(STATUS "Render chapter ${RENDER_CHAPTER}")
    message(STATUS "Render at compile time: ${RENDER_STATIC}")
    add_subdirectory(scene)
endif()
