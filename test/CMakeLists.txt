cmake_minimum_required(VERSION 3.10)

set(project_test
    ${CMAKE_SOURCE_DIR}/test/main.cc
    ${CMAKE_SOURCE_DIR}/test/Test_vec.cc
    ${CMAKE_SOURCE_DIR}/test/Test_matrix.cc
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
