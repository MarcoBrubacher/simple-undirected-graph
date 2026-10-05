#include <simple_undirected_graph/graph.hpp>

#include <string>
#include <vector>

int main()
{
    // graph property combinations //
    simple_undirected_graph::Graph<> defaultGraph;

    simple_undirected_graph::Graph<int> vertexLabeledGraph;

    simple_undirected_graph::Graph<std::vector<int>, float, double> complexGraph;

    // initialized graph data //
    simple_undirected_graph::Graph<bool, float, std::string> initializedGraph("Is a graph");

    return 0;
}