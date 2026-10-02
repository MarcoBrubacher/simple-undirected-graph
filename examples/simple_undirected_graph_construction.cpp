#include <simple_undirected_graph/graph.hpp>
#include <iostream>

int main()
{
    using Graph = simple_undirected_graph::Graph<int>;

    Graph graph;

    Graph::VertexId id = graph.addVertex(42);

    std::cout << "graphId:    " << id.graphId << '\n';
    std::cout << "slot index: " << id.index << '\n';
    std::cout << "generation: " << id.generation << '\n';

    return 0;
}