#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>

/**
 *  graph construction tests
 */

TEST_CASE("newly created graph is empty")
{
    simple_undirected_graph::Graph<> graph;

    CHECK(graph.empty());
    CHECK(graph.vertexCount() == 0);
    CHECK(graph.edgeCount() == 0);
}

/**
 *  graph data tests
 */

/**
 *  graph state tests
 */