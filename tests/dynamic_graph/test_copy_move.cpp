
#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/dynamic_graph.hpp>
#include <utility>
#include <concepts>
#include <type_traits>
#include <string>
#include <memory>
#include <span>
#include <vector>
#include <stdexcept>
#include <optional>

/**
 * compile time tests
 */

using DefaultGraph = simple_undirected_graph::DynamicGraph<>;

static_assert(std::copy_constructible<DefaultGraph>, "DynamicGraph<> must be copy-constructible");
static_assert(std::is_copy_assignable_v<DefaultGraph>, "DynamicGraph<> must be copy-assignable");

static_assert(std::move_constructible<DefaultGraph>, "DynamicGraph<> must be move-constructible");
static_assert(std::is_move_assignable_v<DefaultGraph>, "DynamicGraph<> must be move-assignable");

static_assert(std::same_as<decltype(std::declval<DefaultGraph &>() = std::declval<const DefaultGraph &>()), DefaultGraph &>, "copy assignment must return Graph&");
static_assert(std::same_as<decltype(std::declval<DefaultGraph &>() = std::declval<DefaultGraph &&>()), DefaultGraph &>, "move assignment must return Graph&");

static_assert(std::is_nothrow_move_constructible_v<DefaultGraph>, "DynamicGraph<> move construction must be noexcept");
static_assert(std::is_nothrow_move_assignable_v<DefaultGraph>, "DynamicGraph<> move assignment must be noexcept");

using MoveOnlyGraph = simple_undirected_graph::DynamicGraph<std::unique_ptr<int>, std::unique_ptr<int>, std::unique_ptr<int>>;

static_assert(!std::copy_constructible<MoveOnlyGraph>, "a graph with move-only properties must not be copy-constructible");
static_assert(!std::is_copy_assignable_v<MoveOnlyGraph>, "a graph with move-only properties must not be copy-assignable");

static_assert(std::move_constructible<MoveOnlyGraph>, "a graph with move-only properties must be move-constructible");
static_assert(std::is_move_assignable_v<MoveOnlyGraph>, "a graph with move-only properties must be move-assignable");

/**
 * copy construction
 */

TEST_CASE("Dynamic graph: copy constructor copies an empty graph")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph og;

    Graph copy(og);

    CHECK(copy.empty());
    CHECK(copy.vertexCount() == 0);
    CHECK(copy.edgeCount() == 0);
}

TEST_CASE("Dynamic graph: copy constructor copies graph, vertex and edge properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<std::string, std::string, int>;
    Graph og{1};

    const Graph::VertexId a = og.addVertex("vertex a");
    const Graph::VertexId b = og.addVertex("vertex b");

    const std::pair<Graph::EdgeId, bool> ab = og.addEdge(a, b, "edge ab");
    REQUIRE(ab.second);

    Graph copy(og);
    CHECK(copy.graphData() == 1);

    const std::span<const Graph::VertexId> vertices = copy.vertices();
    REQUIRE(vertices.size() == 2);

    bool foundA = false;
    bool foundB = false;

    for (const Graph::VertexId &id : vertices)
    {
        if (copy.vertexData(id) == "vertex a")
        {
            foundA = true;
        }
        else if (copy.vertexData(id) == "vertex b")
        {
            foundB = true;
        }
    }

    CHECK(foundA);
    CHECK(foundB);

    const std::span<const Graph::EdgeId> edges = copy.edges();
    REQUIRE(edges.size() == 1);
    CHECK(copy.edgeData(edges[0]) == "edge ab");
}

