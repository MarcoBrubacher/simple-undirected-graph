set(SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES
    csr_graph/01_graph_creation.cpp
    csr_graph/02_graph_traversal.cpp
    csr_graph/03_graph_properties.cpp

    dynamic_graph/01_graph_creation.cpp
    dynamic_graph/02_vertex_operations.cpp
    dynamic_graph/03_edge_operations.cpp
    dynamic_graph/04_graph_properties.cpp
    dynamic_graph/05_graph_copy_move.cpp
    dynamic_graph/06_iteration.cpp
    dynamic_graph/07_id_hashing.cpp

    graph_conversion/01_dynamic_to_csr.cpp
    graph_conversion/02_csr_to_dynamic.cpp
)

foreach(EXAMPLE_SOURCE IN LISTS SIMPLE_UNDIRECTED_GRAPH_EXAMPLE_SOURCES)
    get_filename_component(EXAMPLE_NAME "${EXAMPLE_SOURCE}" NAME_WE)
    get_filename_component(EXAMPLE_DIR "${EXAMPLE_SOURCE}" DIRECTORY)

    string(REGEX REPLACE "^[0-9]+_" "" EXAMPLE_NAME "${EXAMPLE_NAME}")

    set(EXAMPLE_TARGET "simple_undirected_graph_${EXAMPLE_DIR}_${EXAMPLE_NAME}")

    add_executable(
        ${EXAMPLE_TARGET}
        ${EXAMPLE_SOURCE}
    )

    target_link_libraries(
        ${EXAMPLE_TARGET}
        PRIVATE
        simple_undirected_graph::simple_undirected_graph
    )
endforeach()