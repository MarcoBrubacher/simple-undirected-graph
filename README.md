# simple-undirected-graph

A small header-only C++20 library for finite simple undirected graphs, with optional data attached to vertices, edges, and the graph itself.


## Graph representations

### DynamicGraph

`DynamicGraph` stores a mutable graph topology. Vertices and edges can be added or removed after construction, while graph, vertex, and edge data remain mutable. 

### CSRGraph

`CSRGraph` stores a fixed topology in CSR format. Its topology cannot be modified after construction, but graph, vertex, and edge data remain mutable.

## Requirements

- C++20
- CMake 3.21

## Third-party dependencies

The library itself has no external dependencies.

The `external/` directory contains third-party code used for development and testing. Catch2 3.16.0 is vendored there as the test framework so the test suite can be built without downloading additional dependencies.

## Building

Configure and build the library's examples and tests:

```bash
cmake -S . -B build -DSIMPLE_UNDIRECTED_GRAPH_BUILD_EXAMPLES=ON -DSIMPLE_UNDIRECTED_GRAPH_BUILD_TESTS=ON
cmake --build build --config Debug --parallel
```

## Testing

Run all registered tests:

```bash
ctest --test-dir build -C Debug --output-on-failure
```

## Current work

- CSR graph tests
- Implementing conversions between `DynamicGraph` and `CSRGraph`
- Implement graph I/O for loading graph data from external files, starting with TU Dataset

## Planned extensions

- Support additional commonly used graph-data formats
- Add benchmarks and investigate performance, including hash collision behavior
- Rendering graphs?