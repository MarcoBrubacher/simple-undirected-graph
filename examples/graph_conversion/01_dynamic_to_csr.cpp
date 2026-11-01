#include <simple_undirected_graph/conversion.hpp>
#include <iostream>
#include <string>

int main()
{
    using DynamicGraph = simple_undirected_graph::DynamicGraph<std::string, std::string, std::string>;
    using CSRGraph = simple_undirected_graph::CSRGraph<std::string, std::string, std::string>;

    DynamicGraph dynamic("dynamic graph");

    DynamicGraph::VertexId vertexA = dynamic.addVertex("a");
    DynamicGraph::VertexId vertexB = dynamic.addVertex("b");
    DynamicGraph::VertexId vertexC = dynamic.addVertex("c");
    dynamic.addEdge(vertexA, vertexB, "a-b");
    dynamic.addEdge(vertexB, vertexC, "b-c");

    // conversion
    CSRGraph csr = simple_undirected_graph::Conversion::toCSRGraph(dynamic);

    // check
    std::cout << "Converted graph: " << csr.graphData() << '\n';
    std::cout << "Vertices: " << csr.vertexCount() << '\n';
    std::cout << "Edges: " << csr.edgeCount() << '\n';
    std::cout << "First vertex: " << csr.vertexData(0) << '\n';
    std::cout << "First edge: " << csr.edgeData(0) << '\n';

    return 0;
}