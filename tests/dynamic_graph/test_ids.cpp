#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/dynamic_graph.hpp>
#include <utility>
#include <concepts>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cstddef>

/**
 * compile time tests
 */

using DefaultGraph = simple_undirected_graph::DynamicGraph<>;

static_assert(std::equality_comparable<DefaultGraph::VertexId>, "VertexId must support equality comparison");

static_assert(std::equality_comparable<DefaultGraph::EdgeId>, "EdgeId must support equality comparison");

static_assert(!std::default_initializable<DefaultGraph::VertexId>, "VertexId must not be default-constructible");

static_assert(!std::default_initializable<DefaultGraph::EdgeId>, "EdgeId must not be default-constructible");

static_assert(std::same_as<decltype(std::declval<const DefaultGraph::VertexIdHash &>()(std::declval<const DefaultGraph::VertexId &>())), std::size_t>, "VertexIdHash must return std::size_t");

static_assert(std::same_as<decltype(std::declval<const DefaultGraph::EdgeIdHash &>()(std::declval<const DefaultGraph::EdgeId &>())), std::size_t>, "EdgeIdHash must return std::size_t");

static_assert(noexcept(std::declval<const DefaultGraph::VertexIdHash &>()(std::declval<const DefaultGraph::VertexId &>())), "VertexIdHash must be noexcept");

static_assert(noexcept(std::declval<const DefaultGraph::EdgeIdHash &>()(std::declval<const DefaultGraph::EdgeId &>())), "EdgeIdHash must be noexcept");

/**
 * ID equality and uniqueness
 */

TEST_CASE("Dynamic graph: vertex IDs distinguish different existing vertices")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();
    const Graph::VertexId copyOfA = a;

    CHECK(a == copyOfA);
    CHECK_FALSE(a == b);
    CHECK_FALSE(a == c);
    CHECK_FALSE(b == c);
    CHECK(graph.vertexExists(a));
    CHECK(graph.vertexExists(b));
    CHECK(graph.vertexExists(c));
}

TEST_CASE("Dynamic graph: edge IDs distinguish different existing edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    REQUIRE(ab.second);
    REQUIRE(bc.second);

    const Graph::EdgeId copyOfAB = ab.first;

    CHECK(ab.first == copyOfAB);
    CHECK_FALSE(ab.first == bc.first);
    CHECK(graph.edgeExists(ab.first));
    CHECK(graph.edgeExists(bc.first));
}

/**
 * graph ownership
 */

TEST_CASE("Dynamic graph: vertex IDs belong to their graph instance")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graphA;
    Graph graphB;

    const Graph::VertexId idA = graphA.addVertex();
    const Graph::VertexId idB = graphB.addVertex();

    CHECK_FALSE(idA == idB);
    CHECK(graphA.vertexExists(idA));
    CHECK(graphB.vertexExists(idB));
    CHECK_FALSE(graphA.vertexExists(idB));
    CHECK_FALSE(graphB.vertexExists(idA));
}

TEST_CASE("Dynamic graph: edge IDs belong to their graph instance")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graphA;
    Graph graphB;

    const Graph::VertexId a = graphA.addVertex();
    const Graph::VertexId b = graphA.addVertex();
    const Graph::VertexId x = graphB.addVertex();
    const Graph::VertexId y = graphB.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graphA.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> xy = graphB.addEdge(x, y);
    REQUIRE(ab.second);
    REQUIRE(xy.second);

    CHECK_FALSE(ab.first == xy.first);
    CHECK(graphA.edgeExists(ab.first));
    CHECK(graphB.edgeExists(xy.first));
    CHECK_FALSE(graphA.edgeExists(xy.first));
    CHECK_FALSE(graphB.edgeExists(ab.first));
}

/**
 * generation and reuse
 */

TEST_CASE("Dynamic graph: reused vertex slots keep old vertex IDs invalid")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    Graph::VertexId currentId = graph.addVertex();
    std::vector<Graph::VertexId> oldIds;

    for (int i = 0; i < 12; ++i)
    {
        oldIds.push_back(currentId);
        REQUIRE(graph.removeVertex(currentId));

        currentId = graph.addVertex();
        REQUIRE(graph.vertexExists(currentId));
        CHECK(graph.vertexCount() == 1);

        for (const Graph::VertexId &oldId : oldIds)
        {
            CHECK_FALSE(graph.vertexExists(oldId));
            CHECK_FALSE(oldId == currentId);
            CHECK_FALSE(graph.removeVertex(oldId));
        }
    }

    CHECK(graph.vertexExists(currentId));
}

