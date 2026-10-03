# simple-undirected-graph

A small header-only C++20 library for finite simple undirected graphs.

It provides typed vertex, edge, and graph properties, stable generation-safe IDs, reusable storage, adjacency queries, and efficient edge lookup.

## Features

- typed vertex, edge, and graph properties
- propertyless graphs through `NoProperties`
- strong `VertexId` and `EdgeId` handles
- generation-safe stale-ID detection
- graph-specific IDs
- safe vertex and edge removal
- reusable storage slots
- adjacency queries
- average O(1) edge lookup
- live vertex and edge counts
- iteration over existing vertices and edges
- const-correct property access
- copy and move semantics

## Graph model

The library represents finite simple undirected graphs:

- no self-loops
- no parallel edges
- each undirected edge is stored once


## Requirements

- C++20
- CMake 3.21 or newer when building the repository

## Design

The graph uses reusable storage slots, generation-safe IDs, compact live-ID lists, adjacency-position bookkeeping, and an endpoint-pair hash table.

For a detailed description of the internal representation, see [DESIGN.md](DESIGN.md).

## Goal

The goal is to provide a small and reusable graph container without depending on a large graph framework.

## Roadmap

The library is still under active development. The current focus is on finishing and validating the core graph implementation before adding larger extensions.

### Planned

- Complete the full test suite, including edge cases, stale IDs, copy/move behavior, exception safety, and randomized mutation tests.
- Add complete examples covering the public API and typical usage.
- Refine and polish the API and internal implementation based on findings from tests and examples.
- Add benchmarks and investigate further performance improvements.

### Further extensions

- Add a graph reader/importer for loading graph data from external files.
- Support multiple commonly used graph-data formats.
- Keep the importer strict and optimized for well-formed input rather than attempting to recover from malformed datasets.
- Keep file parsing and importing separate from the core graph container.
- Investigate a read-only/frozen graph representation optimized for traversal.