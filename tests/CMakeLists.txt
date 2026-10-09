add_subdirectory(
    ${PROJECT_SOURCE_DIR}/external/Catch2
    ${PROJECT_BINARY_DIR}/external/Catch2
)

set(SIMPLE_UNDIRECTED_GRAPH_TEST_SOURCES
    core/test_graph.cpp
    core/test_vertices.cpp
    core/test_edges.cpp
    core/test_adjacency.cpp
    core/test_iteration.cpp
    core/test_copy_move.cpp
    core/test_ids.cpp
    core/test_api.cpp
    core/test_exception_safety.cpp
    core/test_consistency.cpp
    core/test_randomized.cpp
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