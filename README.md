# simple-undirected-graph

A small C++20 library for finite simple undirected graphs, with optional data attached to vertices, edges, and the graph itself.

## Graph representations


### DynamicGraph

DynamicGraph stores a mutable graph topology. Vertices and edges can be added or removed after construction, while graph, vertex, and edge data remain mutable. It is designed for efficient graph construction and modification.


### CSRGraph

CSRGraph stores a fixed topology in CSR format. Its topology cannot be modified after construction, but graph, vertex, and edge data remain mutable. Compared with DynamicGraph, it is designed for more compact storage and more efficient graph traversal.

In addition to the standard CSR arrays, it stores an edge ID for each adjacency entry, linking each vertex-neighbor connection to its corresponding edge. Both adjacency entries of an undirected edge reference the same edge ID, allowing edge properties to be stored only once and accessed directly during traversal.

Edge endpoints are not stored permanently to minimize memory usage. Instead, the `reconstructEdgeEndpoints()` function reconstructs the endpoints of each edge, indexed by its edge ID, when needed for edge-based processing.


## Graph conversion

Conversion supports conversion between DynamicGraph and CSRGraph in both directions.

- `toCSRGraph()` converts a dynamic graph to CSR format.
- `toDynamicGraph()` converts a CSR graph to a dynamic graph.

Both functions support copy and move semantics. Conversions preserve graph properties.


## Example

```cpp
#include <simple_undirected_graph/dynamic_graph.hpp>

int main()
{
    simple_undirected_graph::DynamicGraph<> graph;

    auto u = graph.addVertex();
    auto v = graph.addVertex();

    graph.addEdge(u, v);
}
```

This example creates a simple undirected graph containing two vertices
connected by one edge.


## Using the library


### FetchContent

Include [simple-undirected-graph](https://github.com/MarcoBrubacher/simple-undirected-graph) directly from GitHub:

```cmake
include(FetchContent)

FetchContent_Declare(
    simple_undirected_graph
    GIT_REPOSITORY https://github.com/MarcoBrubacher/simple-undirected-graph.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(simple_undirected_graph)
```


### Installed package

The library can also be installed as a CMake package:

```bash
cmake -S . -B build -DSIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES=OFF -DSIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS=OFF
cmake --install build --prefix path/to/install
```

locate it using:

```cmake
find_package(simple_undirected_graph 1.0.0 CONFIG REQUIRED)
```

### Linking

```cmake
target_link_libraries(my_app PRIVATE
    simple_undirected_graph::simple_undirected_graph
)
```


## Building

```bash
cmake -S . -B build -DSIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES=ON -DSIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS=ON
cmake --build build --config Debug --parallel
```

## Testing

```bash
ctest --test-dir build -C Debug --output-on-failure
```

## Requirements

- C++20
- CMake 3.21


## Third-party dependencies

The library itself has no external dependencies.

The external/ directory contains third-party code used for development and testing. Catch2 3.16.0 is vendored there as the test framework so the test suite can be built without downloading additional dependencies.


## Planned extensions

- Implement graph I/O for loading graph data from external files, starting with TU Dataset
- Support additional commonly used graph-data formats
- Add benchmarks and investigate performance, including hash collision behavior
- Rendering graphs
