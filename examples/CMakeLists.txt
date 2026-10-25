set(SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES
    dynamic_graph/01_graph_creation.cpp
    dynamic_graph/02_vertex_operations.cpp
    dynamic_graph/03_edge_operations.cpp
    dynamic_graph/04_graph_properties.cpp
    dynamic_graph/05_graph_copy_move.cpp
    dynamic_graph/06_iteration.cpp
    dynamic_graph/07_id_hashing.cpp
)

foreach(EXAMPLE_SOURCE IN LISTS SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES)
    get_filename_component(EXAMPLE_NAME "${EXAMPLE_SOURCE}" NAME_WE)

    string(REGEX REPLACE "^[0-9]+_" "" EXAMPLE_NAME "${EXAMPLE_NAME}")

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