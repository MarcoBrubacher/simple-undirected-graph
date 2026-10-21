#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <string>
#include <utility>
#include <concepts>
#include <memory>
#include <stdexcept>

/**
 *  compile time tests
 */

using defaultGraph = simple_undirected_graph::Graph<>;

static_assert(std::same_as<decltype(std::declval<defaultGraph &>().vertexData(std::declval<defaultGraph::VertexId>())), simple_undirected_graph::NoProperties &>,"vertexData() must return NoProperties&");

static_assert(std::same_as<decltype(std::declval<const defaultGraph &>().vertexData(std::declval<defaultGraph::VertexId>())), const simple_undirected_graph::NoProperties &>,"vertexData()const must return const NoProperties&");

static_assert(noexcept(std::declval<const defaultGraph &>().vertexExists(std::declval<defaultGraph::VertexId>())),"vertexExists() must be noexcept");

static_assert(noexcept(std::declval<const defaultGraph &>().vertexCount()),"vertexCount() must be noexcept");

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

TEST_CASE("emplaceVertex supports move-only vertex data")
{
    using Graph = simple_undirected_graph::Graph<std::unique_ptr<int>>;
    Graph graph;

    const Graph::VertexId id = graph.emplaceVertex(std::make_unique<int>(73));

    REQUIRE(graph.vertexExists(id));
    REQUIRE(graph.vertexData(id) != nullptr);
    CHECK(*graph.vertexData(id) == 73);
}
/**
 * existence and count
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

    const Graph::VertexId id1 = graph.addVertex();
    CHECK(graph.vertexCount() == 1);

    const Graph::VertexId id2 = graph.addVertex();
    CHECK(graph.vertexCount() == 2);

    REQUIRE(graph.removeVertex(id1));
    CHECK(graph.vertexCount() == 1);

    REQUIRE(graph.removeVertex(id2));
    CHECK(graph.vertexCount() == 0);
}

/**
 * properties
 */

TEST_CASE("vertexData returns stored vertex properties")
{
    using Graph = simple_undirected_graph::Graph<double>;
    Graph graph;

    const Graph::VertexId id = graph.addVertex(20.0);

    REQUIRE(graph.vertexExists(id));
    CHECK(graph.vertexData(id) == 20.0);
}

TEST_CASE("vertexData allows modifying vertex properties")
{
    using Graph = simple_undirected_graph::Graph<double>;
    Graph graph;

    const Graph::VertexId id = graph.addVertex(20.0);

    REQUIRE(graph.vertexExists(id));

    graph.vertexData(id) = 30.0;
    CHECK(graph.vertexData(id) == 30.0);
}

TEST_CASE("vertexData allows reading properties from a const graph")
{
    using Graph = simple_undirected_graph::Graph<double>;
    Graph graph;

    const Graph::VertexId id = graph.addVertex(20.0);

    const Graph &constGraph = graph;

    REQUIRE(constGraph.vertexExists(id));
    CHECK(constGraph.vertexData(id) == 20.0);
}

TEST_CASE("vertexData throws for an invalid vertex ID")
{
    using Graph = simple_undirected_graph::Graph<int>;
    Graph graph;
    const Graph::VertexId id = graph.addVertex(91);

    SECTION("removed vertex ID")
    {
        REQUIRE(graph.removeVertex(id));
        CHECK_THROWS_AS(graph.vertexData(id), std::invalid_argument);
    }

    SECTION("vertex ID from another graph")
    {
        Graph graphB;
        const Graph::VertexId idB = graphB.addVertex(916);

        REQUIRE(graphB.vertexExists(idB));
        CHECK_THROWS_AS(graph.vertexData(idB), std::invalid_argument);
    }

    SECTION("invalid ID on a const graph")
    {
        const Graph &constGraph = graph;

        REQUIRE(graph.removeVertex(id));
        CHECK_THROWS_AS(constGraph.vertexData(id), std::invalid_argument);
    }
}

