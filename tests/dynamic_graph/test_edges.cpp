#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/dynamic_graph.hpp>
#include <utility>
#include <string>
#include <memory>
#include <concepts>
#include <optional>
#include <stdexcept>

/**
 * compile time tests
 */

using DefaultGraph = simple_undirected_graph::DynamicGraph<>;

static_assert(std::same_as<decltype(std::declval<DefaultGraph &>().edgeData(std::declval<DefaultGraph::EdgeId>())), simple_undirected_graph::NoProperties &>, "edgeData() must return NoProperties&");

static_assert(std::same_as<decltype(std::declval<const DefaultGraph &>().edgeData(std::declval<DefaultGraph::EdgeId>())), const simple_undirected_graph::NoProperties &>, "edgeData() const must return const NoProperties&");

static_assert(noexcept(std::declval<const DefaultGraph &>().edgeExists(std::declval<DefaultGraph::EdgeId>())), "edgeExists() must be noexcept");

static_assert(noexcept(std::declval<const DefaultGraph &>().edgeCount()), "edgeCount() must be noexcept");

static_assert(std::same_as<decltype(std::declval<DefaultGraph &>().addEdge(std::declval<DefaultGraph::VertexId>(), std::declval<DefaultGraph::VertexId>())), std::pair<DefaultGraph::EdgeId, bool>>, "addEdge() must return std::pair<EdgeId, bool>"); //

/**
 * edge creation
 */

TEST_CASE("addEdge without properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    CHECK(graph.edgeCount() == 1);
    CHECK(graph.edgeExists(ab.first));
}

TEST_CASE("addEdge by copying the given edge data")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, std::string>;
    Graph graph;
    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    std::string original = "original string";
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, original);

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));

    CHECK(original == "original string");
    CHECK(graph.edgeData(ab.first) == "original string");

    original = "modified string";
    CHECK(graph.edgeData(ab.first) == "original string");
}

TEST_CASE("addEdge by moving the given edge data")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, std::string>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    std::string original = "original string";
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, std::move(original));

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));
    CHECK(graph.edgeData(ab.first) == "original string");

    original = "modified string";
    CHECK(graph.edgeData(ab.first) == "original string");
}

TEST_CASE("emplaceEdge by constructing its data directly from the supplied arguments")
{
    struct EdgeProperties
    {
        int weight;
        std::string name;
        EdgeProperties(int weight, std::string name) : weight(weight), name(std::move(name)) {}
    };

    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, EdgeProperties>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = graph.emplaceEdge(a, b, 18, "edge uno");

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));
    CHECK(graph.edgeCount() == 1);
    CHECK(graph.edgeData(ab.first).weight == 18);
    CHECK(graph.edgeData(ab.first).name == "edge uno");
}

TEST_CASE("addEdge supports move-only edge data")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, std::unique_ptr<int>>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    std::unique_ptr<int> upOg = std::make_unique<int>(5);
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, std::move(upOg));

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));

    CHECK(upOg == nullptr);
    REQUIRE(graph.edgeData(ab.first) != nullptr);
    CHECK(*graph.edgeData(ab.first) == 5);
}

TEST_CASE("emplaceEdge supports move-only edge data")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, std::unique_ptr<int>>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.emplaceEdge(a, b, std::make_unique<int>(73));

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));
    REQUIRE(graph.edgeData(ab.first) != nullptr);
    CHECK(*graph.edgeData(ab.first) == 73);
}

/**
 * existence and count
 */

TEST_CASE("edgeExists correctly identifies existing edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graphA;

    const Graph::VertexId a = graphA.addVertex();
    const Graph::VertexId b = graphA.addVertex();
    const Graph::VertexId c = graphA.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graphA.addEdge(a, b);
    REQUIRE(ab.second);

    CHECK(graphA.edgeExists(ab.first));

    const std::pair<Graph::EdgeId, bool> bc = graphA.addEdge(b, c);
    REQUIRE(bc.second);

    CHECK(graphA.edgeExists(ab.first));
    CHECK(graphA.edgeExists(bc.first));
    REQUIRE(graphA.removeEdge(ab.first));
    CHECK_FALSE(graphA.edgeExists(ab.first));
    CHECK(graphA.edgeExists(bc.first));

    Graph graphB;

    const Graph::VertexId x = graphB.addVertex();
    const Graph::VertexId y = graphB.addVertex();

    const std::pair<Graph::EdgeId, bool> xy = graphB.addEdge(x, y);
    REQUIRE(xy.second);
    CHECK(graphB.edgeExists(xy.first));
    CHECK_FALSE(graphA.edgeExists(xy.first));
    CHECK_FALSE(graphB.edgeExists(bc.first));
}

TEST_CASE("edgeCount tracks the number of existing edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    CHECK(graph.edgeCount() == 0);

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);
    CHECK(graph.edgeCount() == 1);

    const std::pair<Graph::EdgeId, bool> bc = graph.addEdge(b, c);
    REQUIRE(bc.second);
    CHECK(graph.edgeCount() == 2);

    REQUIRE(graph.removeEdge(ab.first));
    CHECK(graph.edgeCount() == 1);
    REQUIRE(graph.removeEdge(bc.first));
    CHECK(graph.edgeCount() == 0);
}