TEST_CASE("Dynamic graph: copy constructor preserves topology with new graph-specific IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<int>;
    Graph og;

    const Graph::VertexId a = og.addVertex(1);
    const Graph::VertexId b = og.addVertex(2);
    const Graph::VertexId c = og.addVertex(3);

    const std::pair<Graph::EdgeId, bool> ab = og.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> bc = og.addEdge(b, c);

    REQUIRE(ab.second);
    REQUIRE(bc.second);

    Graph copy(og);
    REQUIRE(copy.vertexCount() == 3);
    REQUIRE(copy.edgeCount() == 2);

    std::optional<Graph::VertexId> copiedA;
    std::optional<Graph::VertexId> copiedB;
    std::optional<Graph::VertexId> copiedC;

    for (const Graph::VertexId &id : copy.vertices())
    {
        if (copy.vertexData(id) == 1)
        {
            copiedA = id;
        }
        else if (copy.vertexData(id) == 2)
        {
            copiedB = id;
        }
        else if (copy.vertexData(id) == 3)
        {
            copiedC = id;
        }
    }

    REQUIRE(copiedA.has_value());
    REQUIRE(copiedB.has_value());
    REQUIRE(copiedC.has_value());

    CHECK_FALSE(copy.vertexExists(a));
    CHECK_FALSE(copy.vertexExists(b));
    CHECK_FALSE(copy.vertexExists(c));

    CHECK_FALSE(og.vertexExists(*copiedA));
    CHECK_FALSE(og.vertexExists(*copiedB));
    CHECK_FALSE(og.vertexExists(*copiedC));

    CHECK(copy.hasEdge(*copiedA, *copiedB));
    CHECK(copy.hasEdge(*copiedB, *copiedC));
    CHECK_FALSE(copy.hasEdge(*copiedA, *copiedC));

    const std::optional<Graph::EdgeId> copiedAB = copy.findEdgeId(*copiedA, *copiedB);
    const std::optional<Graph::EdgeId> copiedBC = copy.findEdgeId(*copiedB, *copiedC);

    REQUIRE(copiedAB.has_value());
    REQUIRE(copiedBC.has_value());

    CHECK_FALSE(og.edgeExists(*copiedAB));
    CHECK_FALSE(og.edgeExists(*copiedBC));
    CHECK_FALSE(copy.edgeExists(ab.first));
    CHECK_FALSE(copy.edgeExists(bc.first));
}

TEST_CASE("Dynamic graph: copy constructor creates an independent graph")
{
    using Graph = simple_undirected_graph::DynamicGraph<int, int, std::string>;
    Graph og{"the og"};

    const Graph::VertexId a = og.addVertex(1);
    const Graph::VertexId b = og.addVertex(2);

    const std::pair<Graph::EdgeId, bool> ab = og.addEdge(a, b, 3);
    REQUIRE(ab.second);

    Graph copy(og);

    REQUIRE(copy.vertices().size() == 2);
    REQUIRE(copy.edges().size() == 1);

    const Graph::VertexId copiedVertex = copy.vertices()[0];
    const Graph::EdgeId copiedEdge = copy.edges()[0];

    copy.graphData() = "just a copy";
    copy.vertexData(copiedVertex) = 4;
    copy.edgeData(copiedEdge) = 5;

    CHECK(og.graphData() == "the og");
    CHECK(og.vertexData(a) == 1);
    CHECK(og.vertexData(b) == 2);
    CHECK(og.edgeData(ab.first) == 3);

    REQUIRE(og.removeVertex(a));

    CHECK(copy.vertexCount() == 2);
    CHECK(copy.edgeCount() == 1);
    CHECK(copy.graphData() == "just a copy");
    CHECK(copy.vertexData(copiedVertex) == 4);
    CHECK(copy.edgeData(copiedEdge) == 5);
}

/**
 * copy assignment
 */

TEST_CASE("Dynamic graph: copy assignment replaces contents and invalidates old IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<int, int, std::string>;
    Graph src{"src"};
    Graph target{"target"};

    const Graph::VertexId a = src.addVertex(1);
    const Graph::VertexId b = src.addVertex(2);
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, 3);
    REQUIRE(ab.second);

    const Graph::VertexId oldA = target.addVertex(4);
    const Graph::VertexId oldB = target.addVertex(5);
    const std::pair<Graph::EdgeId, bool> oldAB = target.addEdge(oldA, oldB, 6);
    REQUIRE(oldAB.second);

    target = src;
    CHECK(target.graphData() == "src");
    REQUIRE(target.vertexCount() == 2);
    REQUIRE(target.edgeCount() == 1);

    CHECK_FALSE(target.vertexExists(oldA));
    CHECK_FALSE(target.vertexExists(oldB));
    CHECK_FALSE(target.edgeExists(oldAB.first));
    CHECK_FALSE(target.vertexExists(a));
    CHECK_FALSE(target.vertexExists(b));
    CHECK_FALSE(target.edgeExists(ab.first));

    bool foundA = false;
    bool foundB = false;

    for (const Graph::VertexId &id : target.vertices())
    {
        if (target.vertexData(id) == 1)
        {
            foundA = true;
        }
        else if (target.vertexData(id) == 2)
        {
            foundB = true;
        }
    }

    CHECK(foundA);
    CHECK(foundB);

    REQUIRE(target.edges().size() == 1);
    CHECK(target.edgeData(target.edges()[0]) == 3);

    CHECK(src.graphData() == "src");
    CHECK(src.vertexExists(a));
    CHECK(src.vertexExists(b));
    CHECK(src.edgeExists(ab.first));
}