TEST_CASE("emplaceVertex default-initializes vertex properties")
{
    using Graph = simple_undirected_graph::Graph<int>;
    Graph graph;

    const Graph::VertexId id = graph.emplaceVertex();

    REQUIRE(graph.vertexExists(id));
    CHECK(graph.vertexData(id) == 0);
}

/**
 * removal and reuse
 */

TEST_CASE("removeVertex removes a vertex without affecting other vertices")
{
    using Graph = simple_undirected_graph::Graph<int>;
    Graph graph;

    const Graph::VertexId id1 = graph.addVertex(1);
    const Graph::VertexId id2 = graph.addVertex(2);
    const Graph::VertexId id3 = graph.addVertex(3);

    REQUIRE(graph.removeVertex(id2));
    CHECK_FALSE(graph.vertexExists(id2));
    CHECK(graph.vertexCount() == 2);

    REQUIRE(graph.vertexExists(id1));
    REQUIRE(graph.vertexExists(id3));

    CHECK(graph.vertexData(id1) == 1);
    CHECK(graph.vertexData(id3) == 3);
}

TEST_CASE("removeVertex returns false for invalid vertex IDs")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId id = graph.addVertex();

    SECTION("already removed vertex ID")
    {
        REQUIRE(graph.removeVertex(id));
        CHECK_FALSE(graph.removeVertex(id));
        CHECK(graph.empty());
    }

    SECTION("vertex ID from another graph")
    {
        Graph otherGraph;
        const Graph::VertexId foreignId = otherGraph.addVertex();

        REQUIRE(otherGraph.vertexExists(foreignId));

        CHECK_FALSE(graph.removeVertex(foreignId));
        CHECK(graph.vertexExists(id));
        CHECK(otherGraph.vertexExists(foreignId));
        CHECK(graph.vertexCount() == 1);
    }
}

TEST_CASE("removed vertex IDs remain invalid after adding new vertices")
{
    using Graph = simple_undirected_graph::Graph<int>;
    Graph graph;

    const Graph::VertexId oldId = graph.addVertex(14);

    REQUIRE(graph.removeVertex(oldId));
    CHECK_FALSE(graph.vertexExists(oldId));

    const Graph::VertexId newId = graph.addVertex(24);

    CHECK_FALSE(graph.removeVertex(oldId));
    REQUIRE(graph.vertexExists(newId));
    CHECK_FALSE(graph.vertexExists(oldId));
    CHECK_FALSE(oldId == newId);
    CHECK(graph.vertexData(newId) == 24);
    CHECK(graph.vertexCount() == 1);
}

TEST_CASE("removeVertex supports removing multiple vertices in different positions")
{
    using Graph = simple_undirected_graph::Graph<>;
    Graph graph;

    const Graph::VertexId id1 = graph.addVertex();
    const Graph::VertexId id2 = graph.addVertex();
    const Graph::VertexId id3 = graph.addVertex();
    const Graph::VertexId id4 = graph.addVertex();

    REQUIRE(graph.removeVertex(id2));
    CHECK(graph.vertexCount() == 3);
    REQUIRE(graph.removeVertex(id3));
    CHECK(graph.vertexCount() == 2);
    REQUIRE(graph.vertexExists(id1));
    REQUIRE(graph.vertexExists(id4));
    REQUIRE(graph.removeVertex(id1));
    REQUIRE(graph.removeVertex(id4));

    CHECK(graph.empty());
    CHECK(graph.vertexCount() == 0);
}

TEST_CASE("removeVertex removes incident edges without affecting unrelated edges")
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

    REQUIRE(graph.removeVertex(b));
    CHECK_FALSE(graph.vertexExists(b));

    CHECK_FALSE(graph.edgeExists(ab.first));
    CHECK_FALSE(graph.edgeExists(bc.first));
    CHECK(graph.edgeExists(cd.first));

    CHECK(graph.edgeCount() == 1);
    CHECK(graph.vertexCount() == 3);

    CHECK(graph.vertexExists(a));
    CHECK(graph.vertexExists(c));
    CHECK(graph.vertexExists(d));
}

/**
 * additional tests / edge cases
 */