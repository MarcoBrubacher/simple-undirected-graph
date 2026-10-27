#include <simple_undirected_graph/csr_graph.hpp>
#include <string>
#include <vector>

int main()
{
    simple_undirected_graph::CSRGraph<> defaultGraph;

    simple_undirected_graph::CSRGraph<int> vertexLabeledGraph;

    simple_undirected_graph::CSRGraph<std::vector<int>, float, double> complexGraph;

    // use byteBool instead of bool to avoid the special behavior of std::vector<bool>
    simple_undirected_graph::CSRGraph<simple_undirected_graph::byteBool, float, std::string> initializedGraph("Is a graph");

    return 0;
}