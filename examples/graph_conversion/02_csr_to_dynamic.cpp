#include <simple_undirected_graph/conversion.hpp>
#include <iostream>

int main()
{
    using CSRGraph = simple_undirected_graph::CSRGraph<>;
    using DynamicGraph = simple_undirected_graph::DynamicGraph<>;

    CSRGraph csr;

    DynamicGraph dynamic = toDynamicGraph(csr);

    std::cout << "Vertices: " << dynamic.vertexCount() << '\n';
    std::cout << "Edges: " << dynamic.edgeCount() << '\n';

    return 0;
}