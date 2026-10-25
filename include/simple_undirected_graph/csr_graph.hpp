#pragma once
#include <simple_undirected_graph/graph.hpp>
#include <cstddef>
#include <vector>
#include <utility>

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
        struct VertexIdHash;
        struct EdgeIdHash;

        /**
         * identifies one vertex in one graph instance
         * an ID becomes invalid if it is used with another graph
         */
        struct VertexId
        {
        public:
            bool operator==(const VertexId &) const noexcept = default;

        private:
            friend class CSRGraph;
            friend struct VertexIdHash;

            VertexId(std::size_t graphId, std::size_t index);

            // is the ID of the graph this vertex belongs to and must match graphId_
            std::size_t graphId;
            // is the index of this vertex in vertexData_ and offsets_
            std::size_t index;
        };

        /**
         * identifies one edge in one graph instance
         * an ID becomes invalid if it is used with another graph
         */
        struct EdgeId
        {
        public:
            bool operator==(const EdgeId &) const noexcept = default;

        private:
            friend class CSRGraph;
            friend struct EdgeIdHash;

            EdgeId(std::size_t graphId, std::size_t index);

            // is the ID of the graph this edge belongs to and must match graphId_
            std::size_t graphId;
            // is the index of this edge in edgeData_ and endpoints_
            std::size_t index;
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

    private:
        // data stored with the whole graph
        GraphData graphData_;
        // identifies this graph instance
        std::size_t graphId_;

        // vertex properties indexed by vertex index
        std::vector<VertexData> vertexData_;
        // edge properties indexed by edge index
        std::vector<EdgeData> edgeData_;

        // offsets_[i] and offsets_[i + 1] define the adjacency range of vertex i
        std::vector<std::size_t> offsets_{0};
        // neighboring vertex indices stored consecutively for all vertices
        std::vector<std::size_t> neighbors_;
        // edge index corresponding to each entry in neighbors_
        std::vector<std::size_t> edgeIndices_;
        // stores both endpoint indices of each undirected edge
        std::vector<std::pair<std::size_t, std::size_t>> endpoints_;
    };

}
#include <simple_undirected_graph/csr_graph.tpp>