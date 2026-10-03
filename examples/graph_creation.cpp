#include <simple_undirected_graph.hpp>

#include <string>
#include <vector>

int main()
{
    // creates an empty graph without vertex, edge, or graph properties
    simple_undirected_graph::Graph<> defaultGraph;

    // creates an empty graph with integer vertex properties, while edge and graph properties use the default NoProperties type
    simple_undirected_graph::Graph<int> vertexLabeledGraph;

    // creates an empty graph with custom vertex, edge, and graph property types
    simple_undirected_graph::Graph<std::vector<int>, float, double> complexGraph;

    // creates a graph and initializes its graph-level property directly
    simple_undirected_graph::Graph<bool, float, std::string> initializedGraph("Is a graph");

    return 0;
}