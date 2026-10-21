#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <utility>
#include <concepts>
#include <span>

/**
 * compile time tests
 */

using DefaultGraph = simple_undirected_graph::Graph<>;

static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().vertices()), std::span<const DefaultGraph::VertexId>>, "vertices() must return std::span<const VertexId>");

static_assert(noexcept(std::declval<const DefaultGraph &>().vertices()), "vertices() must be noexcept");

static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().edges()), std::span<const DefaultGraph::EdgeId>>, "edges() must return std::span<const EdgeId>");

static_assert(noexcept(std::declval<const DefaultGraph &>().edges()), "edges() must be noexcept");

/**
 * empty ranges
 */

TEST_CASE("vertices and edges return empty ranges for an empty graph")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    CHECK(graph.vertices().empty());
    CHECK(graph.edges().empty());
}

/**
 * vertex iteration
 */

TEST_CASE("vertices returns all existing vertex IDs regardless of order")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::span<const Graph::VertexId> ids = graph.vertices();
    REQUIRE(ids.size() == 3);

    bool foundA = false;
    bool foundB = false;
    bool foundC = false;

    for (const Graph::VertexId &id : ids)
    {
        if (id == a)
        {
            foundA = true;
        }
        else if (id == b)
        {
            foundB = true;
        }
        else if (id == c)
        {
            foundC = true;
        }
    }

    CHECK(foundA);
    CHECK(foundB);
    CHECK(foundC);
}

TEST_CASE("vertices excludes removed IDs and includes newly added vertices")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    REQUIRE(graph.removeVertex(b));

    const Graph::VertexId d = graph.addVertex();
    const std::span<const Graph::VertexId> ids = graph.vertices();
    REQUIRE(ids.size() == 3);

    bool foundA = false;
    bool foundC = false;
    bool foundD = false;

    for (const Graph::VertexId &id : ids)
    {
        CHECK_FALSE(id == b);

        if (id == a)
        {
            foundA = true;
        }
        else if (id == c)
        {
            foundC = true;
        }
        else if (id == d)
        {
            foundD = true;
        }
    }

    CHECK(foundA);
    CHECK(foundC);
    CHECK(foundD);
}

/**
 * edge iteration
 */

TEST_CASE("edges returns all existing edge IDs regardless of order")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId d = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    const std::pair<Graph::EdgeId, bool> cd = graph.addEdge(c, d);

    REQUIRE(ab.second);
    REQUIRE(bc.second);
    REQUIRE(cd.second);

    const std::span<const Graph::EdgeId> ids = graph.edges();
    REQUIRE(ids.size() == 3);

    bool foundAB = false;
    bool foundBC = false;
    bool foundCD = false;

    for (const Graph::EdgeId &id : ids)
    {
        if (id == ab.first)
        {
            foundAB = true;
        }
        else if (id == bc.first)
        {
            foundBC = true;
        }
        else if (id == cd.first)
        {
            foundCD = true;
        }
    }

    CHECK(foundAB);
    CHECK(foundBC);
    CHECK(foundCD);
}

TEST_CASE("edges excludes removed IDs and includes newly added edges")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId d = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    const std::pair<Graph::EdgeId, bool> cd = graph.addEdge(c, d);

    REQUIRE(ab.second);
    REQUIRE(bc.second);
    REQUIRE(cd.second);
    REQUIRE(graph.removeEdge(bc.first));

    const std::pair<Graph::EdgeId, bool> ad = graph.addEdge(a, d);
    REQUIRE(ad.second);

    const std::span<const Graph::EdgeId> ids = graph.edges();
    REQUIRE(ids.size() == 3);

    bool foundAB = false;
    bool foundCD = false;
    bool foundAD = false;

    for (const Graph::EdgeId &id : ids)
    {
        CHECK_FALSE(id == bc.first);

        if (id == ab.first)
        {
            foundAB = true;
        }
        else if (id == cd.first)
        {
            foundCD = true;
        }
        else if (id == ad.first)
        {
            foundAD = true;
        }
    }

    CHECK(foundAB);
    CHECK(foundCD);
    CHECK(foundAD);
}

/**
 * const iteration and clear
 */

TEST_CASE("vertices and edges can be iterated from a const graph")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const Graph &constGraph = graph;

    const std::span<const Graph::VertexId> vertexIds = constGraph.vertices();
    const std::span<const Graph::EdgeId> edgeIds = constGraph.edges();

    REQUIRE(vertexIds.size() == 2);
    REQUIRE(edgeIds.size() == 1);

    const bool validVertices = (vertexIds[0] == a && vertexIds[1] == b) || (vertexIds[0] == b && vertexIds[1] == a);
    CHECK(validVertices);
    CHECK(edgeIds[0] == ab.first);
}

TEST_CASE("vertices and edges return empty ranges after clear")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    graph.clear();

    CHECK(graph.vertices().empty());
    CHECK(graph.edges().empty());
}
