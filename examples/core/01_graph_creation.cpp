#include <simple_undirected_graph/graph.hpp>

#include <string>
#include <vector>

int main()
{
    simple_undirected_graph::Graph<> defaultGraph;

    simple_undirected_graph::Graph<int> vertexLabeledGraph;

    simple_undirected_graph::Graph<std::vector<int>, float, double> complexGraph;

    simple_undirected_graph::Graph<bool, float, std::string> initializedGraph("Is a graph");

    return 0;
}