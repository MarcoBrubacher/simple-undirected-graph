
#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/dynamic_graph.hpp>
#include <utility>
#include <concepts>
#include <span>
#include <cstddef>
#include <stdexcept>

/**
 * compile time tests
 */

using DefaultGraph = simple_undirected_graph::DynamicGraph<>;

static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().degree(std::declval<DefaultGraph::VertexId>())), std::size_t>, "degree() must return std::size_t");

static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().adjacency(std::declval<DefaultGraph::VertexId>())), std::span<const DefaultGraph::Adjacency>>, "adjacency() must return std::span<const Adjacency>");

static_assert(std::same_as<decltype(std::declval<DefaultGraph &>().adjacency(std::declval<DefaultGraph::VertexId>())), std::span<const DefaultGraph::Adjacency>>, "adjacency() must provide read-only adjacency entries");

/**
 * degree
 */

TEST_CASE("Dynamic graph: isolated vertices have degree zero")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    CHECK(graph.degree(a) == 0);
    CHECK(graph.degree(b) == 0);
}

TEST_CASE("Dynamic graph: degree keeps track of the number of incident edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    CHECK(graph.degree(a) == 1);
    CHECK(graph.degree(b) == 1);
    CHECK(graph.degree(c) == 0);

    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    REQUIRE(bc.second);

    CHECK(graph.degree(a) == 1);
    CHECK(graph.degree(b) == 2);
    CHECK(graph.degree(c) == 1);

    REQUIRE(graph.removeEdge(ab.first));

    CHECK(graph.degree(a) == 0);
    CHECK(graph.degree(b) == 1);
    CHECK(graph.degree(c) == 1);
}

/**
 * adjacency
 */

TEST_CASE("Dynamic graph: adjacency stores the neighbor and edge ID at both endpoints")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    const std::span<const Graph::Adjacency> adjacentB = graph.adjacency(b);

    REQUIRE(adjacentA.size() == 1);
    CHECK(adjacentA[0].neighbor == b);
    CHECK(adjacentA[0].edge == ab.first);

    REQUIRE(adjacentB.size() == 1);
    CHECK(adjacentB[0].neighbor == a);
    CHECK(adjacentB[0].edge == ab.first);
}

TEST_CASE("Dynamic graph: adjacency identifies all neighbors regardless of their order")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId d = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> ac = graph.addEdge(a, c);
    const std::pair<Graph::EdgeId, bool> ad = graph.addEdge(a, d);

    REQUIRE(ab.second);
    REQUIRE(ac.second);
    REQUIRE(ad.second);

    const std::span<const Graph::Adjacency> neighbors = graph.adjacency(a);
    REQUIRE(neighbors.size() == 3);

    bool foundB = false, foundC = false, foundD = false;

    for (const Graph::Adjacency &entry : neighbors)
    {
        if (entry.neighbor == b && entry.edge == ab.first)
        {
            foundB = true;
        }
        else if (entry.neighbor == c && entry.edge == ac.first)
        {
            foundC = true;
        }
        else if (entry.neighbor == d && entry.edge == ad.first)
        {
            foundD = true;
        }
    }
    CHECK(foundB);
    CHECK(foundC);
    CHECK(foundD);
}

TEST_CASE("Dynamic graph: adjacency can be read from a const graph")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const Graph &constGraph = graph;
    const std::span<const Graph::Adjacency> neighbors = constGraph.adjacency(a);

    REQUIRE(neighbors.size() == 1);
    CHECK(neighbors[0].neighbor == b);
    CHECK(neighbors[0].edge == ab.first);
}

TEST_CASE("Dynamic graph: adding a duplicate edge does not duplicate adjacency entries")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const std::pair<Graph::EdgeId, bool> duplicate = graph.addEdge(b, a);
    CHECK_FALSE(duplicate.second);

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    const std::span<const Graph::Adjacency> adjacentB = graph.adjacency(b);

    REQUIRE(adjacentA.size() == 1);
    REQUIRE(adjacentB.size() == 1);

    CHECK(adjacentA[0].neighbor == b);
    CHECK(adjacentA[0].edge == ab.first);
    CHECK(adjacentB[0].neighbor == a);
    CHECK(adjacentB[0].edge == ab.first);
}

/**
 * removal and updates
 */

TEST_CASE("Dynamic graph: removeEdge updates adjacency at both endpoints")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> ac = graph.addEdge(a, c);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);

    REQUIRE(ab.second);
    REQUIRE(ac.second);
    REQUIRE(bc.second);
    REQUIRE(graph.removeEdge(ab.first));

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    const std::span<const Graph::Adjacency> adjacentB = graph.adjacency(b);

    REQUIRE(adjacentA.size() == 1);
    REQUIRE(adjacentB.size() == 1);

    CHECK(adjacentA[0].neighbor == c);
    CHECK(adjacentA[0].edge == ac.first);
    CHECK(adjacentB[0].neighbor == c);
    CHECK(adjacentB[0].edge == bc.first);
    CHECK(graph.degree(c) == 2);
}

