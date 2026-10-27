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

}
