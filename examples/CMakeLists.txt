set(SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES
    core/01_graph_creation.cpp
    core/02_vertex_operations.cpp
    core/03_edge_operations.cpp
    core/04_graph_properties.cpp
    core/05_graph_copy_move.cpp
    core/06_iteration.cpp
    core/07_id_hashing.cpp
)

foreach(EXAMPLE_SOURCE IN LISTS SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES)
    get_filename_component(EXAMPLE_NAME "${EXAMPLE_SOURCE}" NAME_WE)

    # remove the numeric prefix, for example 01_graph_creation -> graph_creation
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