TEST_CASE("Dynamic graph: reused edge slots keep old edge IDs invalid")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> initial = graph.addEdge(a, b);
    REQUIRE(initial.second);

    Graph::EdgeId currentId = initial.first;
    std::vector<Graph::EdgeId> oldIds;

    for (int i = 0; i < 16; ++i)
    {
        oldIds.push_back(currentId);
        REQUIRE(graph.removeEdge(currentId));

        const std::pair<Graph::EdgeId, bool> next = graph.addEdge(a, b);
        REQUIRE(next.second);
        currentId = next.first;

        REQUIRE(graph.edgeExists(currentId));
        CHECK(graph.edgeCount() == 1);

        for (const Graph::EdgeId &oldId : oldIds)
        {
            CHECK_FALSE(graph.edgeExists(oldId));
            CHECK_FALSE(oldId == currentId);
            CHECK_FALSE(graph.removeEdge(oldId));
        }
    }

    CHECK(graph.edgeExists(currentId));
}

TEST_CASE("Dynamic graph: clear invalidates vertex and edge IDs even after new insertions")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    graph.clear();

    CHECK_FALSE(graph.vertexExists(a));
    CHECK_FALSE(graph.vertexExists(b));
    CHECK_FALSE(graph.edgeExists(ab.first));

    const Graph::VertexId newA = graph.addVertex();
    const Graph::VertexId newB = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> newAB = graph.addEdge(newA, newB);
    REQUIRE(newAB.second);

    CHECK_FALSE(a == newA);
    CHECK_FALSE(b == newB);

    CHECK_FALSE(graph.vertexExists(a));
    CHECK_FALSE(graph.vertexExists(b));

    CHECK_FALSE(ab.first == newAB.first);
    CHECK_FALSE(graph.edgeExists(ab.first));

    CHECK(graph.vertexExists(newA));
    CHECK(graph.vertexExists(newB));
    CHECK(graph.edgeExists(newAB.first));
}

/**
 * hashing
 */

TEST_CASE("Dynamic graph: VertexIdHash supports IDs as unordered set keys")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;
    Graph otherGraph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId foreignId = otherGraph.addVertex();

    std::unordered_set<Graph::VertexId, Graph::VertexIdHash> ids;
    CHECK(ids.insert(a).second);
    CHECK(ids.insert(b).second);
    CHECK(ids.insert(foreignId).second);
    CHECK_FALSE(ids.insert(a).second);
    CHECK(ids.size() == 3);

    const Graph::VertexId copyOfA = a;
    CHECK(ids.contains(copyOfA));
    CHECK(ids.contains(b));
    CHECK(ids.contains(foreignId));

    REQUIRE(graph.removeVertex(a));
    const Graph::VertexId newA = graph.addVertex();

    CHECK_FALSE(ids.contains(newA));
    CHECK(ids.contains(a));
    CHECK_FALSE(graph.vertexExists(a));
    CHECK(graph.vertexExists(newA));
}

TEST_CASE("Dynamic graph: EdgeIdHash supports IDs as unordered map keys")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    REQUIRE(ab.second);
    REQUIRE(bc.second);

    std::unordered_map<Graph::EdgeId, int, Graph::EdgeIdHash> values;
    CHECK(values.emplace(ab.first, 18).second);
    CHECK(values.emplace(bc.first, 3).second);
    CHECK_FALSE(values.emplace(ab.first, 99).second);

    REQUIRE(values.contains(ab.first));
    REQUIRE(values.contains(bc.first));
    CHECK(values.at(ab.first) == 18);
    CHECK(values.at(bc.first) == 3);
    CHECK(values.size() == 2);

    REQUIRE(graph.removeEdge(ab.first));
    const std::pair<Graph::EdgeId, bool> newAB = graph.addEdge(a, b);
    REQUIRE(newAB.second);

    CHECK_FALSE(values.contains(newAB.first));
    CHECK(values.contains(ab.first));
    CHECK_FALSE(graph.edgeExists(ab.first));
    CHECK(graph.edgeExists(newAB.first));
}

/**
 * copied graphs
 */

TEST_CASE("Dynamic graph: copying a graph gives copied vertices and edges new graph-specific IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph source;

    const Graph::VertexId a = source.addVertex();
    const Graph::VertexId b = source.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = source.addEdge(a, b);
    REQUIRE(ab.second);

    Graph copy(source);

    CHECK_FALSE(copy.vertexExists(a));
    CHECK_FALSE(copy.vertexExists(b));
    CHECK_FALSE(copy.edgeExists(ab.first));

    REQUIRE(copy.vertices().size() == 2);
    REQUIRE(copy.edges().size() == 1);

    for (const Graph::VertexId &copiedId : copy.vertices())
    {
        CHECK(copy.vertexExists(copiedId));
        CHECK_FALSE(source.vertexExists(copiedId));
        CHECK_FALSE(copiedId == a);
        CHECK_FALSE(copiedId == b);
    }

    const Graph::EdgeId copiedEdge = copy.edges()[0];
    CHECK(copy.edgeExists(copiedEdge));
    CHECK_FALSE(source.edgeExists(copiedEdge));
    CHECK_FALSE(copiedEdge == ab.first);
}