/**
 * edge properties
 */

TEST_CASE("edgeData returns stored edge properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, double>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 20.0);
    REQUIRE(ab.second);

    REQUIRE(graph.edgeExists(ab.first));
    CHECK(graph.edgeData(ab.first) == 20.0);
}

TEST_CASE("edgeData allows modifying edge properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, double>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 20.0);

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));

    graph.edgeData(ab.first) = 30.0;
    CHECK(graph.edgeData(ab.first) == 30.0);
}

TEST_CASE("edgeData allows reading properties from a const graph")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, double>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 20.0);

    const Graph &constGraph = graph;
    REQUIRE(ab.second);
    REQUIRE(constGraph.edgeExists(ab.first));
    CHECK(constGraph.edgeData(ab.first) == 20.0);
}

TEST_CASE("edgeData throws for an invalid edge ID")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, int>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b, 91);
    REQUIRE(ab.second);

    SECTION("removed edge ID")
    {
        REQUIRE(graph.removeEdge(ab.first));
        CHECK_THROWS_AS(graph.edgeData(ab.first), std::invalid_argument);
    }

    SECTION("edge ID from another graph")
    {
        Graph graphB;
        const Graph::VertexId x = graphB.addVertex();
        const Graph::VertexId y = graphB.addVertex();

        const std::pair<Graph::EdgeId, bool> xy = graphB.addEdge(x, y, 916);

        REQUIRE(xy.second);
        REQUIRE(graphB.edgeExists(xy.first));
        CHECK_THROWS_AS(graph.edgeData(xy.first), std::invalid_argument);
    }

    SECTION("invalid edge ID on a const graph")
    {
        const Graph &constGraph = graph;

        REQUIRE(graph.removeEdge(ab.first));
        CHECK_THROWS_AS(constGraph.edgeData(ab.first), std::invalid_argument);
    }
}

TEST_CASE("emplaceEdge value-initializes edge properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, int>;
    Graph graph;
    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.emplaceEdge(a, b);

    REQUIRE(ab.second);
    REQUIRE(graph.edgeExists(ab.first));
    CHECK(graph.edgeData(ab.first) == 0);
}

/**
 * edge lookup and endpoints
 */

TEST_CASE("hasEdge identifies existing connections regardless of endpoint order")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    CHECK_FALSE(graph.hasEdge(a, b));

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    CHECK(graph.hasEdge(a, b));
    CHECK(graph.hasEdge(b, a));
    CHECK_FALSE(graph.hasEdge(a, c));

    REQUIRE(graph.removeEdge(ab.first));
    CHECK_FALSE(graph.hasEdge(a, b));
}

TEST_CASE("findEdgeId finds edges regardless of endpoint order")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const Graph::VertexId c = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const std::optional<Graph::EdgeId> foundAB = graph.findEdgeId(a, b);
    const std::optional<Graph::EdgeId> foundBA = graph.findEdgeId(b, a);

    REQUIRE(foundAB.has_value());
    REQUIRE(foundBA.has_value());

    CHECK(*foundAB == ab.first);
    CHECK(*foundBA == ab.first);
    CHECK_FALSE(graph.findEdgeId(a, c).has_value());

    REQUIRE(graph.removeEdge(ab.first));
    CHECK_FALSE(graph.findEdgeId(a, b).has_value());
}

TEST_CASE("endpoints returns both vertices of an edge")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    const std::pair<Graph::VertexId, Graph::VertexId> result = graph.endpoints(ab.first);
    const bool validEndpoints = (result.first == a && result.second == b) || (result.first == b && result.second == a);
    CHECK(validEndpoints);
}

/**
 * removal and reuse
 */

TEST_CASE("removeEdge removes only the selected edge and preserves vertices")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
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

    CHECK_FALSE(graph.edgeExists(bc.first));
    CHECK(graph.edgeExists(ab.first));
    CHECK(graph.edgeExists(cd.first));
    CHECK(graph.edgeCount() == 2);

    CHECK(graph.vertexExists(a));
    CHECK(graph.vertexExists(b));
    CHECK(graph.vertexExists(c));
    CHECK(graph.vertexExists(d));
}

TEST_CASE("removeEdge returns false for invalid edge IDs")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    SECTION("already removed edge ID")
    {
        REQUIRE(graph.removeEdge(ab.first));
        CHECK_FALSE(graph.removeEdge(ab.first));
        CHECK(graph.edgeCount() == 0);
    }

    SECTION("edge ID from another graph")
    {
        Graph otherGraph;

        const Graph::VertexId x = otherGraph.addVertex();
        const Graph::VertexId y = otherGraph.addVertex();

        const std::pair<Graph::EdgeId, bool> xy = otherGraph.addEdge(x, y);
        REQUIRE(xy.second);

        CHECK_FALSE(graph.removeEdge(xy.first));
        CHECK(graph.edgeExists(ab.first));
        CHECK(otherGraph.edgeExists(xy.first));
    }
}

