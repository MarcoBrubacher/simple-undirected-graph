# simple-undirected-graph

C++20 library for simple undirected graphs with typed properties, stable IDs, safe removal, and reusable storage.

## Scope

This library provides a small, focused container for finite simple undirected graphs.

It should support in the end:
- typed vertex properties
- typed edge properties
- typed graph-level properties
- propertyless graphs through `NoProperties`
- strong vertex and edge IDs
- generation-safe IDs for detecting stale handles
- safe vertex and edge removal
- reusable storage slots
- adjacency queries
- vertex and edge lookup
- live vertex and edge counts
- iteration over existing vertices and edges
- const-correct editable and read-only property access

The graph model is intentionally limited to simple undirected graphs:
- no self-loops
- no parallel edges
- each undirected edge is stored once

The library does not provide graph algorithms or other graph procedure, or even graph visualisations. Those are intended to be implemented separately using the graph API.

The goal is to provide a small, predictable, reusable graph container that is easy to use in algorithmic, educational, visualization, and procedural-generation projects without depending on a large graph framework.