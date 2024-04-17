set(scenario_adapt_srcs
    adapt.cpp
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/oeAwarelib)

add_library(scenario_example SHARED ${scenario_adapt_srcs})

message(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(scenario_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/dep_plugin/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/for_tune/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/frame/
    ${CMAKE_CURRENT_SOURCE_DIR}/../scenario/
)

target_link_libraries(scenario_example scenario_base)
