set(scenario_srcs
    scenario.cpp
)

include_directories(scenario_base PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/dep_plugin/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/for_tune/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/frame/
)
add_compile_options(-Wall -fPIC)
add_library(scenario_base OBJECT ${scenario_srcs})