cmake_minimum_required(VERSION 3.15)

project(analysis)
set(OUTPUT_LIBRARY_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
message("-- libkperf library path: ${LIB_KPERF_LIBPATH}")
message("-- libkperf include path: ${LIB_KPERF_INCPATH}")
set(analysis_src
    analysis/analysis.cpp
    analysis/common.cpp
    analysis/env.cpp
)

set(oeaware_src adapt/oeaware_adapt.cpp)

add_compile_options(-O2 -fPIC -Wall -Wextra)

add_library(analysis_base OBJECT ${analysis_src})
target_include_directories(analysis_base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${LIB_KPERF_INCPATH}
)


target_link_libraries(analysis_base numa boundscheck)

add_library(analysis_oeaware SHARED ${oeaware_src})
target_include_directories(analysis_oeaware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(analysis_oeaware analysis_base)
set_target_properties(analysis_oeaware PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBRARY_DIRECTORY})


