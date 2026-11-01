#include <simple_undirected_graph/conversion.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    using CSRGraph = simple_undirected_graph::CSRGraph<std::string, std::string, std::string>;
    using DynamicGraph = simple_undirected_graph::DynamicGraph<std::string, std::string, std::string>;

    std::vector<std::string> vertexData = {"a", "b", "c"};
    std::vector<std::string> edgeData = {"a-b", "b-c"};
    std::vector<std::size_t> offsets = {0, 1, 3, 4};
    std::vector<CSRGraph::Index> neighbors = {1, 0, 2, 1};
    std::vector<CSRGraph::Index> edgeIndices = {0, 0, 1, 1};

    if (!CSRGraph::isValidCSRFormat(3, 2, offsets, neighbors, edgeIndices))
    {
        std::cerr << "Invalid CSR format\n";
        return 1;
    }

    CSRGraph csr = CSRGraph::fromValidCSRFormat("csr graph", std::move(vertexData), std::move(edgeData), std::move(offsets), std::move(neighbors), std::move(edgeIndices));

    // conversion
    DynamicGraph dynamic = simple_undirected_graph::Conversion::toDynamicGraph(std::move(csr));

    // check
    std::cout << "Converted graph: " << dynamic.graphData() << '\n';
    std::cout << "Vertices: " << dynamic.vertexCount() << '\n';
    std::cout << "Edges: " << dynamic.edgeCount() << '\n';
    std::cout << "First vertex: " << dynamic.vertexData(dynamic.vertices()[0]) << '\n';
    std::cout << "First edge: " << dynamic.edgeData(dynamic.edges()[0]) << '\n';

    return 0;
}