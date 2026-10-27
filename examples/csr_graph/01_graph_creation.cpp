#include <simple_undirected_graph/csr_graph.hpp>

int main()
{
    simple_undirected_graph::CSRGraph<> defaultGraph;

    simple_undirected_graph::CSRGraph<int> vertexLabeledGraph;

    simple_undirected_graph::CSRGraph<std::vector<int>, float, double> complexGraph;

    simple_undirected_graph::CSRGraph<bool, float, std::string> initializedGraph("Is a graph");

    return 0;
}