TEST_CASE("Dynamic graph: removing edges preserves adjacency after reordering")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId d = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> ac = graph.addEdge(a, c);
    const std::pair<Graph::EdgeId, bool> ad = graph.addEdge(a, d);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    const std::pair<Graph::EdgeId, bool> bd = graph.addEdge(b, d);

    REQUIRE(ab.second);
    REQUIRE(ac.second);
    REQUIRE(ad.second);
    REQUIRE(bc.second);
    REQUIRE(bd.second);

    REQUIRE(graph.removeEdge(ab.first));
    REQUIRE(graph.removeEdge(ad.first));
    REQUIRE(graph.removeEdge(bd.first));

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    const std::span<const Graph::Adjacency> adjacentB = graph.adjacency(b);

    REQUIRE(adjacentA.size() == 1);
    REQUIRE(adjacentB.size() == 1);

    CHECK(adjacentA[0].neighbor == c);
    CHECK(adjacentA[0].edge == ac.first);
    CHECK(adjacentB[0].neighbor == c);
    CHECK(adjacentB[0].edge == bc.first);
    CHECK(graph.degree(c) == 2);
    CHECK(graph.degree(d) == 0);
}

TEST_CASE("Dynamic graph: removeVertex updates adjacency of its former neighbors")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId d = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    const std::pair<Graph::EdgeId, bool> bd = graph.addEdge(b, d);
    const std::pair<Graph::EdgeId, bool> ac = graph.addEdge(a, c);

    REQUIRE(ab.second);
    REQUIRE(bc.second);
    REQUIRE(bd.second);
    REQUIRE(ac.second);
    REQUIRE(graph.removeVertex(b));

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    const std::span<const Graph::Adjacency> adjacentC = graph.adjacency(c);

    REQUIRE(adjacentA.size() == 1);
    REQUIRE(adjacentC.size() == 1);

    CHECK(adjacentA[0].neighbor == c);
    CHECK(adjacentA[0].edge == ac.first);
    CHECK(adjacentC[0].neighbor == a);
    CHECK(adjacentC[0].edge == ac.first);
    CHECK(graph.adjacency(d).empty());
    CHECK(graph.degree(d) == 0);
}

TEST_CASE("Dynamic graph: reusing a vertex slot does not restore stale adjacency entries")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId oldNeighbor = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> oldEdge = graph.addEdge(a, oldNeighbor);
    const std::pair<Graph::EdgeId, bool> ac = graph.addEdge(a, c);

    REQUIRE(oldEdge.second);
    REQUIRE(ac.second);
    REQUIRE(graph.removeVertex(oldNeighbor));

    const Graph::VertexId newNeighbor = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> newEdge = graph.addEdge(a, newNeighbor);
    REQUIRE(newEdge.second);

    const std::span<const Graph::Adjacency> adjacentA = graph.adjacency(a);
    REQUIRE(adjacentA.size() == 2);

    bool foundC = false;
    bool foundNewNeighbor = false;

    for (const Graph::Adjacency &entry : adjacentA)
    {
        CHECK_FALSE(entry.neighbor == oldNeighbor);
        CHECK_FALSE(entry.edge == oldEdge.first);

        if (entry.neighbor == c && entry.edge == ac.first)
        {
            foundC = true;
        }
        else if (entry.neighbor == newNeighbor && entry.edge == newEdge.first)
        {
            foundNewNeighbor = true;
        }
    }

    CHECK(foundC);
    CHECK(foundNewNeighbor);
}

/**
 * invalid operations
 */

TEST_CASE("Dynamic graph: degree throws for invalid vertex IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();

    SECTION("removed vertex ID")
    {
        REQUIRE(graph.removeVertex(a));
        CHECK_THROWS_AS(graph.degree(a), std::invalid_argument);
    }
    SECTION("vertex ID from another graph")
    {
        Graph otherGraph;
        const Graph::VertexId foreignId = otherGraph.addVertex();

        REQUIRE(otherGraph.vertexExists(foreignId));
        CHECK_THROWS_AS(graph.degree(foreignId), std::invalid_argument);
    }
}

TEST_CASE("Dynamic graph: adjacency throws for invalid vertex IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph &constGraph = graph;

    SECTION("removed vertex ID")
    {
        REQUIRE(graph.removeVertex(a));
        CHECK_THROWS_AS(constGraph.adjacency(a), std::invalid_argument);
    }

    SECTION("vertex ID from another graph")
    {
        Graph otherGraph;
        const Graph::VertexId foreignId = otherGraph.addVertex();

        REQUIRE(otherGraph.vertexExists(foreignId));
        CHECK_THROWS_AS(constGraph.adjacency(foreignId), std::invalid_argument);
    }
}
