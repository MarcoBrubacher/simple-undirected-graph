#pragma once
#include <cstddef>
#include <vector>
#include <stdexcept>
#include <optional>
#include <utility>
#include <atomic>

namespace simple_undirected_graph
{
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
     * vertex and edge IDs belong to one graph instance and are generation-safe
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
         * identifies one vertex in one graph instance
         * an ID becomes invalid if it is used with another graph or its slot is reused
         */
        struct VertexId
        {
            std::size_t graphId;
            std::size_t index;
            std::size_t generation;

            bool operator==(const VertexId &) const = default;
        };

        /**
         * identifies one edge in one graph instance
         * an ID becomes invalid if it is used with another graph or its slot is reused
         */
        struct EdgeId
        {
            std::size_t graphId;
            std::size_t index;
            std::size_t generation;

            bool operator==(const EdgeId &) const = default;
        };

        /**
         * one entry in a vertex's adjacency list; stores the neighboring vertex and the connecting edge
         */
        struct Adjacency
        {
            VertexId neighbor;
            EdgeId edge;
        };

        // construction and graph data //

        // creates an empty graph with default-initialized graph data
        Graph();

        /**
         * creates an empty graph with the supplied graph data
         * @param graphData data stored with the graph
         */
        explicit Graph(GraphData graphData);

        /**
         * creates a new graph by copying another graph (IDs from the original graph are not valid in the copy)
         * @param other graph to copy
         */
        Graph(const Graph &other);

        /**
         * replaces this graph with a copy of another graph (IDs from the other graph are not valid in this graph after the copy)
         * @param other graph to copy
         * @return this graph
         */
        Graph &operator=(const Graph &other);

        /**
         * creates a graph by taking ownership of another graph's contents (existing IDs continue to belong to the moved graph)
         * @param other graph to move from
         */
        Graph(Graph &&other);

        /**
         * replaces this graph by taking ownership of another graph's contents (existing IDs continue to belong to the moved graph)
         * @param other graph to move from
         * @return this graph
         */
        Graph &operator=(Graph &&other);

        // returns editable access to the graph data
        GraphData &graphData();

        // returns read-only access to the graph data
        const GraphData &graphData() const;

        /**
         * removes all vertices and edges (all previously issued vertex and edge IDs become invalid)
         */
        void clear();

        /**
         * checks whether the graph has no vertices
         * @return true if the graph is empty, false otherwise
         */
        bool empty() const;

        // vertex operations //

        /**
         * @return ID of the new vertex
         */
        VertexId addVertex(VertexData vertexData);

        /**
         * IDs of other existing vertices remain unchanged
         * @return true if the vertex existed and was removed, false otherwise
         */
        bool removeVertex(VertexId id);

        std::size_t vertexCount() const;

        /**
         * checks whether the ID refers to a currently existing vertex in this graph
         * @return false for another graph, out-of-range index, unused slot, or stale generation
         */
        bool vertexExists(VertexId id) const;

        /**
         * returns the number of neighbours of a vertex (which equals the numbers of incident edges)
         * @throws std::invalid_argument if the vertex does not exist
         */
        std::size_t degree(VertexId id) const;

        /**
         * returns read-only access to the vertex's adjacency list
         * this reference becomes invalid if the vertex is removed and may become invalid if vertex storage moves
         * keep the VertexId and call adjacency(id) again when accessing the adjacency list later
         * @throws std::invalid_argument if the vertex does not exist
         */
        const std::vector<Adjacency> &adjacency(VertexId id) const;
        /**
         * returns editable access to the vertex data
         * this reference becomes invalid if the vertex is removed and may become invalid if vertex storage moves
         * keep the VertexId and call vertexData(id) again when accessing the vertex later
         * @throws std::invalid_argument if the vertex does not exist
         */
        VertexData &vertexData(VertexId id);

        /**
         * returns read-only access to the vertex data
         * this reference becomes invalid if the vertex is removed and may become invalid if vertex storage moves
         * keep the VertexId and call vertexData(id) again when accessing the vertex later
         * @throws std::invalid_argument if the vertex does not exist
         */
        const VertexData &vertexData(VertexId id) const;

        // edge operations //

        /**
         * existing edge data is left unchanged if the edge already exists
         * @return {edge ID, true} if a new edge was added, otherwise {existing edge ID, false}
         * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
         */
        std::pair<EdgeId, bool> addEdge(VertexId u, VertexId v, EdgeData edgeData);

        /**
         * removed storage may be reused with a new generation (IDs of other existing edges remain unchanged)
         * @return true if the edge existed and was removed, false otherwise
         */
        bool removeEdge(EdgeId id);

        std::size_t edgeCount() const;

        /**
         * checks whether the ID refers to a currently existing edge in this graph
         * @return false for another graph, out-of-range index, unused slot, or stale generation
         */
        bool edgeExists(EdgeId id) const;

        /**
         * checks whether an edge exists between two vertices
         * @return true if an edge connects the vertices, false otherwise
         * @throws std::invalid_argument if a vertex does not exist
         */
        bool hasEdge(VertexId u, VertexId v) const;

        /**
         * finds the ID of the edge connecting two vertices
         * @return edge ID if an edge exists
         * @throws std::invalid_argument if a vertex does not exist
         */
        std::optional<EdgeId> findEdgeId(VertexId u, VertexId v) const;

        /**
         * returns editable access to the edge data
         * this reference becomes invalid if the edge is removed and may become invalid if edge storage moves
         * keep the EdgeId and call edgeData(id) again when accessing the edge later
         * @throws std::invalid_argument if the edge does not exist
         */
        EdgeData &edgeData(EdgeId id);

        /**
         * returns read-only access to the edge data
         * this reference becomes invalid if the edge is removed and may become invalid if edge storage moves
         * keep the EdgeId and call edgeData(id) again when accessing the edge later
         * @throws std::invalid_argument if the edge does not exist
         */
        const EdgeData &edgeData(EdgeId id) const;

        /**
         * @throws std::invalid_argument if the edge does not exist
         */
        VertexId endpointU(EdgeId id) const;

        /**
         * @throws std::invalid_argument if the edge does not exist
         */
        VertexId endpointV(EdgeId id) const;

    private:
        // internal storage types //

        /**
         * vertex stored inside a vertex slot, which owns its data and adjacency list
         */
        struct Vertex
        {
            VertexData data;
            std::vector<Adjacency> adjacency;
        };

        /**
         * storage for one vertex that can be reused after removal (if the slot is unused the vertex is empty)
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
         * storage for one edge that can be reused after removal (if the slot is unused the edge is empty)
         * generation is increased when a new edge reuses this slot
         */
        struct EdgeSlot
        {
            std::optional<Edge> edge;
            std::size_t generation = 0;
        };

        // graph storage //

        // provides and ensures unique IDs for graph instances
        inline static std::atomic<std::size_t> nextGraphId_ = 0;

        std::size_t graphId_;
        GraphData graphData_{};

        std::vector<VertexSlot> vertexSlots_;
        std::vector<EdgeSlot> edgeSlots_;

        std::vector<std::size_t> freeVertexSlots_;
        std::vector<std::size_t> freeEdgeSlots_;

        std::size_t vertexCount_ = 0;
        std::size_t edgeCount_ = 0;
    };

#include "graph.tpp"
}