TEST_CASE("removed edge IDs remain invalid after adding new edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, int>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> oldEdge = graph.addEdge(a, b, 14);

    REQUIRE(oldEdge.second);
    REQUIRE(graph.removeEdge(oldEdge.first));

    const std::pair<Graph::EdgeId, bool> newEdge = graph.addEdge(a, b, 24);

    REQUIRE(newEdge.second);

    CHECK_FALSE(graph.edgeExists(oldEdge.first));
    CHECK_FALSE(oldEdge.first == newEdge.first);
    CHECK_FALSE(graph.removeEdge(oldEdge.first));

    REQUIRE(graph.edgeExists(newEdge.first));
    CHECK(graph.edgeData(newEdge.first) == 24);
    CHECK(graph.edgeCount() == 1);
}

/**
 * invalid operations
 */

TEST_CASE("addEdge and emplaceEdge reject self-loops")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    CHECK_THROWS_AS(graph.addEdge(a, a), std::invalid_argument);
    CHECK_THROWS_AS(graph.emplaceEdge(a, a), std::invalid_argument);

    CHECK(graph.edgeCount() == 0);
}

TEST_CASE("addEdge and emplaceEdge reject invalid vertices")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    SECTION("removed vertex ID")
    {
        REQUIRE(graph.removeVertex(b));
        CHECK_THROWS_AS(graph.addEdge(a, b), std::invalid_argument);
        CHECK_THROWS_AS(graph.emplaceEdge(a, b), std::invalid_argument);
    }

    SECTION("vertex ID from another graph")
    {
        Graph otherGraph;
        const Graph::VertexId foreignId = otherGraph.addVertex();
        CHECK_THROWS_AS(graph.addEdge(a, foreignId), std::invalid_argument);
        CHECK_THROWS_AS(graph.emplaceEdge(a, foreignId), std::invalid_argument);
    }
    CHECK(graph.edgeCount() == 0);
}

TEST_CASE("hasEdge and findEdgeId reject invalid vertices")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    SECTION("removed vertex ID")
    {
        REQUIRE(graph.removeVertex(b));

        CHECK_THROWS_AS(graph.hasEdge(a, b), std::invalid_argument);
        CHECK_THROWS_AS(graph.findEdgeId(a, b), std::invalid_argument);
    }

    SECTION("vertex ID from another graph")
    {
        Graph otherGraph;
        const Graph::VertexId foreignId = otherGraph.addVertex();

        CHECK_THROWS_AS(graph.hasEdge(a, foreignId), std::invalid_argument);
        CHECK_THROWS_AS(graph.findEdgeId(a, foreignId), std::invalid_argument);
    }
}

TEST_CASE("endpoints throws for an invalid edge ID")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> ab = graph.addEdge(a, b);
    REQUIRE(ab.second);

    SECTION("removed edge ID")
    {
        REQUIRE(graph.removeEdge(ab.first));

        CHECK_THROWS_AS(graph.endpoints(ab.first), std::invalid_argument);
    }

    SECTION("edge ID from another graph")
    {
        Graph otherGraph;

        const Graph::VertexId x = otherGraph.addVertex();
        const Graph::VertexId y = otherGraph.addVertex();

        const std::pair<Graph::EdgeId, bool> xy = otherGraph.addEdge(x, y);
        REQUIRE(xy.second);
        CHECK_THROWS_AS(graph.endpoints(xy.first), std::invalid_argument);
    }
}

TEST_CASE("addEdge does not create duplicate edges")
{
    using Graph = simple_undirected_graph::DynamicGraph<>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();

    const std::pair<Graph::EdgeId, bool> first = graph.addEdge(a, b);
    REQUIRE(first.second);

    const std::pair<Graph::EdgeId, bool> duplicate = graph.addEdge(a, b);
    const std::pair<Graph::EdgeId, bool> reversed = graph.addEdge(b, a);

    CHECK_FALSE(duplicate.second);
    CHECK_FALSE(reversed.second);

    CHECK(duplicate.first == first.first);
    CHECK(reversed.first == first.first);

    CHECK(graph.edgeCount() == 1);
}

TEST_CASE("duplicate edge insertion preserves existing edge properties")
{
    using Graph = simple_undirected_graph::DynamicGraph<simple_undirected_graph::NoProperties, std::string>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex();
    const Graph::VertexId b = graph.addVertex();
    const std::pair<Graph::EdgeId, bool> original = graph.addEdge(a, b, "original");
    REQUIRE(original.second);

    SECTION("duplicate using addEdge")
    {
        const std::pair<Graph::EdgeId, bool> duplicate = graph.addEdge(b, a, "replacement");
        CHECK_FALSE(duplicate.second);
        CHECK(duplicate.first == original.first);
    }
    SECTION("duplicate using emplaceEdge")
    {
        const std::pair<Graph::EdgeId, bool> duplicate = graph.emplaceEdge(a, b, "replacement");
        CHECK_FALSE(duplicate.second);
        CHECK(duplicate.first == original.first);
    }
    CHECK(graph.edgeCount() == 1);
    CHECK(graph.edgeData(original.first) == "original");
}
