#include <simple_undirected_graph/csr_graph.hpp>
#include <iostream>
#include <utility>

int main()
{
    using Graph = simple_undirected_graph::CSRGraph<std::string, std::string, std::string>;

    // graph topology:
    //  (v1)---e0---(v0)---e1---(v2)

    std::string graphData = "my graph";
    std::vector<std::string> vertexData = {"0", "1", "2"};
    std::vector<std::string> edgeData = {"0", "2"};

    std::vector<std::uint32_t> neighbors_ = {1, 2, 0, 0};
    std::vector<std::uint32_t> edgeIndices_ = {0, 1, 0, 1};
    std::vector<std::size_t> offsets_ = {0, 2, 3, 4};

    if (!Graph::isValidCSRFormat(3, 2, offsets_, neighbors_, edgeIndices_))
    {
        std::cerr << "Invalid CSR format\n";
        return 1;
    }

    Graph graph = Graph::fromValidCSRFormat(std::move(graphData), std::move(vertexData), std::move(edgeData), std::move(offsets_), std::move(neighbors_), std::move(edgeIndices_));

    std::cout << "Vertices: " << graph.vertexCount() << '\n';
    std::cout << "Edges: " << graph.edgeCount() << '\n';

    for (std::size_t u = 0; u < graph.vertexCount(); ++u)
    {
        const Graph::VertexId id = static_cast<Graph::VertexId>(u);
        std::cout << "Vertex " << id << " degree: " << graph.degree(id) << '\n';

        const std::span<const Graph::Index> neighbors = graph.neighbors(id);
        const std::span<const Graph::Index> edges = graph.incidentEdgeIndices(id);
        for (std::size_t i = 0; i < neighbors.size(); ++i)
        {
            std::cout << "  Neighbor: " << neighbors[i] << " via edge: " << edges[i] << '\n';
        }
    }

    std::cout << "Edge (0, 1): " << std::boolalpha << graph.hasEdge(0, 1) << '\n';
    std::cout << "Edge (1, 2): " << std::boolalpha << graph.hasEdge(1, 2) << '\n';

    const std::vector<std::pair<Graph::VertexId, Graph::VertexId>> endpoints = graph.reconstructEdgeEndpoints();
    for (std::size_t e = 0; e < endpoints.size(); ++e)
    {
        std::cout << "Edge " << e << ": " << endpoints[e].first << "---" << endpoints[e].second << '\n';
    }
}