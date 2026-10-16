#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <string>
#include <utility>
#include <concepts>
#include <memory>

/**
 *  compile time tests
 */

using defaultGraph = simple_undirected_graph::Graph<>;

static_assert(std::same_as<decltype(std::declval<defaultGraph &>().vertexData(std::declval<defaultGraph::VertexId>())), simple_undirected_graph::NoProperties &>,
              "vertexData() must return NoProperties&");

static_assert(std::same_as<decltype(std::declval<const defaultGraph &>().vertexData(std::declval<defaultGraph::VertexId>())), const simple_undirected_graph::NoProperties &>,
              "vertexData()const must return const NoProperties&");

static_assert(noexcept(std::declval<const defaultGraph &>().vertexExists(std::declval<defaultGraph::VertexId>())),
              "vertexExists() must be noexcept");

static_assert(noexcept(std::declval<const defaultGraph &>().vertexCount()),
              "vertexCount() must be noexcept");

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
    using Graph = simple_undirected_graph::Graph<std::string>;
    Graph graph;

    std::string og = "original string";
    const Graph::VertexId id = graph.addVertex(og);

    REQUIRE(graph.vertexCount() == 1);
    REQUIRE(graph.vertexExists(id));

    CHECK(og == "original string");
    CHECK(graph.vertexData(id) == "original string");

    og = "modified string";
    CHECK(graph.vertexData(id) == "original string");
}

TEST_CASE("addVertex by moving the given vertex data")
{
    using Graph = simple_undirected_graph::Graph<std::string>;
    Graph graph;

    std::string og = "original string";
    const Graph::VertexId id = graph.addVertex(std::move(og));

    REQUIRE(graph.vertexCount() == 1);
    REQUIRE(graph.vertexExists(id));

    og = "modified string";

    CHECK(graph.vertexData(id) == "original string");
}

TEST_CASE("emplaceVertex by constructing its data directly from the supplied arguments")
{
    struct VertexProperties
    {
        int weight;
        std::string name;
        VertexProperties(int weight, std::string name) : weight(weight), name(std::move(name)) {}
    };

    using Graph = simple_undirected_graph::Graph<VertexProperties>;
    Graph graph;

    const Graph::VertexId id = graph.emplaceVertex(18, "vertex uno");

    REQUIRE(graph.vertexExists(id));
    CHECK(graph.vertexCount() == 1);

    CHECK(graph.vertexData(id).weight == 18);
    CHECK(graph.vertexData(id).name == "vertex uno");
}

TEST_CASE("addVertex supports move-only vertex data")
{
    using Graph = simple_undirected_graph::Graph<std::unique_ptr<int>>;
    Graph graph;

    std::unique_ptr<int> upOg = std::make_unique<int>(5);

    const Graph::VertexId id = graph.addVertex(std::move(upOg));

    REQUIRE(graph.vertexCount() == 1);
    REQUIRE(graph.vertexExists(id));

    REQUIRE(graph.vertexData(id) != nullptr);
    CHECK(upOg == nullptr);

    CHECK(*graph.vertexData(id) == 5);
}

/**
 * existance and count
 */

TEST_CASE("vertexExists correctly identifies existing vertices")
{
    using Graph = simple_undirected_graph::Graph<>;

    Graph graphA;

    const Graph::VertexId idA1 = graphA.addVertex();
    CHECK(graphA.vertexExists(idA1));

    const Graph::VertexId idA2 = graphA.addVertex();
    CHECK(graphA.vertexExists(idA1));
    CHECK(graphA.vertexExists(idA2));

    graphA.removeVertex(idA1);

    CHECK_FALSE(graphA.vertexExists(idA1));
    CHECK(graphA.vertexExists(idA2));

    Graph graphB;

    const Graph::VertexId idB1 = graphB.addVertex();

    CHECK(graphB.vertexExists(idB1));
    CHECK_FALSE(graphA.vertexExists(idB1));
    CHECK_FALSE(graphB.vertexExists(idA2));
}

TEST_CASE("vertexCount tracks the number of existing vertices")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    CHECK(graph.vertexCount() == 0);

    const Graph::VertexId v1 = graph.addVertex();
    CHECK(graph.vertexCount() == 1);

    const Graph::VertexId v2 = graph.addVertex();
    CHECK(graph.vertexCount() == 2);

    REQUIRE(graph.removeVertex(v1));
    CHECK(graph.vertexCount() == 1);

    REQUIRE(graph.removeVertex(v2));
    CHECK(graph.vertexCount() == 0);
}

/**
 * properties
 */

/**
 * removal and reuse
 */

/**
 * additional tests / edge cases
 */