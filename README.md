# simple-undirected-graph

A small header-only C++20 library for finite simple undirected graphs, with optional data attached to vertices, edges, and the graph itself.

## Graph model

The library represents finite simple undirected graphs:

- vertices cannot have self-loops
- there can be at most one edge between any pair of vertices
- each undirected edge is represented once

## Access and traversal

Vertices and edges are accessed through IDs rather than references into the internal storage. The API supports:

- iteration over existing vertices and edges
- adjacency access
- vertex, edge, and graph data
- edge lookup by endpoints

## Requirements

- C++20
- CMake 3.21

## Third-party dependencies

The library itself has no external dependencies.

The `external/` directory contains third-party code used for development and testing. Catch2 3.16.0 is vendored there as the test framework so the test suite can be built without downloading additional dependencies.

## Goal

The goal is to provide a small and reusable graph container without depending on a large graph framework.

The core graph API and implementation are mostly complete. The current focus is testing existing behavior and improving the API and implementation where problems are found.

## Current work

- Writing the test suite
- Fix bugs and API issues found during testing
- Add benchmarks and investigate performance, including hash collision behavior

## Planned extensions

- Add graph I/O for loading graph data from external files
- Support commonly used graph-data formats, with TUDataset as the first target
- Add a read-only/frozen graph representation optimized for traversal