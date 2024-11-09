cmake_minimum_required(VERSION 3.10)
project(test)

add_compile_options(-O2 -fPIC -Wall -Wextra -g)
set(analysis_src
    ${CMAKE_CURRENT_SOURCE_DIR}/../analysis/analysis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../analysis/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../analysis/env.cpp
)

add_executable(test_base common_test.cpp  env_test.cpp ${analysis_src})

target_include_directories(test_base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../analysis/
    ${LIB_KPERF_INCPATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(test_base gtest gtest_main numa boundscheck)