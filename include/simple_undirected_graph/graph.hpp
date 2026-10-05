#pragma once
#include <cstddef>
#include <vector>
#include <stdexcept>
#include <optional>
#include <atomic>
#include <utility>
#include <span>
#include <unordered_map>
#include <concepts>
#include <type_traits>

namespace simple_undirected_graph
{
    /**
     * empty property type used when no data is attached
     */
    struct NoProperties
    {
    };

    /**
     * finite simple undirected graph with typed vertex, edge, and graph data
     * vertices and edges may carry user-defined data, while graph-level data is stored once for the whole graph
     *
     * @tparam VertexData type stored with each vertex
     * @tparam EdgeData type stored with each edge
     * @tparam GraphData type stored with the whole graph
     */
    template <typename VertexData = NoProperties, typename EdgeData = NoProperties, typename GraphData = NoProperties>
    class Graph
    {
    public:
        struct VertexIdHash;
        struct EdgeIdHash;

        /**
         * identifies one vertex in one graph instance
         * an ID becomes invalid if it is used with another graph or its slot is reused
         */
        struct VertexId
        {
        public:
            bool operator==(const VertexId &) const noexcept = default;

        private:
            friend class Graph;
            friend struct VertexIdHash;

            VertexId(std::size_t graphId, std::size_t index, std::size_t generation);

            // is the ID of the graph this vertex belongs to and must match graphId_
            std::size_t graphId;
            // is the index of this vertex in vertexSlots_
            std::size_t index;
            // is the generation number of vertexSlots_[index] when this ID was created
            std::size_t generation;
        };

        /**
         * identifies one edge in one graph instance
         * an ID becomes invalid if it is used with another graph or its slot is reused
         */
        struct EdgeId
        {
        public:
            bool operator==(const EdgeId &) const noexcept = default;

        private:
            friend class Graph;
            friend struct EdgeIdHash;

            EdgeId(std::size_t graphId, std::size_t index, std::size_t generation);

            // is the ID of the graph this vertex belongs to, must match graphId_
            std::size_t graphId;
            // is the index of this edge in edgeSlots_
            std::size_t index;
            // is the generation number of edgeSlots_[index] when this ID was created
            std::size_t generation;
        };

        // allows vertex and edge IDs to be used as keys in hash-based containers (buckets)
        struct VertexIdHash
        {
            std::size_t operator()(const VertexId &id) const noexcept;
        };
        struct EdgeIdHash
        {
            std::size_t operator()(const EdgeId &id) const noexcept;
        };

        /**
         * one entry in a vertex's adjacency list
         */
        struct Adjacency
        {
            // neighbor of the vertex that owns this adjacency entry
            VertexId neighbor;

            // edge between the vertex that owns this adjacency entry and the neighbor
            EdgeId edge;
        };

        // creates empty graph with default-initialized graph data
        Graph()
            requires std::default_initializable<GraphData>;

        /**
         * creates an empty graph with the supplied graph data
         * @param graphData data stored with the graph
         */
        explicit Graph(GraphData graphData)
            requires std::constructible_from<GraphData, GraphData &&>;

        /**
         * creates a new graph by copying another graph
         * IDs from the source graph are not valid in the copy
         * @param sourceGraph graph to copy
         */
        Graph(const Graph &sourceGraph)
            requires std::constructible_from<VertexData, const VertexData &> && std::constructible_from<EdgeData, const EdgeData &> && std::constructible_from<GraphData, const GraphData &>;

        /**
         * replaces this graph with a copy of another graph
         * IDs from the source graph are not valid in this graph after the copy
         * @param sourceGraph graph to copy
         * @return this graph
         */
        Graph &operator=(const Graph &sourceGraph);

        /**
         * creates a graph by taking ownership of another graph's contents
         * existing IDs continue to belong to the moved graph
         * the source graph is left empty with a new graph identity
         * @param sourceGraph graph to move from
         */
        Graph(Graph &&sourceGraph) noexcept(std::is_nothrow_move_constructible_v<GraphData>);

        /**
         * replaces this graph by taking ownership of another graph's contents
         * existing IDs continue to belong to the moved graph
         * the source graph is left empty with a new graph identity
         * @param sourceGraph graph to move from
         * @return this graph
         */
        Graph &operator=(Graph &&sourceGraph) noexcept(std::is_nothrow_move_assignable_v<GraphData>);

        // returns editable access to the graph data
        [[nodiscard]] GraphData &graphData() noexcept;

        // returns read-only access to the graph data
        [[nodiscard]] const GraphData &graphData() const noexcept;

        /**
         * removes all vertices and edges (all previously issued vertex and edge IDs become invalid)
         */
        void clear() noexcept;

        /**
         * checks whether the graph has no vertices
         * @return true if the graph is empty, false otherwise
         */
        [[nodiscard]] bool empty() const noexcept;

        // vertex operations //

        /**
         * creates a vertex without properties
         * @return ID of the new vertex
         */
        VertexId addVertex()
            requires std::same_as<VertexData, NoProperties>;

