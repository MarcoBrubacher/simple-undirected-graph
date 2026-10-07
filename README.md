# simple-undirected-graph

A small header-only C++20 library for finite simple undirected graphs.

## Graph model

The library represents finite simple undirected graphs:

- no self-loops
- no parallel edges
- each undirected edge is stored once

## Requirements

- C++20
- CMake 3.21 or newer when building the repository

## Goal

The goal is to provide a small and reusable graph container without depending on a large graph framework.

### TODOs

- Test suite
- Refine/polish the API and internal implementation based on findings from tests and examples.
- Add benchmarks and investigate further performance improvements.

### Further extensions

- Add a graph I/O for loading graph data from external files.
- Support multiple commonly used graph-data formats (only allow cleaned/formatted data).
- a read-only/frozen graph representation optimized for traversal.
