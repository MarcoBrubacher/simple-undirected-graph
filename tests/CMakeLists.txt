add_subdirectory(
    ${PROJECT_SOURCE_DIR}/external/Catch2
    ${PROJECT_BINARY_DIR}/external/Catch2
)

set(SIMPLE_UNDIRECTED_GRAPH_TEST_SOURCES
    core/test_graph.cpp
    core/test_compile_time.cpp
)

add_executable(
    simple_undirected_graph_tests
    ${SIMPLE_UNDIRECTED_GRAPH_TEST_SOURCES}
)

target_link_libraries(
    simple_undirected_graph_tests
    PRIVATE
        simple_undirected_graph::simple_undirected_graph
        Catch2::Catch2WithMain
)

target_include_directories(
    simple_undirected_graph_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/external/Catch2/extras
)

include(Catch)

catch_discover_tests(simple_undirected_graph_tests)