# Design

This file gives a rough and simple overview of how the graph is built internally and how the different parts work together.

## Main idea

The graph uses IDs to refer to vertices and edges instead of exposing the stored objects directly. The actual vertices and edges live inside reusable slots. Separate ID lists are then used to iterate quickly over the currently existing vertices and edges, thus the graph does not have to scan empty storage every time.

## Structure

Dependency on the vertex side:

`VertexId` (public) / `EdgeId` (public) <- `Adjacency` (public) <- `Vertex` (private) <- `VertexSlot` (private)

Dependency on the edge side:

`VertexId` (public) <- `Edge` (private) <- `EdgeSlot` (private)

A `Vertex` stores its own data and a list of `Adjacency` entries. Each adjacency entry stores the neighbouring `VertexId` together with the connecting `EdgeId`. An `Edge` stores the IDs of its two endpoints and its edge data. It additionally stores the positions of its two adjacency entries through `adjacencyIndexU` and `adjacencyIndexV`, therefore those entries can later be located directly when the edge is removed. The slot types then wrap the actual `Vertex` or `Edge`, therefore removed storage can later be reused instead of continuously growing the storage vectors.

The graph additionally keeps an `edgeLookup_` hash table. Its key is a normalized pair of endpoint slot indices and its value is the corresponding `EdgeId`. Normalizing the endpoint pair means that the smaller endpoint index is always stored first, therefore `(u, v)` and `(v, u)` produce the same key for an undirected edge.

### Complete nesting overview

```text
Graph<VertexData, EdgeData, GraphData>
|
|-- graphId_
|   `-- unique ID of this graph instance
|
|-- graphData_
|   `-- GraphData
|
|-- VertexIdHash
|   `-- hashes VertexId values for unordered containers
|
|-- EdgeIdHash
|   `-- hashes EdgeId values for unordered containers
|
|-- vertexSlots_
|   `-- std::vector<VertexSlot>
|       `-- VertexSlot
|           |-- vertex
|           |   `-- std::optional<Vertex>
|           |       `-- Vertex
|           |           |-- data
|           |           |   `-- VertexData
|           |           `-- adjacency
|           |               `-- std::vector<Adjacency>
|           |                   `-- Adjacency
|           |                       |-- neighbor
|           |                       |   `-- VertexId
|           |                       |       |-- graphId
|           |                       |       |-- index
|           |                       |       `-- generation
|           |                       `-- edge
|           |                           `-- EdgeId
|           |                               |-- graphId
|           |                               |-- index
|           |                               `-- generation
|           |-- generation
|           `-- liveIndex
|
|-- edgeSlots_
|   `-- std::vector<EdgeSlot>
|       `-- EdgeSlot
|           |-- edge
|           |   `-- std::optional<Edge>
|           |       `-- Edge
|           |           |-- endpointU
|           |           |   `-- VertexId
|           |           |-- endpointV
|           |           |   `-- VertexId
|           |           |-- data
|           |           |   `-- EdgeData
|           |           |-- adjacencyIndexU
|           |           `-- adjacencyIndexV
|           |-- generation
|           `-- liveIndex
|
|-- freeVertexSlots_
|   `-- std::vector<std::size_t>
|
|-- freeEdgeSlots_
|   `-- std::vector<std::size_t>
|
|-- liveVertices_
|   `-- std::vector<VertexId>
|
|-- liveEdges_
|   `-- std::vector<EdgeId>
|
`-- edgeLookup_
    `-- std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher>
        |-- EndpointPair
        |   `-- std::pair<std::size_t, std::size_t>
        `-- EndpointPairHasher
            `-- hashes the normalized endpoint pair
```

The essential relationships are:

```text
VertexId.index: points to a VertexSlot inside vertexSlots_

EdgeId.index: points to an EdgeSlot inside edgeSlots_

VertexSlot.liveIndex: points to this vertex's VertexId inside liveVertices_

EdgeSlot.liveIndex: points to this edge's EdgeId inside liveEdges_

Edge.adjacencyIndexU: points to this edge's adjacency entry inside endpointU's adjacency list

Edge.adjacencyIndexV: points to this edge's adjacency entry inside endpointV's adjacency list

edgeLookup_: maps a normalized pair of endpoint slot indices to the corresponding EdgeId
```

