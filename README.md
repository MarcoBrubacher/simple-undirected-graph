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