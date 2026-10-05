# Design

This file gives a rough and simple overview of how the graph is built internally and how the different parts work together.

## Main idea

The graph uses IDs to refer to vertices and edges instead of exposing the stored objects directly. The actual vertices and edges live inside reusable slots. Separate ID lists are then used to iterate quickly over the currently existing vertices and edges, thus the graph does not have to scan empty storage every time.

## Structure

Dependency on the vertex side:

`VertexId` (public) / `EdgeId` (public) <- `Adjacency` (public) <- `Vertex` (private) <- `VertexSlot` (private)

Dependency on the edge side:

`VertexId` (public) <- `Edge` (private) <- `EdgeSlot` (private)

A `Vertex` owns its vertex data together with its adjacency list. Each `Adjacency` entry stores the neighbouring `VertexId` and the `EdgeId` connecting both vertices, thus the adjacency lists represent the topology of the graph.

An `Edge` keeps both endpoint IDs and its own edge data. It also stores `adjacencyIndexU` and `adjacencyIndexV`, which are the positions of this edge inside the adjacency lists of its two endpoints. Therefore a known edge can later be removed without first searching either adjacency list.

Vertices and edges themselves are stored inside `VertexSlot` and `EdgeSlot`. The actual object inside a slot is wrapped in `std::optional`, thus removing an element can leave its slot empty so that the same storage may later be reused.

All slots are kept in `vertexSlots_` and `edgeSlots_`. Since these vectors may contain empty slots, the graph additionally keeps `liveVertices_` and `liveEdges_`, which only contain IDs of currently existing elements and are used for iteration.

## IDs and slot reuse

A `VertexId` or `EdgeId` internally contains a graph ID, slot index and generation. These values are private and IDs are used as opaque handles by callers. The slot index tells the graph where the corresponding element is stored, while the generation changes whenever a removed slot is reused. Therefore an old ID cannot accidentally become valid again just because its old slot is occupied by a new element. The graph ID additionally prevents IDs from one graph instance from being used with another graph.

Each occupied slot also stores a `liveIndex`, which points to its ID inside `liveVertices_` or `liveEdges_`. This allows the graph to remove IDs from these lists using swap-and-pop instead of shifting every following element.

`VertexIdHash` and `EdgeIdHash` provide hash functions for the opaque ID types, thus vertex and edge IDs can be used as keys in user-owned unordered containers without exposing their private components.

## Adding and removing

When adding a vertex or edge, the graph first checks whether a free slot already exists. If one exists, that slot is reused. Otherwise a new slot is appended. When removing an element, its stored object is destroyed, the slot becomes free, and the slot index is stored in the matching free-slot list so it can be reused later. The ID is also removed from the live-ID list. When removing a vertex, all edges connected to it are removed first so no adjacency entry or edge can keep referring to a removed vertex.

When an edge is added, one adjacency entry is appended to each endpoint. The edge stores the positions of these entries in `adjacencyIndexU` and `adjacencyIndexV`, and its normalized endpoint pair is inserted into `edgeLookup_`. When an edge is removed, its adjacency entries can therefore be accessed directly. If swap-and-pop moves another adjacency entry into the removed position, the corresponding moved edge has its stored adjacency index updated. The removed edge's endpoint pair is also removed from `edgeLookup_`.

The `addVertex()` overloads and `emplaceVertex()` use one shared private insertion function. Vertex data is constructed directly inside the selected vertex slot.

The `addEdge()` overloads and `emplaceEdge()` likewise use one shared private insertion function. Edge data is only constructed after the endpoints have been validated and the graph has confirmed that the edge does not already exist, and it is constructed directly inside the selected edge slot.

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