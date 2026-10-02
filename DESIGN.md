# Design

This file gives a rough and simple overview of how the graph is built internally and how the different parts work together.

## Main idea

The graph uses IDs to refer to vertices and edges instead of exposing the stored objects directly. The actual vertices and edges live inside reusable slots. Separate ID lists are then used to iterate quickly over the currently existing vertices and edges, thus the graph does not have to scan empty storage every time.

## Structure

Dependency on the vertex side:

`VertexId` (public) / `EdgeId` (public) <- `Adjacency` (public) <- `Vertex` (private) <- `VertexSlot` (private)

Dependency on the edge side:

`VertexId` (public) <- `Edge` (private) <- `EdgeSlot` (private)

A `Vertex` stores its own data and a list of `Adjacency` entries. Each adjacency entry stores the neighbouring `VertexId` together with the connecting `EdgeId`. An `Edge` stores the IDs of its two endpoints and its edge data. The slot types then wrap the actual `Vertex` or `Edge`, therefore removed storage can later be reused instead of continuously growing the storage vectors.

## IDs and slot reuse

A `VertexId` or `EdgeId` contains a graph ID, slot index and generation. The slot index tells the graph where the element is stored. The generation changes when a removed slot is reused, therefore an old ID does not accidentally become valid again just because the same slot index is used for a new element. The graph ID additionally prevents IDs from one graph instance from being used with another graph.

## Adding and removing

When adding a vertex or edge, the graph first checks whether a free slot already exists. If one exists, that slot is reused. Otherwise a new slot is appended. When removing an element, its stored object is destroyed, the slot becomes free, and the slot index is stored in the matching free-slot list so it can be reused later. The ID is also removed from the live-ID list. When removing a vertex, all edges connected to it are removed first so no adjacency entry or edge can keep referring to a removed vertex.

## Live IDs

The slot lists can contain empty entries after removals, so the graph also keeps `liveVertices_` and `liveEdges_`. These lists contain only IDs of elements that currently exist. Each occupied slot remembers its position inside the matching live-ID list through `liveIndex`. Thus an ID can be removed from that list using swap-and-pop instead of shifting every later element.

## Iteration

`existingVertices()` and `existingEdges()` return read-only views over the live-ID lists. This means iteration only visits elements that actually exist, while the slot lists remain responsible for storage and reuse, thus enabling fast iteration over the existing vertices and edges.

## Copy and move

When a graph is copied, the new graph gets its own graph ID. The copied internal IDs are then updated to use that new graph ID, therefore the copy becomes independent from the original graph. When a graph is moved, its contents and existing graph ID move together. Clearing a graph also gives it a new graph ID, therefore all IDs created before the clear become invalid.