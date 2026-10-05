set(SIMPLE_UNDIRECTED_GRAPH_TEST_SOURCES
    core/test_construction.cpp
    core/test_vertices.cpp
    core/test_edges.cpp
    core/test_adjacency.cpp
    core/test_edge_lookup.cpp
    core/test_removal.cpp
    core/test_slot_reuse.cpp
    core/test_invalid_ids.cpp
    core/test_iteration.cpp
    core/test_clear.cpp
    core/test_copy.cpp
    core/test_move.cpp
    core/test_properties.cpp
    core/test_invariants.cpp
    core/test_exception_safety.cpp
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

add_test(
    NAME simple_undirected_graph_tests
    COMMAND simple_undirected_graph_tests
)