TEST_CASE("Dynamic graph: copy assignment creates an independent graph")
{
    using Graph = simple_undirected_graph::DynamicGraph<int, int, std::string>;
    Graph src{"original"};
    Graph target{"target"};

    const Graph::VertexId a = src.addVertex(1);
    const Graph::VertexId b = src.addVertex(2);
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, 3);
    REQUIRE(ab.second);

    target = src;
    REQUIRE(target.vertices().size() == 2);
    REQUIRE(target.edges().size() == 1);

    const Graph::VertexId copiedVertex = target.vertices()[0];
    const Graph::EdgeId copiedEdge = target.edges()[0];

    target.graphData() = "modified";
    target.vertexData(copiedVertex) = 4;
    target.edgeData(copiedEdge) = 5;

    CHECK(src.graphData() == "original");
    CHECK(src.vertexData(a) == 1);
    CHECK(src.vertexData(b) == 2);
    CHECK(src.edgeData(ab.first) == 3);

    REQUIRE(src.removeVertex(a));

    CHECK(target.vertexCount() == 2);
    CHECK(target.edgeCount() == 1);
    CHECK(target.graphData() == "modified");
    CHECK(target.vertexData(copiedVertex) == 4);
    CHECK(target.edgeData(copiedEdge) == 5);
}

TEST_CASE("Dynamic graph: copy assignment handles self-assignment")
{
    using Graph = simple_undirected_graph::DynamicGraph<int, int, int>;
    Graph graph{1};

    const Graph::VertexId a = graph.addVertex(2);
    const Graph::VertexId b = graph.addVertex(3);
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 4);
    REQUIRE(ab.second);

    graph = graph;
    CHECK(graph.graphData() == 1);
    CHECK(graph.vertexCount() == 2);
    CHECK(graph.edgeCount() == 1);

    REQUIRE(graph.vertexExists(a));
    REQUIRE(graph.vertexExists(b));
    REQUIRE(graph.edgeExists(ab.first));

    CHECK(graph.vertexData(a) == 2);
    CHECK(graph.vertexData(b) == 3);
    CHECK(graph.edgeData(ab.first) == 4);
    CHECK(graph.hasEdge(a, b));
}

/**
 * move construction
 */

TEST_CASE("Dynamic graph: move constructor transfers contents and preserves IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<std::string, std::string, std::string>;
    Graph src{"graph"};

    const Graph::VertexId a = src.addVertex("a");
    const Graph::VertexId b = src.addVertex("b");
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, "ab");
    REQUIRE(ab.second);

    Graph moved(std::move(src));
    CHECK(moved.graphData() == "graph");

    REQUIRE(moved.vertexExists(a));
    REQUIRE(moved.vertexExists(b));
    REQUIRE(moved.edgeExists(ab.first));

    CHECK(moved.vertexData(a) == "a");
    CHECK(moved.vertexData(b) == "b");
    CHECK(moved.edgeData(ab.first) == "ab");
    CHECK(moved.hasEdge(a, b));
}

TEST_CASE("Dynamic graph: move constructor leaves the source empty and reusable")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph src;

    const Graph::VertexId a = src.addVertex();
    const Graph::VertexId b = src.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b);
    REQUIRE(ab.second);

    Graph moved(std::move(src));

    CHECK(src.empty());
    CHECK(src.edgeCount() == 0);
    CHECK_FALSE(src.vertexExists(a));
    CHECK_FALSE(src.edgeExists(ab.first));

    const Graph::VertexId newA = src.addVertex();
    const Graph::VertexId newB = src.addVertex();
    const std::pair<Graph::EdgeId, bool> newAB = src.addEdge(newA, newB);

    REQUIRE(newAB.second);

    CHECK(src.vertexCount() == 2);
    CHECK(src.edgeCount() == 1);

    CHECK_FALSE(src.vertexExists(a));
    CHECK_FALSE(src.edgeExists(ab.first));
}

/**
 * move assignment
 */

TEST_CASE("Dynamic graph: move assignment replaces contents and preserves transferred IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<std::string, std::string, std::string>;
    Graph src{"src"};
    Graph target{"target"};

    const Graph::VertexId a = src.addVertex("a");
    const Graph::VertexId b = src.addVertex("b");
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, "ab");
    REQUIRE(ab.second);

    const Graph::VertexId oldA = target.addVertex("old a");
    const Graph::VertexId oldB = target.addVertex("old b");
    const std::pair<Graph::EdgeId, bool> oldAB = target.addEdge(oldA, oldB, "old ab");
    REQUIRE(oldAB.second);

    target = std::move(src);

    CHECK(target.graphData() == "src");

    REQUIRE(target.vertexExists(a));
    REQUIRE(target.vertexExists(b));
    REQUIRE(target.edgeExists(ab.first));

    CHECK(target.vertexData(a) == "a");
    CHECK(target.vertexData(b) == "b");
    CHECK(target.edgeData(ab.first) == "ab");
    CHECK(target.hasEdge(a, b));

    CHECK_FALSE(target.vertexExists(oldA));
    CHECK_FALSE(target.vertexExists(oldB));
    CHECK_FALSE(target.edgeExists(oldAB.first));
}

