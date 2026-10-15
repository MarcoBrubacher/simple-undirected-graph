#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <string>
#include <utility>
#include <concepts>

/**
 *  compile time tests
 */

using defaultGraph = simple_undirected_graph::Graph<>;

static_assert(std::same_as<decltype(std::declval<defaultGraph &>().vertexData(std::declval<defaultGraph::VertexId>())), simple_undirected_graph::NoProperties &>);

/**
 * vertex  creation
 */

TEST_CASE("addVertex without properties")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId id = graph.addVertex();

    CHECK(graph.vertexCount() == 1);
    CHECK(graph.vertexExists(id));
}

TEST_CASE("addVertex by copying the given vertex data")
{
}

TEST_CASE("addVertex by moving the given vertex data")
{
}

TEST_CASE("emplaceVertex by constructing its data directly from the supplied arguments")
{
}

/**
 * existance and count
 */

/**
 * properties
 */

/**
 * removal and reuse
 */

/**
 * additional tests / edge cases
 */