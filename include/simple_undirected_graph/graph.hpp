#pragma once
#include <cstddef>
#include <vector>
#include <stdexcept>
#include <optional>

/**
 * empty property type used when no data is attached
 * Graph<> uses NoProperties for vertex, edge, and graph data
 */
struct NoProperties
{
};

/**
 * finite simple undirected graph with typed vertex, edge, and graph data, where
 * vertices and edges may carry user-defined data, while graph-level data is stored once for the whole graph
 *
 * vertex and edge IDs are generation-safe handles
 * removed storage may be reused without making stale IDs valid again
 * topology is modified only through the graph API
 *
 * @tparam VertexData type stored with each vertex
 * @tparam EdgeData type stored with each edge
 * @tparam GraphData type stored with the whole graph
 */
template <
    typename VertexData = NoProperties,
    typename EdgeData = NoProperties,
    typename GraphData = NoProperties>
class Graph
{
public:
    // handle and adjacency types //

    /**
     * identifies one lifetime of a vertex storage slot
     * an ID becomes stale when its slot is removed and later reused
     */
    struct VertexId
    {
        std::size_t index;
        std::size_t generation;

        bool operator==(const VertexId &) const = default;
    };

    /**
     * identifies one lifetime of an edge storage slot
     * an ID becomes stale when its slot is removed and later reused
     */
    struct EdgeId
    {
        std::size_t index;
        std::size_t generation;

        bool operator==(const EdgeId &) const = default;
    };

    /**
     * one entry in a vertex's adjacency list
     * stores the neighboring vertex and the connecting edge
     */
    struct Adjacency
    {
        VertexId neighbor;
        EdgeId edge;
    };

    // construction and graph data //

    /**
     * creates an empty graph with default (empty) graph data
     */
    Graph() = default;

    /**
     * creates an empty graph with the supplied graph data
     * @param graphData data stored with the graph
     */
    explicit Graph(GraphData graphData);

    /**
     * returns editable access to the graph data
     */
    GraphData &graphData();

    /**
     * returns read-only access to the graph data
     */
    const GraphData &graphData() const;

    // vertex operations //

    /**
     * adds a vertex with the supplied data and no connections
     * @return ID of the new vertex
     */
    VertexId addVertex(VertexData vertexData);

    /**
     * removes a vertex and all of its incident edges
     * removed storage may be reused with a new generation
     * IDs of other existing vertices remain unchanged
     * @return true if the vertex existed and was removed, false otherwise
     */
    bool removeVertex(VertexId id);

    /**
     * returns the number of currently existing vertices
     */
    std::size_t vertexCount() const;

    /**
     * checks whether the ID refers to a currently existing vertex
     * @return false for an out-of-range index, unused slot, or stale generation
     */
    bool vertexExists(VertexId id) const;

    /**
     * returns the number of neighbours of a vertex
     * @throws std::invalid_argument if the vertex does not exist
     */
    std::size_t degree(VertexId id) const;

    /**
     * returns read-only access to the vertex's adjacency list
     * @throws std::invalid_argument if the vertex does not exist
     */
    const std::vector<Adjacency> &adjacency(VertexId id) const;

    /**
     * returns editable access to the vertex data
     * @throws std::invalid_argument if the vertex does not exist
     */
    VertexData &vertexData(VertexId id);

    /**
     * returns read-only access to the vertex data
     * @throws std::invalid_argument if the vertex does not exist
     */
    const VertexData &vertexData(VertexId id) const;

    // edge operations //

    /**
     * adds an undirected edge with the supplied data
     * existing edge data is left unchanged if the edge already exists
     * @return true if the edge was added, false if it already exists
     * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
     */
    bool addEdge(VertexId u, VertexId v, EdgeData edgeData);

    /**
     * removes an edge
     * removed storage may be reused with a new generation
     * IDs of other existing edges remain unchanged
     * @return true if the edge existed and was removed, false otherwise
     */
    bool removeEdge(EdgeId id);

    /**
     * returns the number of currently existing edges
     */
    std::size_t edgeCount() const;

    /**
     * checks whether an edge exists between two vertices
     * @return true if an edge connects the vertices, false otherwise
     * @throws std::invalid_argument if a vertex does not exist
     */
    bool hasEdge(VertexId u, VertexId v) const;

    /**
     * finds the ID of the edge connecting two vertices
     * @return edge ID if an edge exists, otherwise std::nullopt
     * @throws std::invalid_argument if a vertex does not exist
     */
    std::optional<EdgeId> findEdgeId(VertexId u, VertexId v) const;

    /**
     * checks whether the ID refers to a currently existing edge
     * @return false for an out-of-range index, unused slot, or stale generation
     */
    bool edgeExists(EdgeId id) const;

    /**
     * returns editable access to the edge data
     * @throws std::invalid_argument if the edge does not exist
     */
    EdgeData &edgeData(EdgeId id);

    /**
     * returns read-only access to the edge data
     * @throws std::invalid_argument if the edge does not exist
     */
    const EdgeData &edgeData(EdgeId id) const;

    /**
     * returns the first endpoint of the undirected edge
     * @throws std::invalid_argument if the edge does not exist
     */
    VertexId endpointU(EdgeId id) const;

    /**
     * returns the second endpoint of the undirected edge
     * @throws std::invalid_argument if the edge does not exist
     */
    VertexId endpointV(EdgeId id) const;

private:
    // internal storage types

    /**
     * vertex stored inside a vertex slot, which owns its data and adjacency list
     */
    struct Vertex
    {
        VertexData data;
        std::vector<Adjacency> adjacency;
    };

    /**
     * storage for one vertex that can be reused after removal
     * if the slot is unused the vertex is empty
     * generation is increased when a new vertex reuses this slot
     */
    struct VertexSlot
    {
        std::optional<Vertex> vertex;
        std::size_t generation = 0;
    };

    /**
     * undirected edge stored inside an edge slot, which owns its data and stores both endpoints (endpoint order does not imply direction)
     */
    struct Edge
    {
        VertexId endpointU;
        VertexId endpointV;
        EdgeData data;
    };

    /**
     * storage for one edge that can be reused after removal
     * if the slot is unused the edge is empty
     * generation is increased when a new edge reuses this slot
     */
    struct EdgeSlot
    {
        std::optional<Edge> edge;
        std::size_t generation = 0;
    };

    // graph storage

    // data stored with the whole graph
    GraphData graphData_{};

    // reusable storage for vertex slots
    std::vector<VertexSlot> vertices_;

    // reusable storage for edge slots
    std::vector<EdgeSlot> edges_;

    // indices of unused vertex slots available for reuse
    std::vector<std::size_t> freeVertexSlots_;

    // indices of unused edge slots available for reuse
    std::vector<std::size_t> freeEdgeSlots_;

    // number of currently existing vertices
    std::size_t vertexCount_ = 0;

    // number of currently existing edges
    std::size_t edgeCount_ = 0;
};

#include "graph.tpp"