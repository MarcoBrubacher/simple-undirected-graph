set(SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES
    graph_creation.cpp
    graph_copy_move.cpp
    vertex_operations.cpp
    edge_operations.cpp
    graph_properties.cpp
    iteration.cpp
    id_hashing.cpp
)

foreach(EXAMPLE_SOURCE IN LISTS SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES)
    get_filename_component(EXAMPLE_NAME "${EXAMPLE_SOURCE}" NAME_WE)
    string(REGEX REPLACE "^graph_" "" EXAMPLE_NAME "${EXAMPLE_NAME}")

    add_executable(
        simple_undirected_graph_${EXAMPLE_NAME}
        ${EXAMPLE_SOURCE}
    )

    target_link_libraries(
        simple_undirected_graph_${EXAMPLE_NAME}
        PRIVATE
        simple_undirected_graph::simple_undirected_graph
    )
endforeach()