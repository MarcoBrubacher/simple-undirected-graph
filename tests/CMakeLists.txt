set(SIMPLE_UNDIRECTED_GRAPH_TEST_SOURCES
    test_construction.cpp
    test_vertices.cpp
    test_edges.cpp
    test_adjacency.cpp
    test_edge_lookup.cpp
    test_removal.cpp
    test_slot_reuse.cpp
    test_invalid_ids.cpp
    test_iteration.cpp
    test_clear.cpp
    test_copy.cpp
    test_move.cpp
    test_properties.cpp
    test_invariants.cpp
    
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