TEST_CASE("Dynamic graph: move assignment leaves the source empty and reusable")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph src;
    Graph target;

    const Graph::VertexId a = src.addVertex();
    const Graph::VertexId b = src.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b);
    REQUIRE(ab.second);

    target = std::move(src);

    CHECK(src.empty());
    CHECK(src.edgeCount() == 0);
    CHECK_FALSE(src.vertexExists(a));
    CHECK_FALSE(src.edgeExists(ab.first));

    const Graph::VertexId newA = src.addVertex();
    const Graph::VertexId newB = src.addVertex();
    const std::pair<Graph::EdgeId, bool> newAB = src.addEdge(newA, newB);

    REQUIRE(newAB.second);
    CHECK(src.vertexCount() == 2);
    CHECK(src.edgeCount() == 1);
    CHECK_FALSE(src.vertexExists(a));
    CHECK_FALSE(src.edgeExists(ab.first));
}

TEST_CASE("Dynamic graph: move assignment handles self-assignment")
{
    using Graph = simple_undirected_graph::DynamicGraph<int, int, int>;
    Graph graph{9};

    const Graph::VertexId a = graph.addVertex(2);
    const Graph::VertexId b = graph.addVertex(3);
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 4);
    REQUIRE(ab.second);

    graph = std::move(graph);

    CHECK(graph.graphData() == 9);
    CHECK(graph.vertexCount() == 2);
    CHECK(graph.edgeCount() == 1);

    REQUIRE(graph.vertexExists(a));
    REQUIRE(graph.vertexExists(b));
    REQUIRE(graph.edgeExists(ab.first));

    CHECK(graph.vertexData(a) == 2);
    CHECK(graph.vertexData(b) == 3);
    CHECK(graph.edgeData(ab.first) == 4);
    CHECK(graph.hasEdge(a, b));
}

/**
 * move-only properties
 */

TEST_CASE("Dynamic graph: move constructor supports move-only properties")
{
    using Graph = MoveOnlyGraph;
    Graph src{std::make_unique<int>(1)};

    const Graph::VertexId a = src.addVertex(std::make_unique<int>(2));
    const Graph::VertexId b = src.addVertex(std::make_unique<int>(3));
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, std::make_unique<int>(4));
    REQUIRE(ab.second);
    Graph moved(std::move(src));

    REQUIRE(moved.vertexExists(a));
    REQUIRE(moved.vertexExists(b));
    REQUIRE(moved.edgeExists(ab.first));

    REQUIRE(moved.graphData() != nullptr);
    REQUIRE(moved.vertexData(a) != nullptr);
    REQUIRE(moved.vertexData(b) != nullptr);
    REQUIRE(moved.edgeData(ab.first) != nullptr);

    CHECK(*moved.graphData() == 1);
    CHECK(*moved.vertexData(a) == 2);
    CHECK(*moved.vertexData(b) == 3);
    CHECK(*moved.edgeData(ab.first) == 4);
}

TEST_CASE("Dynamic graph: move assignment supports move-only properties")
{
    using Graph = MoveOnlyGraph;
    Graph src{std::make_unique<int>(1)};
    Graph target{std::make_unique<int>(5)};

    const Graph::VertexId a = src.addVertex(std::make_unique<int>(2));
    const Graph::VertexId b = src.addVertex(std::make_unique<int>(3));
    const std::pair<Graph::EdgeId, bool> ab = src.addEdge(a, b, std::make_unique<int>(4));
    REQUIRE(ab.second);

    const Graph::VertexId oldId = target.addVertex(std::make_unique<int>(6));
    target = std::move(src);

    REQUIRE(target.vertexExists(a));
    REQUIRE(target.vertexExists(b));
    REQUIRE(target.edgeExists(ab.first));
    CHECK_FALSE(target.vertexExists(oldId));

    REQUIRE(target.graphData() != nullptr);
    REQUIRE(target.vertexData(a) != nullptr);
    REQUIRE(target.vertexData(b) != nullptr);
    REQUIRE(target.edgeData(ab.first) != nullptr);

    CHECK(*target.graphData() == 1);
    CHECK(*target.vertexData(a) == 2);
    CHECK(*target.vertexData(b) == 3);
    CHECK(*target.edgeData(ab.first) == 4);
}
