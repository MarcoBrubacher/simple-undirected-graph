#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <string>
#include <utility>

/**
 *  graph construction tests
 */

TEST_CASE("default-constructed graph is empty")
{
    simple_undirected_graph::Graph<> graph;

    CHECK(graph.empty());
    CHECK(graph.vertexCount() == 0);
    CHECK(graph.edgeCount() == 0);
}

/**
 *  graph data tests
 */

TEST_CASE("graph constructor stores graph data")
{
    using Graph = simple_undirected_graph::Graph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, std::string>;
    const Graph graph("graph property");

    CHECK(graph.graphData() == "graph property");
}

TEST_CASE("graph data can be modified through a non-const graph")
{
    using Graph = simple_undirected_graph::Graph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, std::string>;
    Graph graph("graph property");

    graph.graphData() = "new graph property";

    CHECK(graph.graphData() == "new graph property");
}

TEST_CASE("default constructor initializes graph properties")
{
    using Graph = simple_undirected_graph::Graph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, int>;
    Graph graph;

    CHECK(graph.graphData() == 0);
}

TEST_CASE("clearing a graph does not change graph properties")
{
    using Graph = simple_undirected_graph::Graph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, double>;

    Graph graph(21.12);
    REQUIRE(graph.graphData() == 21.12);

    graph.addVertex();
    REQUIRE(graph.vertexCount() == 1);

    graph.clear();

    CHECK(graph.graphData() == 21.12);
}

/**
 *  graph state tests
 */

TEST_CASE("adding vertices makes the graph non-empty")
{
    simple_undirected_graph::Graph<> graph;
    graph.addVertex();

    CHECK_FALSE(graph.empty());
    CHECK(graph.vertexCount() == 1);
}

TEST_CASE("clearing an empty graph leaves it empty")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    REQUIRE(graph.empty());

    graph.clear();

    CHECK(graph.empty());
    CHECK(graph.vertexCount() == 0);
    CHECK(graph.edgeCount() == 0);
}

TEST_CASE("clear resets the graph")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId v1 = graph.addVertex();
    const Graph::VertexId v2 = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> e1 = graph.addEdge(v1, v2);

    REQUIRE(e1.second);
    REQUIRE(graph.vertexCount() == 2);
    REQUIRE(graph.edgeCount() == 1);

    graph.clear();

    SECTION("removes all vertices and edges")
    {
        CHECK(graph.empty());
        CHECK(graph.vertexCount() == 0);
        CHECK(graph.edgeCount() == 0);
    }

    SECTION("invalidates existing IDs")
    {
        CHECK_FALSE(graph.vertexExists(v1));
        CHECK_FALSE(graph.vertexExists(v2));
        CHECK_FALSE(graph.edgeExists(e1.first));
    }

    SECTION("allows reuse without revalidating old IDs")
    {
        const Graph::VertexId v1new = graph.addVertex();
        const Graph::VertexId v2new = graph.addVertex();

        const std::pair<Graph::EdgeId, bool> e1new = graph.addEdge(v1new, v2new);
        REQUIRE(e1new.second);

        CHECK(graph.vertexExists(v1new));
        CHECK(graph.vertexExists(v2new));
        CHECK(graph.edgeExists(e1new.first));

        CHECK_FALSE(graph.vertexExists(v1));
        CHECK_FALSE(graph.vertexExists(v2));
        CHECK_FALSE(graph.edgeExists(e1.first));

        CHECK_FALSE(graph.empty());
        CHECK(graph.vertexCount() == 2);
        CHECK(graph.edgeCount() == 1);
    }
}
