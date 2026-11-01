#include <catch2/catch_test_macros.hpp>
#include <csr_graph/include/csr_graph_fixture.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <concepts>
#include <optional>
#include <utility>

using Graph = csr_graph_test::Graph;

static_assert(std::same_as<decltype(std::declval<Graph &>().neighbors(0)), std::span<const Graph::Index>>);
static_assert(std::same_as<decltype(std::declval<Graph &>().incidentEdgeIndices(0)), std::span<const Graph::Index>>);
static_assert(std::same_as<decltype(std::declval<const Graph &>().findEdgeId(0, 1)), std::optional<Graph::EdgeId>>);

TEST_CASE("CSR graph: vertex degrees are correct")
{
    const Graph graph = csr_graph_test::constructGraph();
    const std::array<std::size_t, csr_graph_test::vertexCount> deg = {3, 2, 1, 2, 2, 0};

    for (Graph::VertexId id = 0; id < csr_graph_test::vertexCount; ++id)
    {
        CHECK(graph.degree(id) == deg[id]);
    }
}

TEST_CASE("CSR graph: neighbors and incident edge indices are correct")
{
    const Graph graph = csr_graph_test::constructGraph();

    for (Graph::VertexId id = 0; id < csr_graph_test::vertexCount; ++id)
    {
        const std::size_t start = csr_graph_test::offsets[id];
        const std::size_t count = csr_graph_test::offsets[id + 1] - start;
        CHECK(std::ranges::equal(graph.neighbors(id), std::span(csr_graph_test::neighbors).subspan(start, count)));
        CHECK(std::ranges::equal(graph.incidentEdgeIndices(id), std::span(csr_graph_test::edgeIndices).subspan(start, count)));
    }
}

TEST_CASE("CSR graph: edges can be found in both directions")
{
    const Graph graph = csr_graph_test::constructGraph();

    for (Graph::EdgeId id = 0; id < csr_graph_test::edgeCount; ++id)
    {
        const Graph::VertexId u = csr_graph_test::expectedEndpoints[id].first;
        const Graph::VertexId v = csr_graph_test::expectedEndpoints[id].second;

        CHECK(graph.findEdgeId(u, v) == id);
        CHECK(graph.findEdgeId(v, u) == id);
        CHECK(graph.hasEdge(u, v));
        CHECK(graph.hasEdge(v, u));
    }
}

TEST_CASE("CSR graph: missing edges and self-loops are not found")
{
    const Graph graph = csr_graph_test::constructGraph();
    const std::array<std::pair<Graph::VertexId, Graph::VertexId>, 6> missingEdges = {{{0, 4},
                                                                                      {4, 0},
                                                                                      {1, 2},
                                                                                      {2, 5},
                                                                                      {0, 0},
                                                                                      {5, 5}}};

    for (const std::pair<Graph::VertexId, Graph::VertexId> &edge : missingEdges)
    {
        CHECK_FALSE(graph.findEdgeId(edge.first, edge.second).has_value());
        CHECK_FALSE(graph.hasEdge(edge.first, edge.second));
    }
}

TEST_CASE("CSR graph: edge endpoints are reconstructed correctly")
{
    const Graph graph = csr_graph_test::constructGraph();

    const std::vector<std::pair<Graph::VertexId, Graph::VertexId>> endpoints = graph.reconstructEdgeEndpoints();
    REQUIRE(endpoints.size() == csr_graph_test::edgeCount);

    CHECK(std::ranges::equal(endpoints, csr_graph_test::expectedEndpoints));

    const Graph empty;
    CHECK(empty.reconstructEdgeEndpoints().empty());
}