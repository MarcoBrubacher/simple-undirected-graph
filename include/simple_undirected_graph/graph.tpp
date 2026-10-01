#include <utility>

template <typename VertexData, typename EdgeData, typename GraphData>
Graph<VertexData, EdgeData, GraphData>::Graph(GraphData graphData) : graphData_(std::move(graphData))
{
}

template <typename VertexData, typename EdgeData, typename GraphData>
GraphData &Graph<VertexData, EdgeData, GraphData>::graphData()
{
    return graphData_;
}

template <typename VertexData, typename EdgeData, typename GraphData>
const GraphData &Graph<VertexData, EdgeData, GraphData>::graphData() const
{
    return graphData_;
}

template <typename VertexData, typename EdgeData, typename GraphData>
typename Graph<VertexData, EdgeData, GraphData>::VertexId Graph<VertexData, EdgeData, GraphData>::addVertex(VertexData vertexData)
{
    // create a vertex with initial data, empty adjacency list and set its existence to true
    // move the supplied data into the stored vertex to avoid an unnecessary copy
    Vertex v{std::move(vertexData), {}, true};

    // determine ID of the new vertex
    VertexId id = vertices_.size();

    // add vertex to the list
    vertices_.push_back(std::move(v));

    // increase vertex count after pus_back, in case if push_back throws
    ++vertexCount_;

    return id;
}