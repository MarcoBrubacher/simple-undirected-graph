#include <simple_undirected_graph/conversion.hpp>
#include <iostream>

int main()
{
    using DynamicGraph = simple_undirected_graph::DynamicGraph<>;
    using CSRGraph = simple_undirected_graph::CSRGraph<>;

    DynamicGraph dynamic;

    CSRGraph csr = toCSRGraph(dynamic);

    std::cout << "Vertices: " << dynamic.vertexCount() << '\n';
    std::cout << "Edges: " << dynamic.edgeCount() << '\n';

    return 0;
}