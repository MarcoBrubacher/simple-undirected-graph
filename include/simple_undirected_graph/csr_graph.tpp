#include <utility>
#include <limits>
#include <stdexcept>

namespace simple_undirected_graph
{
    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData>::CSRGraph()
        requires std::default_initializable<GraphData>

    {
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData>::CSRGraph(GraphData graphData)
        requires std::constructible_from<GraphData, GraphData &&>
        : graphData_(std::move(graphData))
    {
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData>::CSRGraph(const CSRGraph &sourceGraph)
        requires std::constructible_from<VertexData, const VertexData &> && std::constructible_from<EdgeData, const EdgeData &> && std::constructible_from<GraphData, const GraphData &>
        : graphData_(sourceGraph.graphData_),
          vertexData_(sourceGraph.vertexData_),
          edgeData_(sourceGraph.edgeData_),
          offsets_(sourceGraph.offsets_),
          neighbors_(sourceGraph.neighbors_),
          edgeIndices_(sourceGraph.edgeIndices_)
    {
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData> &CSRGraph<VertexData, EdgeData, GraphData>::operator=(const CSRGraph &sourceGraph)
        requires std::constructible_from<VertexData, const VertexData &> && std::constructible_from<EdgeData, const EdgeData &> && std::constructible_from<GraphData, const GraphData &> && std::assignable_from<GraphData &, GraphData &&>
    {
        if (this == &sourceGraph)
        {
            return *this;
        }

        CSRGraph copy(sourceGraph);
        *this = std::move(copy);
        return *this;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData>::CSRGraph(CSRGraph &&sourceGraph) noexcept(std::is_nothrow_move_constructible_v<GraphData>)
        requires std::constructible_from<GraphData, GraphData &&>
        : graphData_(std::move(sourceGraph.graphData_)),
          vertexData_(std::move(sourceGraph.vertexData_)),
          edgeData_(std::move(sourceGraph.edgeData_)),
          offsets_(std::move(sourceGraph.offsets_)),
          neighbors_(std::move(sourceGraph.neighbors_)),
          edgeIndices_(std::move(sourceGraph.edgeIndices_))
    {
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData> &CSRGraph<VertexData, EdgeData, GraphData>::operator=(CSRGraph &&sourceGraph) noexcept(std::is_nothrow_move_assignable_v<GraphData>)
        requires std::assignable_from<GraphData &, GraphData &&>
    {
        if (this == &sourceGraph)
        {
            return *this;
        }

        graphData_ = std::move(sourceGraph.graphData_);
        vertexData_ = std::move(sourceGraph.vertexData_);
        edgeData_ = std::move(sourceGraph.edgeData_);

        offsets_ = std::move(sourceGraph.offsets_);
        neighbors_ = std::move(sourceGraph.neighbors_);
        edgeIndices_ = std::move(sourceGraph.edgeIndices_);

        return *this;
    }

    /**
     *
     *graph operations
     *
     */

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool CSRGraph<VertexData, EdgeData, GraphData>::isValidCSRFormat(std::size_t vertexCount,
                                                                     std::size_t edgeCount,
                                                                     std::span<const std::size_t> offsets,
                                                                     std::span<const Index> neighbors,
                                                                     std::span<const Index> edgeIndices)
    {
        const std::size_t maxIndex = std::numeric_limits<Index>::max();

        // storage invariants
        if (offsets.empty() ||
            offsets.size() - 1 != vertexCount ||
            neighbors.size() != edgeIndices.size() ||
            neighbors.size() % 2 != 0 ||
            neighbors.size() / 2 != edgeCount ||
            offsets.front() != 0 ||
            offsets.back() != neighbors.size() ||
            vertexCount > maxIndex ||
            edgeCount > maxIndex)
        {
            return false;
        }

        // track previously encountered neighbors, use the integer maximum to mark unseen neighbors, otherwise store the index of the vertex that encountered them
        std::vector<Index> seenNeighbor(vertexCount, std::numeric_limits<Index>::max());

        std::vector<std::uint8_t> edgeInstance(edgeCount, 0);
        std::vector<std::pair<Index, Index>> firstDirection(edgeCount); // first seen direction of an edge

        for (std::size_t u = 0; u < vertexCount; ++u)
        {
            // validate offsets
            if (offsets[u] > offsets[u + 1] || offsets[u + 1] > neighbors.size())
            {
                return false;
            }

            // validate adjacency indices
            for (std::size_t i = offsets[u]; i < offsets[u + 1]; ++i)
            {
                if (neighbors[i] >= vertexCount || edgeIndices[i] >= edgeCount)
                {
                    return false;
                }

                // reject self-loops
                if (neighbors[i] == u)
                {
                    return false;
                }

                // reject duplicates

                const Index v = neighbors[i];
                if (seenNeighbor[v] == u)
                {
                    return false;
                }
                seenNeighbor[v] = static_cast<Index>(u); // casting because the Index is std::uint32_t

                const Index edge = edgeIndices[i];
                if (edgeInstance[edge] == 0)
                {
                    firstDirection[edge] = {static_cast<Index>(u), v};
                }
                else if (edgeInstance[edge] == 1)
                {
                    const std::pair<Index, Index> &dir = firstDirection[edge];
                    if (dir.first != v || dir.second != u)
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
                ++edgeInstance[edge];
            }
        }
        return true;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData> CSRGraph<VertexData, EdgeData, GraphData>::fromValidCSRFormat(GraphData graphData,
                                                                                                            std::vector<VertexData> vertexData,
                                                                                                            std::vector<EdgeData> edgeData,
                                                                                                            std::vector<std::size_t> offsets,
                                                                                                            std::vector<Index> neighbors,
                                                                                                            std::vector<Index> edgeIndices)
    {
        return CSRGraph(std::move(graphData), std::move(vertexData), std::move(edgeData), std::move(offsets), std::move(neighbors), std::move(edgeIndices));
    }

    // ¯\_(ツ)_/¯
    template <typename VertexData, typename EdgeData, typename GraphData>
    CSRGraph<VertexData, EdgeData, GraphData>::CSRGraph(GraphData &&graphData,
                                                        std::vector<VertexData> &&vertexData,
                                                        std::vector<EdgeData> &&edgeData,
                                                        std::vector<std::size_t> &&offsets,
                                                        std::vector<Index> &&neighbors,
                                                        std::vector<Index> &&edgeIndices) : graphData_(std::move(graphData)),
                                                                                            vertexData_(std::move(vertexData)),
                                                                                            edgeData_(std::move(edgeData)),
                                                                                            offsets_(std::move(offsets)),
                                                                                            neighbors_(std::move(neighbors)),
                                                                                            edgeIndices_(std::move(edgeIndices)) {}

    template <typename VertexData, typename EdgeData, typename GraphData>
    GraphData &CSRGraph<VertexData, EdgeData, GraphData>::graphData() noexcept
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const GraphData &CSRGraph<VertexData, EdgeData, GraphData>::graphData() const noexcept
    {
        return graphData_;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool CSRGraph<VertexData, EdgeData, GraphData>::empty() const noexcept
    {
        return vertexData_.empty();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool CSRGraph<VertexData, EdgeData, GraphData>::vertexExists(VertexId id) const noexcept
    {
        return id < vertexData_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t CSRGraph<VertexData, EdgeData, GraphData>::vertexCount() const noexcept
    {
        return vertexData_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t CSRGraph<VertexData, EdgeData, GraphData>::degree(VertexId id) const noexcept
    {
        return offsets_[id + 1] - offsets_[id];
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename CSRGraph<VertexData, EdgeData, GraphData>::Index> CSRGraph<VertexData, EdgeData, GraphData>::neighbors(VertexId id) const noexcept
    {
        const std::size_t start = offsets_[id];
        const std::size_t count = offsets_[id + 1] - start;

        return std::span<const Index>(neighbors_).subspan(start, count);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename CSRGraph<VertexData, EdgeData, GraphData>::Index> CSRGraph<VertexData, EdgeData, GraphData>::incidentEdgeIndices(VertexId id) const noexcept
    {
        const std::size_t start = offsets_[id];
        const std::size_t count = offsets_[id + 1] - start;

        return std::span<const Index>(edgeIndices_).subspan(start, count);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    VertexData &CSRGraph<VertexData, EdgeData, GraphData>::vertexData(VertexId id) noexcept
    {
        return vertexData_[id];
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const VertexData &CSRGraph<VertexData, EdgeData, GraphData>::vertexData(VertexId id) const noexcept
    {
        return vertexData_[id];
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<VertexData> CSRGraph<VertexData, EdgeData, GraphData>::vertexData() noexcept
    {
        return std::span<VertexData>(vertexData_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const VertexData> CSRGraph<VertexData, EdgeData, GraphData>::vertexData() const noexcept
    {
        return std::span<const VertexData>(vertexData_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool CSRGraph<VertexData, EdgeData, GraphData>::edgeExists(EdgeId id) const noexcept
    {
        return id < edgeData_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::size_t CSRGraph<VertexData, EdgeData, GraphData>::edgeCount() const noexcept
    {
        return edgeData_.size();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::optional<typename CSRGraph<VertexData, EdgeData, GraphData>::EdgeId> CSRGraph<VertexData, EdgeData, GraphData>::findEdgeId(VertexId u, VertexId v) const noexcept
    {
        if (u == v)
        {
            return std::nullopt;
        }

        if (degree(u) > degree(v))
        {
            std::swap(u, v);
        }

        for (std::size_t i = offsets_[u]; i < offsets_[u + 1]; ++i)
        {
            if (neighbors_[i] == v)
            {
                return edgeIndices_[i];
            }
        }
        return std::nullopt;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    bool CSRGraph<VertexData, EdgeData, GraphData>::hasEdge(VertexId u, VertexId v) const noexcept
    {
        return findEdgeId(u, v).has_value();
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::vector<std::pair<typename CSRGraph<VertexData, EdgeData, GraphData>::VertexId, typename CSRGraph<VertexData, EdgeData, GraphData>::VertexId>> CSRGraph<VertexData, EdgeData, GraphData>::reconstructEdgeEndpoints() const
    {

        std::vector<std::pair<VertexId, VertexId>> endpoints(edgeCount());

        //  size_t is used for iteration because vector sizes and CSR offsets uses size_t
        for (std::size_t u = 0; u < vertexCount(); ++u)
        {
            for (std::size_t i = offsets_[u]; i < offsets_[u + 1]; ++i)
            {
                const VertexId v = neighbors_[i];
                const EdgeId e = edgeIndices_[i];

                if (u < v)
                {
                    endpoints[e] = {static_cast<VertexId>(u), v};
                }
            }
        }
        return endpoints;
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    EdgeData &CSRGraph<VertexData, EdgeData, GraphData>::edgeData(EdgeId id) noexcept
    {
        return edgeData_[id];
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    const EdgeData &CSRGraph<VertexData, EdgeData, GraphData>::edgeData(EdgeId id) const noexcept
    {
        return edgeData_[id];
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<EdgeData> CSRGraph<VertexData, EdgeData, GraphData>::edgeData() noexcept
    {
        return std::span<EdgeData>(edgeData_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const EdgeData> CSRGraph<VertexData, EdgeData, GraphData>::edgeData() const noexcept
    {
        return std::span<const EdgeData>(edgeData_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const std::size_t> CSRGraph<VertexData, EdgeData, GraphData>::offsets() const noexcept
    {
        return std::span<const std::size_t>(offsets_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename CSRGraph<VertexData, EdgeData, GraphData>::Index>
    CSRGraph<VertexData, EdgeData, GraphData>::neighborIndices() const noexcept
    {
        return std::span<const Index>(neighbors_);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
    std::span<const typename CSRGraph<VertexData, EdgeData, GraphData>::Index>
    CSRGraph<VertexData, EdgeData, GraphData>::adjacencyEdgeIndices() const noexcept
    {
        return std::span<const Index>(edgeIndices_);
    }
}