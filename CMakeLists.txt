cmake_minimum_required(VERSION 3.21)

project(
    simple_undirected_graph
    VERSION 0.1.0
    LANGUAGES CXX
)

add_library(simple_undirected_graph INTERFACE)

add_library(
    simple_undirected_graph::simple_undirected_graph
    ALIAS
    simple_undirected_graph
)

target_compile_features(
    simple_undirected_graph
    INTERFACE
    cxx_std_20
)

target_include_directories(
    simple_undirected_graph
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

option(
    SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES
    "Build simple-undirected-graph examples"
    ${PROJECT_IS_TOP_LEVEL}
)

option(
    SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS
    "Build simple-undirected-graph tests"
    ${PROJECT_IS_TOP_LEVEL}
)

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()