
#include <simple_undirected_graph/csr_graph.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    using Graph = simple_undirected_graph::CSRGraph<std::string, std::string, std::string>;

    std::string graphData = "my graph";
    std::vector<std::string> vertexData = {"0", "1", "2", "3", "4"};
    std::vector<std::string> edgeData = {"0", "1", "2", "3", "4"};

    std::vector<std::uint32_t> neighbors_ = {1, 2, 0, 3, 0, 3, 1, 2, 4, 3};
    std::vector<std::uint32_t> edgeIndices_ = {0, 1, 0, 2, 1, 3, 2, 3, 4, 4};
    std::vector<std::size_t> offsets_ = {0, 2, 4, 6, 9, 10};

    if (!Graph::isValidCSRFormat(5, 5, offsets_, neighbors_, edgeIndices_))
    {
        std::cerr << "Invalid CSR format" << '\n';
        return 1;
    }

    Graph graph = Graph::fromValidCSRFormat(std::move(graphData), std::move(vertexData), std::move(edgeData), std::move(offsets_), std::move(neighbors_), std::move(edgeIndices_));

    // modify individual properties
    graph.graphData() = "updated graph";
    graph.vertexData(1) = "updated vertex 1";
    graph.edgeData(0) = "updated edge 0";

    std::cout << "Graph data: " << graph.graphData() << '\n';
    std::cout << "Vertex 1 data: " << graph.vertexData(1) << '\n';
    std::cout << "Edge 0 data: " << graph.edgeData(0) << '\n';

    // modify multiple properties
    std::vector<std::string> newVertexData = {"a", "b", "c", "d", "e"};
    std::vector<std::string> newEdgeData = {"v", "w", "x", "y", "z"};

    if (newVertexData.size() != graph.vertexCount() || newEdgeData.size() != graph.edgeCount())
    {
        std::cerr << "invalid property array sizes" << '\n';
        return 1;
    }

    std::copy(newVertexData.begin(), newVertexData.end(), graph.vertexData().begin());
    std::copy(newEdgeData.begin(), newEdgeData.end(), graph.edgeData().begin());

    for (std::size_t i = 0; i < graph.vertexCount(); ++i)
    {
        std::cout << "Vertex " << i << " data: " << graph.vertexData()[i] << '\n';
    }

    for (std::size_t i = 0; i < graph.edgeCount(); ++i)
    {
        std::cout << "Edge " << i << " data: " << graph.edgeData()[i] << '\n';
    }
}
