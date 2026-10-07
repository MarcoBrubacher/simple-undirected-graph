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
)

target_include_directories(
    simple_undirected_graph_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME simple_undirected_graph_tests
    COMMAND simple_undirected_graph_tests
)