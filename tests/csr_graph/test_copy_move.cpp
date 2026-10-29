#include <catch2/catch_test_macros.hpp>
#include <csr_graph/include/csr_graph_fixture.hpp>
#include <algorithm>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

using DefaultGraph = simple_undirected_graph::CSRGraph<>;

static_assert(std::copy_constructible<DefaultGraph>, "CSRGraph<> must be copy-constructible");
static_assert(std::is_copy_assignable_v<DefaultGraph>, "CSRGraph<> must be copy-assignable");

static_assert(std::move_constructible<DefaultGraph>, "CSRGraph<> must be move-constructible");
static_assert(std::is_move_assignable_v<DefaultGraph>, "CSRGraph<> must be move-assignable");

static_assert(std::same_as<decltype(std::declval<DefaultGraph &>() = std::declval<const DefaultGraph &>()), DefaultGraph &>, "copy assignment must return Graph&");
static_assert(std::same_as<decltype(std::declval<DefaultGraph &>() = std::declval<DefaultGraph &&>()), DefaultGraph &>, "move assignment must return Graph&");

static_assert(std::is_nothrow_move_constructible_v<DefaultGraph>, "CSRGraph<> move construction must be noexcept");
static_assert(std::is_nothrow_move_assignable_v<DefaultGraph>, "CSRGraph<> move assignment must be noexcept");

using MoveOnlyGraph = simple_undirected_graph::CSRGraph<std::unique_ptr<int>, std::unique_ptr<int>, std::unique_ptr<int>>;

static_assert(!std::copy_constructible<MoveOnlyGraph>, "a graph with move-only properties must not be copy-constructible");
static_assert(!std::is_copy_assignable_v<MoveOnlyGraph>, "a graph with move-only properties must not be copy-assignable");

static_assert(std::move_constructible<MoveOnlyGraph>, "a graph with move-only properties must be move-constructible");
static_assert(std::is_move_assignable_v<MoveOnlyGraph>, "a graph with move-only properties must be move-assignable");

using Graph = csr_graph_test::Graph;

/**
 * copy construction
 */

TEST_CASE("CSR graph: copy constructor copies an empty graph")
{
    const Graph og;
    Graph copy(og);

    CHECK(copy.empty());
    CHECK(copy.vertexCount() == 0);
    CHECK(copy.edgeCount() == 0);

    REQUIRE(copy.offsets().size() == 1);
    CHECK(copy.offsets()[0] == 0);

    CHECK(copy.neighborIndices().empty());
    CHECK(copy.adjacencyEdgeIndices().empty());
}

TEST_CASE("CSR graph: copy constructor preserves properties/topology")
{
    const Graph og = csr_graph_test::constructGraph();
    Graph copy(og);

    CHECK(copy.graphData() == csr_graph_test::graphProperty);
    REQUIRE(copy.vertexCount() == csr_graph_test::vertexCount);
    REQUIRE(copy.edgeCount() == csr_graph_test::edgeCount);

    CHECK(std::ranges::equal(copy.offsets(), csr_graph_test::offsets));
    CHECK(std::ranges::equal(copy.neighborIndices(), csr_graph_test::neighbors));
    CHECK(std::ranges::equal(copy.adjacencyEdgeIndices(), csr_graph_test::edgeIndices));

    for (Graph::VertexId id = 0; id < csr_graph_test::vertexCount; ++id)
    {
        CHECK(copy.vertexData(id) == csr_graph_test::vertexProperties[id]);
    }

    for (Graph::EdgeId id = 0; id < csr_graph_test::edgeCount; ++id)
    {
        CHECK(copy.edgeData(id).value == csr_graph_test::edgeProperties[id].value);
    }
}

TEST_CASE("CSR graph: copy constructor creates an independent graph")
{
    Graph og = csr_graph_test::constructGraph();
    Graph copy(og);

    copy.graphData() = "modified copy";
    copy.vertexData(0) = 1.2;
    copy.edgeData(0) = csr_graph_test::ByteBool{0};

    CHECK(og.graphData() == csr_graph_test::graphProperty);
    CHECK(og.vertexData(0) == csr_graph_test::vertexProperties[0]);
    CHECK(og.edgeData(0).value == csr_graph_test::edgeProperties[0].value);

    og.graphData() = "modified original";
    og.vertexData(1) = 2.1;
    og.edgeData(1) = csr_graph_test::ByteBool{1};

    CHECK(copy.graphData() == "modified copy");
    CHECK(copy.vertexData(1) == csr_graph_test::vertexProperties[1]);
    CHECK(copy.edgeData(1).value == csr_graph_test::edgeProperties[1].value);
}

/**
 * move construction
 */
