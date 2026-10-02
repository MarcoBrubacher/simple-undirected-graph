add_executable(
    simple_undirected_graph_construction
    graph_creation.cpp
)

target_link_libraries(
    simple_undirected_graph_construction
    PRIVATE
    simple_undirected_graph::simple_undirected_graph
)