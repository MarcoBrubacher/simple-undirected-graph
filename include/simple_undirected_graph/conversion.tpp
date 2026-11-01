#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <utility>
#include <span>

namespace simple_undirected_graph
{
    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::copy_constructible<VertexData> && std::copy_constructible<EdgeData> && std::copy_constructible<GraphData>
    [[nodiscard]] Conversion::CSRConversionRes<VertexData, EdgeData, GraphData> Conversion::toCSRGraph(const DynamicGraph<VertexData, EdgeData, GraphData> &graph)
    {
        return toCSRGraphImpl<false, VertexData, EdgeData, GraphData>(graph);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::move_constructible<VertexData> && std::move_constructible<EdgeData> && std::move_constructible<GraphData>
    [[nodiscard]] Conversion::CSRConversionRes<VertexData, EdgeData, GraphData> Conversion::toCSRGraph(DynamicGraph<VertexData, EdgeData, GraphData> &&graph)
    {
        return toCSRGraphImpl<true, VertexData, EdgeData, GraphData>(graph);
    }

    template <bool Move, typename VertexData, typename EdgeData, typename GraphData>
    Conversion::CSRConversionRes<VertexData, EdgeData, GraphData> Conversion::toCSRGraphImpl(std::conditional_t<Move, DynamicGraph<VertexData, EdgeData, GraphData> &, const DynamicGraph<VertexData, EdgeData, GraphData> &> graph)
    {
        using CSR = CSRConversionRes<VertexData, EdgeData, GraphData>;
        using Dynamic = DynamicGraph<VertexData, EdgeData, GraphData>;
        using Index = typename CSR::Index;
        using VertexRef = std::conditional_t<Move, typename Dynamic::Vertex &, const typename Dynamic::Vertex &>;
        using EdgeRef = std::conditional_t<Move, typename Dynamic::Edge &, const typename Dynamic::Edge &>;

        const std::size_t vertexCount = graph.vertexCount();
        const std::size_t edgeCount = graph.edgeCount();

        if (vertexCount > std::numeric_limits<Index>::max() || edgeCount > std::numeric_limits<Index>::max() || edgeCount > std::numeric_limits<std::size_t>::max() / 2)
        {
            throw std::overflow_error("graph is too large for CSR representation");
        }

        /**
         * begin construction
         */

        // vertex data
        std::vector<CSRProperty<VertexData>> vertexData;
        vertexData.reserve(vertexCount);

        std::vector<std::size_t> offsets;
        offsets.reserve(vertexCount + 1);
        offsets.push_back(0);

        for (const typename Dynamic::VertexId &id : graph.vertices())
        {
            VertexRef vertex = *graph.vertexSlots_[id.index].vertex;
            if constexpr (std::is_same_v<VertexData, bool>) // evaluated only once at compile time
            {
                vertexData.push_back(byteBool{static_cast<std::uint8_t>(vertex.data)});
            }
            else if constexpr (Move)
            {
                vertexData.push_back(std::move(vertex.data));
            }
            else
            {
                vertexData.push_back(vertex.data);
            }

            offsets.push_back(offsets.back() + vertex.adjacency.size());
        }

        // edge data
        std::vector<CSRProperty<EdgeData>> edgeData;
        edgeData.reserve(edgeCount);

        for (const typename Dynamic::EdgeId &id : graph.edges())
        {
            EdgeRef edge = *graph.edgeSlots_[id.index].edge;
            if constexpr (std::is_same_v<EdgeData, bool>)
            {
                edgeData.push_back(byteBool{static_cast<std::uint8_t>(edge.data)});
            }
            else if constexpr (Move)
            {
                edgeData.push_back(std::move(edge.data));
            }
            else
            {
                edgeData.push_back(edge.data);
            }
        }

        // csr arrays construction
        std::vector<Index> neighbors;
        neighbors.reserve(edgeCount * 2);
        std::vector<Index> edgeIndices;
        edgeIndices.reserve(edgeCount * 2);

        for (const typename Dynamic::VertexId &id : graph.vertices())
        {
            const typename Dynamic::Vertex &vertex = *graph.vertexSlots_[id.index].vertex;
            for (const typename Dynamic::Adjacency &adjacency : vertex.adjacency)
            {
                neighbors.push_back(static_cast<Index>(graph.vertexSlots_[adjacency.neighbor.index].liveIndex));
                edgeIndices.push_back(static_cast<Index>(graph.edgeSlots_[adjacency.edge.index].liveIndex));
            }
        }

        if constexpr (Move)
        {
            return CSR::fromValidCSRFormat(std::move(graph.graphData()), std::move(vertexData), std::move(edgeData), std::move(offsets), std::move(neighbors), std::move(edgeIndices));
        }
        else
        {
            return CSR::fromValidCSRFormat(graph.graphData(), std::move(vertexData), std::move(edgeData), std::move(offsets), std::move(neighbors), std::move(edgeIndices));
        }
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::copy_constructible<VertexData> && std::copy_constructible<EdgeData> && std::copy_constructible<GraphData>
    [[nodiscard]] Conversion::DynamicConversionRes<VertexData, EdgeData, GraphData> Conversion::toDynamicGraph(const CSRGraph<VertexData, EdgeData, GraphData> &graph)
    {
        return toDynamicGraphImpl<false, VertexData, EdgeData, GraphData>(graph);
    }

    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::move_constructible<VertexData> && std::move_constructible<EdgeData> && std::move_constructible<GraphData>
    [[nodiscard]] Conversion::DynamicConversionRes<VertexData, EdgeData, GraphData> Conversion::toDynamicGraph(CSRGraph<VertexData, EdgeData, GraphData> &&graph)
    {
        return toDynamicGraphImpl<true, VertexData, EdgeData, GraphData>(graph);
    }

    template <bool Move, typename VertexData, typename EdgeData, typename GraphData>
    Conversion::DynamicConversionRes<VertexData, EdgeData, GraphData> Conversion::toDynamicGraphImpl(std::conditional_t<Move, CSRGraph<VertexData, EdgeData, GraphData> &, const CSRGraph<VertexData, EdgeData, GraphData> &> graph)
    {
        using CSR = CSRGraph<VertexData, EdgeData, GraphData>;
        using Dynamic = DynamicConversionRes<VertexData, EdgeData, GraphData>;
        using Index = typename CSR::Index;

        // graph data
        using GraphDataRef = std::conditional_t<Move, GraphData &&, const GraphData &>;
        Dynamic dynamicGraph(static_cast<GraphDataRef>(graph.graphData()));

        // vertex data
        std::vector<typename Dynamic::VertexId> vertexIds;
        vertexIds.reserve(graph.vertexCount());

        for (Index vertexIndex = 0; vertexIndex < graph.vertexCount(); ++vertexIndex)
        {
            if constexpr (std::is_same_v<VertexData, byteBool>)
            {
                vertexIds.push_back(dynamicGraph.addVertex(graph.vertexData(vertexIndex).value != 0));
            }
            else if constexpr (Move)
            {
                vertexIds.push_back(dynamicGraph.addVertex(std::move(graph.vertexData(vertexIndex))));
            }
            else
            {
                vertexIds.push_back(dynamicGraph.addVertex(graph.vertexData(vertexIndex)));
            }
        }

        // edge data

        const std::span<const std::size_t> offsets = graph.offsets();
        const std::span<const Index> neighbors = graph.neighborIndices();
        const std::span<const Index> edgeIndices = graph.adjacencyEdgeIndices();

        for (Index vertexIndex = 0; vertexIndex < graph.vertexCount(); ++vertexIndex)
        {
            for (std::size_t adjacencyPosition = offsets[vertexIndex]; adjacencyPosition < offsets[vertexIndex + 1]; ++adjacencyPosition)
            {
                const Index neighborIndex = neighbors[adjacencyPosition];

                if (vertexIndex >= neighborIndex)
                {
                    continue;
                }

                const Index edgeIndex = edgeIndices[adjacencyPosition];

                if constexpr (std::is_same_v<EdgeData, byteBool>)
                {
                    dynamicGraph.addEdge(vertexIds[vertexIndex], vertexIds[neighborIndex], graph.edgeData(edgeIndex).value != 0);
                }
                else if constexpr (Move)
                {
                    dynamicGraph.addEdge(vertexIds[vertexIndex], vertexIds[neighborIndex], std::move(graph.edgeData(edgeIndex)));
                }
                else
                {
                    dynamicGraph.addEdge(vertexIds[vertexIndex], vertexIds[neighborIndex], graph.edgeData(edgeIndex));
                }
            }
        }

        return dynamicGraph;
    }
}
