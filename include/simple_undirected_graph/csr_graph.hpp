#pragma once

#include <simple_undirected_graph/graph.hpp>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include <type_traits>

namespace simple_undirected_graph
{
    /**
     * finite simple undirected graph with fixed CSR topology
     *
     * vertex, edge, and graph data remain mutable
     * structural modifications are not supported
     *
     * @tparam VertexData type stored with each vertex
     * @tparam EdgeData type stored with each edge
     * @tparam GraphData type stored with the whole graph
     */
    template <typename VertexData = NoProperties, typename EdgeData = NoProperties, typename GraphData = NoProperties>
    class CSRGraph
    {
    public:
        using Index = std::uint32_t;
        using VertexId = Index;
        using EdgeId = Index;

        CSRGraph()
            requires std::default_initializable<GraphData>;

        explicit CSRGraph(GraphData graphData)
            requires std::constructible_from<GraphData, GraphData &&>;

        CSRGraph(const CSRGraph &sourceGraph)
            requires std::constructible_from<VertexData, const VertexData &> && std::constructible_from<EdgeData, const EdgeData &> && std::constructible_from<GraphData, const GraphData &>;
        CSRGraph &operator=(const CSRGraph &sourceGraph)
            requires std::constructible_from<VertexData, const VertexData &> && std::constructible_from<EdgeData, const EdgeData &> && std::constructible_from<GraphData, const GraphData &> && std::assignable_from<GraphData &, GraphData &&>;

        CSRGraph(CSRGraph &&sourceGraph) noexcept(std::is_nothrow_move_constructible_v<GraphData>)
            requires std::constructible_from<GraphData, GraphData &&>;
        CSRGraph &operator=(CSRGraph &&sourceGraph) noexcept(std::is_nothrow_move_assignable_v<GraphData>)
            requires std::assignable_from<GraphData &, GraphData &&>;

        // graph operations //

        /**
         * constructs a graph from precomputed CSR arrays
         *
         * ensures every edge connects two different vertices and appears in both directions
         * rejects invalid indices and duplicate edges
         * @throws std::invalid_argument if the input is invalid or exceeds the Index range
         */
        [[nodiscard]] static CSRGraph fromCSRFormat(std::vector<VertexData> vertexData,
                                                    std::vector<EdgeData> edgeData,
                                                    std::vector<std::size_t> offsets,
                                                    std::vector<Index> neighbors,
                                                    std::vector<Index> edgeIndices,
                                                    GraphData graphData);

        [[nodiscard]] GraphData &graphData() noexcept;
        [[nodiscard]] const GraphData &graphData() const noexcept;

        [[nodiscard]] bool empty() const noexcept;

        // vertex operations //

        /**
         * checks whether the ID refers to an existing vertex in this graph
         * @return false for another graph or an invalid index
         */
        [[nodiscard]] bool vertexExists(VertexId id) const noexcept;

        [[nodiscard]] std::size_t vertexCount() const noexcept;

        /**
         * returns the number of neighbors of a vertex
         * @pre vertex < vertexCount()
         */
        [[nodiscard]] std::size_t degree(VertexId vertex) const noexcept;

        /**
         * returns the neighboring vertex indices
         * @pre vertex < vertexCount()
         */
        [[nodiscard]] std::span<const Index> neighbors(VertexId vertex) const noexcept;

        /**
         * returns the edge indices corresponding to neighbors()
         * @pre vertex < vertexCount()
         */
        [[nodiscard]] std::span<const Index> incidentEdgeIndices(VertexId vertex) const noexcept;

        // Access one vertex data
        [[nodiscard]] VertexData &vertexData(VertexId id) noexcept;
        [[nodiscard]] const VertexData &vertexData(VertexId id) const noexcept;
        // Access all vertices data
        [[nodiscard]] std::span<VertexData> vertexData() noexcept;
        [[nodiscard]] std::span<const VertexData> vertexData() const noexcept;

        // edge operations //

        /**
         * checks whether the ID refers to an existing edge in this graph
         * @return false for another graph or an invalid index
         */
        [[nodiscard]] bool edgeExists(EdgeId id) const noexcept;

        /**
         * checks whether an edge connects two vertices
         * @pre u < vertexCount() && v < vertexCount()
         */
        [[nodiscard]] bool hasEdge(VertexId u, VertexId v) const noexcept;

        /**
         * finds the edge connecting two vertices
         * @return edge ID if an edge exists, std::nullopt otherwise
         * @pre u < vertexCount() && v < vertexCount()
         */
        [[nodiscard]] std::optional<EdgeId> findEdgeId(VertexId u, VertexId v) const noexcept;

        [[nodiscard]] std::size_t edgeCount() const noexcept;

        // Access one edge data
        [[nodiscard]] EdgeData &edgeData(EdgeId id) noexcept;
        [[nodiscard]] const EdgeData &edgeData(EdgeId id) const noexcept;
        // Access all edges data
        [[nodiscard]] std::span<EdgeData> edgeData() noexcept;
        [[nodiscard]] std::span<const EdgeData> edgeData() const noexcept;

        /**
         * reconstructs edge endpoints for algorithms requiring direct edge-based access, avoids storing endpoints permanently to minimize memory usage
         * @return endpoint pairs indexed by EdgeId
         */
        [[nodiscard]] std::vector<std::pair<VertexId, VertexId>> reconstructEdgeEndpoints() const;

        // direct CSR access //

        // returns the boundaries of all vertex adjacency ranges
        [[nodiscard]] std::span<const std::size_t> offsets() const noexcept;
        // returns all neighboring vertex indices in CSR order
        [[nodiscard]] std::span<const Index> neighborIndices() const noexcept;

        // returns the edge index corresponding to each neighboring vertex entry
        [[nodiscard]] std::span<const Index> adjacencyEdgeIndices() const noexcept;

    private:
        GraphData graphData_{};
        std::vector<VertexData> vertexData_;
        std::vector<EdgeData> edgeData_;

        // start and end of each vertecies neighbor range
        std::vector<std::size_t> offsets_{0};
        // neighboring vertex indices
        std::vector<Index> neighbors_;
        // edge index for each neighbor entry
        std::vector<Index> edgeIndices_;
    };
}
#include <simple_undirected_graph/csr_graph.tpp>
