
#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/graph.hpp>
#include <stdexcept>
#include <utility>

struct ThrowingData
{
    enum class ThrowOn
    {
        nothing,
        construction,
        copy_construction,
        move_construction,
        copy_assignment,
        move_assignment
    };
    int value;
    ThrowOn throwOn;

    ThrowingData() : value(0), throwOn(ThrowOn::nothing) {}

    explicit ThrowingData(int value, ThrowOn throwOn = ThrowOn::nothing) : value(value), throwOn(throwOn)
    {
        if (throwOn == ThrowOn::construction)
        {
            throw std::runtime_error("construction failed");
        }
    }

    ThrowingData(const ThrowingData &other) : value(other.value), throwOn(other.throwOn)
    {
        if (throwOn == ThrowOn::copy_construction)
        {
            throw std::runtime_error("copy construction failed");
        }
    }

    ThrowingData &operator=(const ThrowingData &other)
    {
        if (other.throwOn == ThrowOn::copy_assignment)
        {
            throw std::runtime_error("copy assignment failed");
        }
        value = other.value;
        throwOn = other.throwOn;
        return *this;
    }

    ThrowingData(ThrowingData &&other) : value(other.value), throwOn(other.throwOn)
    {
        if (throwOn == ThrowOn::move_construction)
        {
            throw std::runtime_error("move construction failed");
        }
    }

    ThrowingData &operator=(ThrowingData &&other)
    {
        if (other.throwOn == ThrowOn::move_assignment)
        {
            throw std::runtime_error("move assignment failed");
        }
        value = other.value;
        throwOn = other.throwOn;
        return *this;
    }
};

/**
 * vertex exception
 */

TEST_CASE("addVertex handles a throwing property copy")
{
    using Graph = simple_undirected_graph::Graph<ThrowingData>;
    Graph graph;
    const Graph::VertexId a = graph.emplaceVertex(1);
    const ThrowingData data{2, ThrowingData::ThrowOn::copy_construction};

    CHECK_THROWS_AS(graph.addVertex(data), std::runtime_error);
    CHECK(graph.vertexCount() == 1);
    REQUIRE(graph.vertexExists(a));
    CHECK(graph.vertexData(a).value == 1);
}

TEST_CASE("emplaceVertex handles a throwing constructor")
{
    using Graph = simple_undirected_graph::Graph<ThrowingData>;
    Graph graph;
    const Graph::VertexId a = graph.emplaceVertex(1);

    CHECK_THROWS_AS(graph.emplaceVertex(2, ThrowingData::ThrowOn::construction), std::runtime_error);
    CHECK(graph.vertexCount() == 1);
    REQUIRE(graph.vertexExists(a));
    CHECK(graph.vertexData(a).value == 1);
}

/**
 * edge exception
 */

TEST_CASE("addEdge handles a throwing property copy")
{
    using Graph = simple_undirected_graph::Graph<int, ThrowingData>;
    Graph graph;
    const Graph::VertexId a = graph.addVertex(1);
    const Graph::VertexId b = graph.addVertex(2);
    const ThrowingData data{3, ThrowingData::ThrowOn::copy_construction};

    CHECK_THROWS_AS(graph.addEdge(a, b, data), std::runtime_error);
    CHECK(graph.edgeCount() == 0);
    CHECK_FALSE(graph.hasEdge(a, b));
    CHECK(graph.degree(a) == 0);
    CHECK(graph.degree(b) == 0);
}

TEST_CASE("emplaceEdge handles a throwing constructor")
{
    using Graph = simple_undirected_graph::Graph<int, ThrowingData>;
    Graph graph;

    const Graph::VertexId a = graph.addVertex(1);
    const Graph::VertexId b = graph.addVertex(2);

    CHECK_THROWS_AS(graph.emplaceEdge(a, b, 3, ThrowingData::ThrowOn::construction), std::runtime_error);

    CHECK(graph.edgeCount() == 0);
    CHECK_FALSE(graph.hasEdge(a, b));
    CHECK(graph.degree(a) == 0);
    CHECK(graph.degree(b) == 0);
}

/**
 * copy exception
 */

TEST_CASE("copy constructor handles a throwing property copy")
{
    using Graph = simple_undirected_graph::Graph<ThrowingData>;
    Graph src;

    const Graph::VertexId a = src.emplaceVertex(1);
    const Graph::VertexId b = src.emplaceVertex(2, ThrowingData::ThrowOn::copy_construction);

    CHECK_THROWS_AS(Graph(src), std::runtime_error);
    CHECK(src.vertexCount() == 2);

    REQUIRE(src.vertexExists(a));
    REQUIRE(src.vertexExists(b));

    CHECK(src.vertexData(a).value == 1);
    CHECK(src.vertexData(b).value == 2);
}

TEST_CASE("copy assignment preserves the target when copying throws")
{
    using Graph = simple_undirected_graph::Graph<ThrowingData, int, int>;
    Graph src{1};
    Graph target{2};

    const Graph::VertexId srcA = src.emplaceVertex(1, ThrowingData::ThrowOn::copy_construction);

    const Graph::VertexId a = target.emplaceVertex(2);
    const Graph::VertexId b = target.emplaceVertex(3);
    const std::pair<Graph::EdgeId, bool> ab = target.addEdge(a, b, 4);
    REQUIRE(ab.second);

    CHECK_THROWS_AS(target = src, std::runtime_error);
    CHECK(target.graphData() == 2);
    CHECK(target.vertexCount() == 2);
    CHECK(target.edgeCount() == 1);

    REQUIRE(target.vertexExists(a));
    REQUIRE(target.vertexExists(b));
    REQUIRE(target.edgeExists(ab.first));

    CHECK(target.vertexData(a).value == 2);
    CHECK(target.vertexData(b).value == 3);
    CHECK(target.edgeData(ab.first) == 4);
    CHECK(target.hasEdge(a, b));
    CHECK(src.vertexExists(srcA));
}

/**
 * graph
 */

TEST_CASE("graph remains usable after a failed insertion")
{
    using Graph = simple_undirected_graph::Graph<ThrowingData, ThrowingData>;
    Graph graph;

    const Graph::VertexId a = graph.emplaceVertex(1);
    CHECK_THROWS_AS(graph.emplaceVertex(2, ThrowingData::ThrowOn::construction), std::runtime_error);

    const Graph::VertexId b = graph.emplaceVertex(2);
    CHECK_THROWS_AS(graph.emplaceEdge(a, b, 3, ThrowingData::ThrowOn::construction), std::runtime_error);

    const std::pair<Graph::EdgeId, bool> ab = graph.emplaceEdge(a, b, 3);
    REQUIRE(ab.second);

    CHECK(graph.vertexCount() == 2);
    CHECK(graph.edgeCount() == 1);
    CHECK(graph.hasEdge(a, b));

    CHECK(graph.vertexData(a).value == 1);
    CHECK(graph.vertexData(b).value == 2);
    CHECK(graph.edgeData(ab.first).value == 3);
}