        /**
         * creates a vertex by copying the supplied vertex data
         * @param vertexData data copied into the vertex
         * @return ID of the new vertex
         */
        VertexId addVertex(const VertexData &vertexData);

        /**
         * creates a vertex by moving the supplied vertex data
         * @param vertexData data moved into the vertex
         * @return ID of the new vertex
         */
        VertexId addVertex(VertexData &&vertexData);

        /**
         * creates a vertex by constructing its data directly from the supplied arguments
         * @param args arguments forwarded to the VertexData constructor
         * @return ID of the new vertex
         */
        template <typename... Args>
            requires std::constructible_from<VertexData, Args...>
        VertexId emplaceVertex(Args &&...args);

        /**
         * IDs of other existing vertices remain unchanged
         * @return true if the vertex existed and was removed, false otherwise
         */
        bool removeVertex(VertexId id);

        [[nodiscard]] std::size_t vertexCount() const noexcept;

        /**
         * checks whether the ID refers to a currently existing vertex in this graph
         * @return false for another graph, out-of-range index, unused slot, or stale generation
         */
        [[nodiscard]] bool vertexExists(VertexId id) const noexcept;

        /**
         * returns the number of neighbours of a vertex (which equals the numbers of incident edges)
         * @throws std::invalid_argument if the vertex does not exist
         */
        [[nodiscard]] std::size_t degree(VertexId id) const;

        /**
         * returns read-only access to the vertex's adjacency entries
         * adjacency order is unspecified and may change when the graph topology is modified
         * the returned span is valid until the graph topology is modified
         * @throws std::invalid_argument if the vertex does not exist
         */
        [[nodiscard]] std::span<const Adjacency> adjacency(VertexId id) const;

        /**
         * returns editable access to the vertex data
         * this reference becomes invalid if the vertex is removed and may become invalid if vertex storage moves
         * keep the VertexId and call vertexData(id) again when accessing the vertex later
         * @throws std::invalid_argument if the vertex does not exist
         */
        [[nodiscard]] VertexData &vertexData(VertexId id);

        /**
         * returns read-only access to the vertex data
         * this reference becomes invalid if the vertex is removed and may become invalid if vertex storage moves
         * keep the VertexId and call vertexData(id) again when accessing the vertex later
         * @throws std::invalid_argument if the vertex does not exist
         */
        [[nodiscard]] const VertexData &vertexData(VertexId id) const;

        // edge operations //

        /**
         * creates an edge without properties
         * @return {edge ID, true} if a new edge was added, otherwise {existing edge ID, false}
         * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
         */
        std::pair<EdgeId, bool> addEdge(VertexId u, VertexId v)
            requires std::same_as<EdgeData, NoProperties>;

        /**
         * creates an edge with the supplied edge data
         * existing edge data is left unchanged if the edge already exists
         * @param u first endpoint
         * @param v second endpoint
         * @param edgeData data stored with the edge
         * @return {edge ID, true} if a new edge was added, otherwise {existing edge ID, false}
         * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
         */
        std::pair<EdgeId, bool> addEdge(VertexId u, VertexId v, const EdgeData &edgeData);

        /**
         * creates an edge with the supplied edge data
         * existing edge data is left unchanged if the edge already exists
         * @param u first endpoint
         * @param v second endpoint
         * @param edgeData data moved into the edge
         * @return {edge ID, true} if a new edge was added, otherwise {existing edge ID, false}
         * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
         */
        std::pair<EdgeId, bool> addEdge(VertexId u, VertexId v, EdgeData &&edgeData);

        /**
         * creates an edge by constructing its data directly from the supplied arguments
         * existing edge data is left unchanged if the edge already exists
         * @param u first endpoint
         * @param v second endpoint
         * @param args arguments forwarded to the EdgeData constructor
         * @return {edge ID, true} if a new edge was added, otherwise {existing edge ID, false}
         * @throws std::invalid_argument if a vertex does not exist or both endpoints refer to the same vertex
         */
        template <typename... Args>
            requires std::constructible_from<EdgeData, Args...>
        std::pair<EdgeId, bool> emplaceEdge(VertexId u, VertexId v, Args &&...args);

        /**
         * removed storage may be reused with a new generation (IDs of other existing edges remain unchanged)
         * @return true if the edge existed and was removed, false otherwise
         */
        bool removeEdge(EdgeId id);

        [[nodiscard]] std::size_t edgeCount() const noexcept;

        /**
         * checks whether the ID refers to a currently existing edge in this graph
         * @return false for another graph, out-of-range index, unused slot, or stale generation
         */
        [[nodiscard]] bool edgeExists(EdgeId id) const noexcept;

        /**
         * checks whether an edge exists between two vertices
         * @return true if an edge connects the vertices, false otherwise
         * @throws std::invalid_argument if a vertex does not exist
         */
        [[nodiscard]] bool hasEdge(VertexId u, VertexId v) const;

