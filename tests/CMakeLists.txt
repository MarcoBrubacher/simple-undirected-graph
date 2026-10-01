add_executable(
    simple_undirected_graph_tests
    simple_undirected_graph_tests.cpp
)

target_link_libraries(
    simple_undirected_graph_tests
    PRIVATE
    simple-undirected-graph
)

add_test(
    NAME simple_undirected_graph_tests
    COMMAND simple_undirected_graph_tests
)