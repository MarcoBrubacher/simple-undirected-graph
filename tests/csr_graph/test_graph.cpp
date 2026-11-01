#include <catch2/catch_test_macros.hpp>
#include <csr_graph/include/csr_graph_fixture.hpp>
#include <concepts>
#include <span>
#include <string>
#include <utility>

using Graph = csr_graph_test::Graph;
using DefaultGraph = simple_undirected_graph::CSRGraph<>;

// graph data types
static_assert(std::same_as<decltype(std::declval<DefaultGraph &>().graphData()), simple_undirected_graph::NoProperties &>);
static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().graphData()), const simple_undirected_graph::NoProperties &>);
static_assert(std::same_as<decltype(std::declval<Graph &>().graphData()), std::string &>);
static_assert(std::same_as<decltype(std::declval<const Graph &>().graphData()), const std::string &>);

// vertex data types
static_assert(std::same_as<decltype(std::declval<Graph &>().vertexData()), std::span<double>>);
static_assert(std::same_as<decltype(std::declval<const Graph &>().vertexData()), std::span<const double>>);

// edge data types
static_assert(std::same_as<decltype(std::declval<Graph &>().edgeData()), std::span<csr_graph_test::ByteBool>>);
static_assert(std::same_as<decltype(std::declval<const Graph &>().edgeData()), std::span<const csr_graph_test::ByteBool>>);

// noexcept guarantees
static_assert(noexcept(std::declval<const Graph &>().empty()));
static_assert(noexcept(std::declval<const Graph &>().vertexCount()));
static_assert(noexcept(std::declval<const Graph &>().edgeCount()));
static_assert(noexcept(std::declval<Graph &>().graphData()));
static_assert(noexcept(std::declval<const Graph &>().graphData()));

/**
 * graph construction tests
 */

TEST_CASE("CSR graph: default constructed CSR graph is empty")
{
    const Graph graph;

    CHECK(graph.empty());
    CHECK(graph.vertexCount() == 0);
    CHECK(graph.edgeCount() == 0);

    REQUIRE(graph.offsets().size() == 1);
    REQUIRE(graph.offsets()[0] == 0);

    CHECK(graph.neighborIndices().empty());
    CHECK(graph.adjacencyEdgeIndices().empty());
}

/**
 * graph data tests
 */

TEST_CASE("CSR graph: graph constructor stores graph data")
{
    const Graph graph("graph property");
    CHECK(graph.graphData() == "graph property");
}

TEST_CASE("CSR graph: graph data can be modified through a non-const graph")
{
    Graph graph("graph property");
    graph.graphData() = "new graph property";
    CHECK(graph.graphData() == "new graph property");
}

TEST_CASE("CSR graph: default constructor initializes graph properties")
{
    using init = simple_undirected_graph::CSRGraph<simple_undirected_graph::NoProperties, simple_undirected_graph::NoProperties, int>;
    const init graph;
    CHECK(graph.graphData() == 0);
}

TEST_CASE("CSR graph: constructing a graph from CSR format preserves graph data")
{
    const Graph graph = csr_graph_test::constructGraph();
    CHECK(graph.graphData() == csr_graph_test::graphProperty);
}

/**
 * graph state tests
 */

TEST_CASE("CSR graph: constructed CSR graph has the expected state")
{
    const Graph graph = csr_graph_test::constructGraph();

    CHECK_FALSE(graph.empty());
    CHECK(graph.vertexCount() == csr_graph_test::vertexCount);
    CHECK(graph.edgeCount() == csr_graph_test::edgeCount);
}

TEST_CASE("CSR graph: vertexExists and edgeExists handle valid and invalid IDs")
{
    const Graph graph = csr_graph_test::constructGraph();

    for (Graph::VertexId id = 0; id < csr_graph_test::vertexCount; ++id)
    {
        CHECK(graph.vertexExists(id));
    }
    CHECK_FALSE(graph.vertexExists(csr_graph_test::vertexCount));

    for (Graph::EdgeId id = 0; id < csr_graph_test::edgeCount; ++id)
    {
        CHECK(graph.edgeExists(id));
    }
    CHECK_FALSE(graph.edgeExists(csr_graph_test::edgeCount));
}

/**
 * vertex data tests
 */

TEST_CASE("CSR graph: vertex data can be accessed and modified")
{
    Graph graph = csr_graph_test::constructGraph();
    REQUIRE(graph.vertexData().size() == csr_graph_test::vertexCount);

    for (Graph::VertexId id = 0; id < csr_graph_test::vertexCount; ++id)
    {
        CHECK(graph.vertexData(id) == csr_graph_test::vertexProperties[id]);
    }

    graph.vertexData(0) = 7.7; // through individual access
    CHECK(graph.vertexData(0) == 7.7);
    CHECK(graph.vertexData()[0] == 7.7);

    graph.vertexData()[5] = 3.1; // through span
    CHECK(graph.vertexData(5) == 3.1);
    CHECK(graph.vertexData(1) == csr_graph_test::vertexProperties[1]);

    const Graph &constGraph = graph;
    CHECK(constGraph.vertexData(0) == 7.7);
    CHECK(constGraph.vertexData()[5] == 3.1);
}

/**
 * edge data tests
 */

TEST_CASE("CSR graph: edge data can be accessed and modified")
{
    Graph graph = csr_graph_test::constructGraph();
    REQUIRE(graph.edgeData().size() == csr_graph_test::edgeCount);

    for (Graph::EdgeId id = 0; id < csr_graph_test::edgeCount; ++id)
    {
        CHECK(graph.edgeData(id).value == csr_graph_test::edgeProperties[id].value);
    }

    graph.edgeData(0) = csr_graph_test::ByteBool{0};
    CHECK(graph.edgeData(0).value == 0);
    CHECK(graph.edgeData()[0].value == 0);

    graph.edgeData()[1] = csr_graph_test::ByteBool{1};
    CHECK(graph.edgeData(1).value == 1);
    CHECK(graph.edgeData(2).value == csr_graph_test::edgeProperties[2].value);

    const Graph &constGraph = graph;
    CHECK(constGraph.edgeData(0).value == 0);
    CHECK(constGraph.edgeData()[1].value == 1);
}

/**
 * additional tests / edge cases
 */

TEST_CASE("CSR graph: CSR graph with isolated vertices is not empty")
{
    const Graph graph = Graph::fromValidCSRFormat("isolated vertices",
                                                  {1.0, 2.0, 3.0},
                                                  {},
                                                  {0, 0, 0, 0},
                                                  {},
                                                  {});

    CHECK_FALSE(graph.empty());
    CHECK(graph.vertexCount() == 3);
    CHECK(graph.edgeCount() == 0);
}