TEST_CASE("CSR graph: move constructor transfers content")
{
    Graph og = csr_graph_test::constructGraph();
    Graph moved(std::move(og));

    CHECK(moved.graphData() == csr_graph_test::graphProperty);

    REQUIRE(moved.vertexCount() == csr_graph_test::vertexCount);
    REQUIRE(moved.edgeCount() == csr_graph_test::edgeCount);

    CHECK(std::ranges::equal(moved.offsets(), csr_graph_test::offsets));
    CHECK(std::ranges::equal(moved.neighborIndices(), csr_graph_test::neighbors));
    CHECK(std::ranges::equal(moved.adjacencyEdgeIndices(), csr_graph_test::edgeIndices));
    CHECK(moved.vertexData(0) == csr_graph_test::vertexProperties[0]);
    CHECK(moved.edgeData(0).value == csr_graph_test::edgeProperties[0].value);
}

/**
 * move assignment
 */

TEST_CASE("CSR graph: move assignment replaces existing content")
{
    Graph og = csr_graph_test::constructGraph();

    Graph tar = Graph::fromValidCSRFormat("old graph",
                                          {1.0},
                                          {},
                                          {0, 0},
                                          {},
                                          {});

    tar = std::move(og);
    CHECK(tar.graphData() == csr_graph_test::graphProperty);

    REQUIRE(tar.vertexCount() == csr_graph_test::vertexCount);
    REQUIRE(tar.edgeCount() == csr_graph_test::edgeCount);

    CHECK(std::ranges::equal(tar.offsets(), csr_graph_test::offsets));
    CHECK(std::ranges::equal(tar.neighborIndices(), csr_graph_test::neighbors));
    CHECK(std::ranges::equal(tar.adjacencyEdgeIndices(), csr_graph_test::edgeIndices));

    CHECK(tar.vertexData(0) == csr_graph_test::vertexProperties[0]);
    CHECK(tar.edgeData(0).value == csr_graph_test::edgeProperties[0].value);
}

TEST_CASE("CSR graph: self-move-assignment preserves the graph")
{
    Graph graph = csr_graph_test::constructGraph();
    Graph &same = graph;

    graph = std::move(same);
    CHECK(graph.graphData() == csr_graph_test::graphProperty);

    REQUIRE(graph.vertexCount() == csr_graph_test::vertexCount);
    REQUIRE(graph.edgeCount() == csr_graph_test::edgeCount);

    CHECK(std::ranges::equal(graph.offsets(), csr_graph_test::offsets));
    CHECK(std::ranges::equal(graph.neighborIndices(), csr_graph_test::neighbors));
    CHECK(std::ranges::equal(graph.adjacencyEdgeIndices(), csr_graph_test::edgeIndices));

    CHECK(graph.vertexData(0) == csr_graph_test::vertexProperties[0]);
    CHECK(graph.edgeData(0).value == csr_graph_test::edgeProperties[0].value);
}

/**
 * move-only properties
 */
TEST_CASE("CSR graph: move construction and assignment support move-only properties")
{
    using Graph = simple_undirected_graph::CSRGraph<std::unique_ptr<std::string>, std::unique_ptr<std::string>, std::unique_ptr<std::string>>;

    std::vector<std::unique_ptr<std::string>> vertices;
    vertices.push_back(std::make_unique<std::string>("vertex a"));
    vertices.push_back(std::make_unique<std::string>("vertex b"));

    std::vector<std::unique_ptr<std::string>> edges;
    edges.push_back(std::make_unique<std::string>("edge ab"));

    Graph og = Graph::fromValidCSRFormat(std::make_unique<std::string>("graph x"),
                                         std::move(vertices),
                                         std::move(edges),
                                         {0, 1, 2},
                                         {1, 0},
                                         {0, 0});

    Graph moved(std::move(og));

    REQUIRE(moved.vertexCount() == 2);
    REQUIRE(moved.edgeCount() == 1);
    REQUIRE(moved.graphData() != nullptr);
    REQUIRE(moved.vertexData(0) != nullptr);
    REQUIRE(moved.edgeData(0) != nullptr);

    CHECK(*moved.graphData() == "graph x");
    CHECK(*moved.vertexData(0) == "vertex a");
    CHECK(*moved.edgeData(0) == "edge ab");

    Graph tar(std::make_unique<std::string>("old graph"));
    tar = std::move(moved);

    REQUIRE(tar.vertexCount() == 2);
    REQUIRE(tar.edgeCount() == 1);
    REQUIRE(tar.graphData() != nullptr);
    REQUIRE(tar.vertexData(0) != nullptr);
    REQUIRE(tar.edgeData(0) != nullptr);

    CHECK(*tar.graphData() == "graph x");
    CHECK(*tar.vertexData(0) == "vertex a");
    CHECK(*tar.edgeData(0) == "edge ab");
}