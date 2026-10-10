#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <string>

TEST_CASE("default graph uses NoProperties")
{
    simple_undirected_graph::Graph<> graph;

    static_assert(std::same_as<decltype(graph.graphData()), simple_undirected_graph::NoProperties &>);
}