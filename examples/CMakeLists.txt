add_executable(
    simple_undirected_graph_construction
    simple_undirected_graph_construction.cpp
)

target_link_libraries(
    simple_undirected_graph_construction
    PRIVATE
    simple-undirected-graph
)