The topology of the graph is represented through the adjacency lists stored inside the vertices. Each `Adjacency` connects one vertex to a neighbouring `VertexId` and stores the `EdgeId` representing that connection. The corresponding `Edge` independently stores both endpoint IDs and the edge data. The edge also remembers where its two adjacency entries are located, therefore removing a known edge does not require searching through either endpoint's adjacency list.

The additional `edgeLookup_` provides a direct lookup from two endpoint vertices to their connecting edge. Since the graph is undirected, the two endpoint slot indices are normalized before being used as a key, therefore both endpoint orders refer to the same edge. `EndpointPairHasher` hashes this normalized pair so `edgeLookup_` can use `std::unordered_map`, giving `hasEdge()` and `findEdgeId()` average O(1) lookup.

## IDs and slot reuse

A `VertexId` or `EdgeId` internally contains a graph ID, slot index and generation. These values are private and IDs are used as opaque handles by callers. The slot index tells the graph where the element is stored. The generation changes when a removed slot is reused, therefore an old ID does not accidentally become valid again just because the same slot index is used for a new element. The graph ID additionally prevents IDs from one graph instance from being used with another graph.

`VertexIdHash` and `EdgeIdHash` provide hash functions for the opaque ID types, allowing vertex and edge IDs to be used as keys in user-owned unordered containers without exposing their private components.

## Adding and removing

When adding a vertex or edge, the graph first checks whether a free slot already exists. If one exists, that slot is reused. Otherwise a new slot is appended. When removing an element, its stored object is destroyed, the slot becomes free, and the slot index is stored in the matching free-slot list so it can be reused later. The ID is also removed from the live-ID list. When removing a vertex, all edges connected to it are removed first so no adjacency entry or edge can keep referring to a removed vertex.

When an edge is added, one adjacency entry is appended to each endpoint. The edge stores the positions of these entries in `adjacencyIndexU` and `adjacencyIndexV`, and its normalized endpoint pair is inserted into `edgeLookup_`. When an edge is removed, its adjacency entries can therefore be accessed directly. If swap-and-pop moves another adjacency entry into the removed position, the corresponding moved edge has its stored adjacency index updated. The removed edge's endpoint pair is also removed from `edgeLookup_`.

The `addEdge()` overloads use one shared private insertion function. Edge data is only copied or moved after the endpoints have been validated and the graph has confirmed that the edge does not already exist.

## Live IDs

The slot lists can contain empty entries after removals, so the graph also keeps `liveVertices_` and `liveEdges_`. These lists contain only IDs of elements that currently exist. Each occupied slot remembers its position inside the matching live-ID list through `liveIndex`. Thus an ID can be removed from that list using swap-and-pop instead of shifting every later element.

## Edge lookup

`edgeLookup_` is an `std::unordered_map` from an `EndpointPair` to an `EdgeId`. An `EndpointPair` contains the slot indices of the two endpoint vertices. The `normalizeEndpoints()` helper places the smaller index first before the pair is used. This is essential, since the graph is undirected and `(u, v)` must represent the same edge as `(v, u)`.

`EndpointPairHasher` converts an `EndpointPair` into a hash value used internally by the unordered map. The hash does not itself identify the edge; it only allows the map to locate the bucket in which the endpoint pair may be stored. The actual key remains the `EndpointPair` and the stored value remains the corresponding `EdgeId`.

The lookup table allows the graph to check whether two vertices are connected and retrieve the corresponding `EdgeId` in average O(1) time instead of searching through an adjacency list.

## Iteration

`vertices()` and `edges()` return read-only `std::span` views over the live-ID lists. This means iteration only visits elements that actually exist without copying the ID lists. The returned views remain valid until the graph topology is modified.

## Copy and move

When a graph is copied, the new graph gets its own graph ID. The copied internal IDs are then updated to use that new graph ID, therefore the copy becomes independent from the original graph. The `EdgeId` values stored inside `edgeLookup_` are updated as well, while the normalized endpoint slot pairs remain unchanged because the copied slot positions stay the same.

When a graph is moved, its contents and existing graph ID move together, including the edge lookup table. Existing IDs therefore continue to belong to the moved graph. The source graph is left empty and receives a new graph ID so it can be reused independently.

Clearing a graph also gives it a new graph ID, therefore all IDs created before the clear become invalid.