cmake_minimum_required(VERSION 3.16)
# Enable C++20
set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Define project name
project(RayTracerTop)
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
    ${CMAKE_SOURCE_DIR}/include/utils/math.hh
    ${CMAKE_SOURCE_DIR}/include/transform.hh
    ${CMAKE_SOURCE_DIR}/include/canvas.hh
    ${CMAKE_SOURCE_DIR}/include/ray.hh
    ${CMAKE_SOURCE_DIR}/include/camera.hh
    ${CMAKE_SOURCE_DIR}/include/shape.hh
    ${CMAKE_SOURCE_DIR}/include/shading.hh
)

set(project_sources
)

find_package(PkgConfig REQUIRED)
add_subdirectory(test)
add_subdirectory(scene)
