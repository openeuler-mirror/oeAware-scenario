cmake_minimum_required(VERSION 3.15)

project(scenario-example)

add_compile_options(-g -fPIC -Wall -Wextra)
add_subdirectory(scenario)
add_subdirectory(adapt)
