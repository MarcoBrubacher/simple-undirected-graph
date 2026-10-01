cmake_minimum_required(VERSION 3.21)

project(
    simple-undirected-graph
    VERSION 0.1.0
    LANGUAGES CXX
)

add_library(simple-undirected-graph INTERFACE)

target_compile_features(
    simple-undirected-graph
    INTERFACE
    cxx_std_20
)

target_include_directories(
    simple-undirected-graph
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

option(SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES "Build examples" ON)
option(SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS "Build tests" ON)

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()