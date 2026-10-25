#include <simple_undirected_graph/dynamic_graph.hpp>

#include <string>
#include <vector>

int main()
{
    simple_undirected_graph::DynamicGraph<> defaultGraph;

    simple_undirected_graph::DynamicGraph<int> vertexLabeledGraph;

    simple_undirected_graph::DynamicGraph<std::vector<int>, float, double> complexGraph;

    simple_undirected_graph::DynamicGraph<bool, float, std::string> initializedGraph("Is a graph");

    return 0;
}