        /**
         * finds the ID of the edge connecting two vertices
         * @return edge ID if an edge exists
         * @throws std::invalid_argument if a vertex does not exist
         */
        [[nodiscard]] std::optional<EdgeId> findEdgeId(VertexId u, VertexId v) const;

        /**
         * returns editable access to the edge data
         * this reference becomes invalid if the edge is removed and may become invalid if edge storage moves
         * keep the EdgeId and call edgeData(id) again when accessing the edge later
         * @throws std::invalid_argument if the edge does not exist
         */
        [[nodiscard]] EdgeData &edgeData(EdgeId id);

        /**
         * returns read-only access to the edge data
         * this reference becomes invalid if the edge is removed and may become invalid if edge storage moves
         * keep the EdgeId and call edgeData(id) again when accessing the edge later
         * @throws std::invalid_argument if the edge does not exist
         */
        [[nodiscard]] const EdgeData &edgeData(EdgeId id) const;

        /**
         * returns both endpoints of an edge, since endpoint order has no directional meaning
         * @throws std::invalid_argument if the edge does not exist
         */
        [[nodiscard]] std::pair<VertexId, VertexId> endpoints(EdgeId id) const;

        // iteration //

        /**
         * returns read-only access to the existing vertex IDs
         * iteration order is unspecified and may change when the graph topology is modified
         * the returned span is valid until the graph topology is modified
         */
        [[nodiscard]] std::span<const VertexId> vertices() const noexcept;

        /**
         * returns read-only access to the existing edge IDs
         * iteration order is unspecified and may change when the graph topology is modified
         * the returned span is valid until the graph topology is modified
         */
        [[nodiscard]] std::span<const EdgeId> edges() const noexcept;

    private:
        template <typename... Args>
            requires std::constructible_from<VertexData, Args...>
        VertexId insertVertex(Args &&...args);

        template <typename... Args>
            requires std::constructible_from<EdgeData, Args...>
        std::pair<EdgeId, bool> insertEdge(VertexId u, VertexId v, Args &&...args);

        /**
         * vertex stored inside a vertex slot
         */
        struct Vertex
        {
            template <typename... Args>
                requires std::constructible_from<VertexData, Args...>
            explicit Vertex(std::in_place_t, Args &&...args);

            VertexData data;

            // stores the neighboring vertices and their connecting edge IDs
            std::vector<Adjacency> adjacency;
        };

        /**
         * storage for one vertex that can be reused after removal (if the slot is unused the vertex is empty)
         * generation is increased when a new vertex reuses this slot!
         */
        struct VertexSlot
        {
            std::optional<Vertex> vertex;
            std::size_t generation = 0;
            // is the index of this vertex in liveVertices_
            std::size_t liveIndex = 0;
        };

        /**
         * undirected edge stored inside an edge slot
         */
        struct Edge
        {
            template <typename... Args>
                requires std::constructible_from<EdgeData, Args...>
            Edge(VertexId u, VertexId v, std::size_t adjacencyIndexU, std::size_t adjacencyIndexV, std::in_place_t, Args &&...args);

            EdgeData data;

            // endpointU.index indexes vertexSlots_
            VertexId endpointU;
            // endpointV.index indexes vertexSlots_
            VertexId endpointV;

            // is the index of this edge in the adjacency vector of vertex U
            std::size_t adjacencyIndexU;
            // is the index of this edge in the adjacency vector of vertex V
            std::size_t adjacencyIndexV;
        };

        /**
         * storage for one edge that can be reused after removal (if the slot is unused the edge is empty)
         * generation is increased when a new edge reuses this slot!
         */
        struct EdgeSlot
        {
            std::optional<Edge> edge;
            std::size_t generation = 0;
            // is the index of this edge in liveEdges_
            std::size_t liveIndex = 0;
        };

        // represent an undirected edge by the slot indices of its two endpoint vertices
        using EndpointPair = std::pair<std::size_t, std::size_t>;
        EndpointPair normalizeEndpoints(VertexId u, VertexId v) const noexcept;

        // combines the endpoint indices into one hash
        static std::size_t combineHash(std::size_t seed, std::size_t value) noexcept;
        struct EndpointPairHasher
        {
            std::size_t operator()(const EndpointPair &pair) const noexcept;
        };

        inline static std::atomic<std::size_t> nextGraphId_ = 0;

        std::size_t graphId_;
        GraphData graphData_{};

        // Indexed by vertexId.index
        std::vector<VertexSlot> vertexSlots_;
        // Indexed by edgeId.index
        std::vector<EdgeSlot> edgeSlots_;

        std::vector<std::size_t> freeVertexSlots_;
        std::vector<std::size_t> freeEdgeSlots_;

        // Indexed by vertexSlot.liveIndex
        std::vector<VertexId> liveVertices_;
        // Indexed by edgeSlot.liveIndex
        std::vector<EdgeId> liveEdges_;

        // finds an edge by its two endpoint indices, regardless of order
        std::unordered_map<EndpointPair, EdgeId, EndpointPairHasher> edgeLookup_;
    };
}
#include <simple_undirected_graph/graph.tpp>
