cmake_minimum_required(VERSION 3.10)

set(project_test
    ${CMAKE_SOURCE_DIR}/test/main.cc
    ${CMAKE_SOURCE_DIR}/test/test_vec.cc
    ${CMAKE_SOURCE_DIR}/test/test_matrix.cc
    ${CMAKE_SOURCE_DIR}/test/test_transform.cc
    ${CMAKE_SOURCE_DIR}/test/test_math.cc
    ${CMAKE_SOURCE_DIR}/test/test_ray.cc
    ${CMAKE_SOURCE_DIR}/test/test_canvas.cc
)

add_executable(raytracer_test 
	${project_test} 
    ${project_headers}
    ${project_sources}
)

target_include_directories(raytracer_test
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(raytracer_test
    gtest
    gtest_main
    gmock
    pthread
)
