
#include <catch2/catch_test_macros.hpp>
#include <simple_undirected_graph/conversion.hpp>
#include <csr_graph/include/csr_graph_fixture.hpp>
#include <concepts>
#include <string>
#include <utility>
#include <span>
#include <optional>
#include <memory>
#include <vector>

using Conversion = simple_undirected_graph::Conversion;
using Dynamic = simple_undirected_graph::DynamicGraph<bool, int, std::string>;
using CSR = simple_undirected_graph::CSRGraph<simple_undirected_graph::byteBool, int, std::string>;

// Dynamic to CSR
static_assert(std::same_as<decltype(Conversion::toCSRGraph(std::declval<const Dynamic &>())), CSR>);
static_assert(std::same_as<decltype(Conversion::toCSRGraph(std::declval<Dynamic &&>())), CSR>);

// CSR to Dynamic
static_assert(std::same_as<decltype(Conversion::toDynamicGraph(std::declval<const CSR &>())), Dynamic>);
static_assert(std::same_as<decltype(Conversion::toDynamicGraph(std::declval<CSR &&>())), Dynamic>);

/**
 * CSR to Dynamic
 */

TEST_CASE("Conversion: CSR to Dynamic preserves graph")
{
    using CSR = csr_graph_test::Graph;
    using Dynamic = simple_undirected_graph::DynamicGraph<double, bool, std::string>;

    const CSR csr = csr_graph_test::constructGraph();

    Dynamic dynamic = simple_undirected_graph::Conversion::toDynamicGraph(csr);

    CHECK(dynamic.vertexCount() == csr_graph_test::vertexCount);
    CHECK(dynamic.edgeCount() == csr_graph_test::edgeCount);
    CHECK(dynamic.graphData() == csr_graph_test::graphProperty);

    const std::span<const Dynamic::VertexId> vertices = dynamic.vertices();
    REQUIRE(vertices.size() == csr_graph_test::vertexCount);

    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        CHECK(dynamic.vertexData(vertices[i]) == csr_graph_test::vertexProperties[i]);
    }

    for (std::size_t i = 0; i < csr_graph_test::edgeCount; ++i)
    {
        const CSR::VertexId sourceIndex = csr_graph_test::expectedEndpoints[i].first;
        const CSR::VertexId targetIndex = csr_graph_test::expectedEndpoints[i].second;

        const std::optional<Dynamic::EdgeId> edgeId = dynamic.findEdgeId(vertices[sourceIndex], vertices[targetIndex]);
        REQUIRE(edgeId.has_value());
        CHECK(dynamic.edgeData(*edgeId) == (csr_graph_test::edgeProperties[i].value != 0));
    }
}

TEST_CASE("Conversion: CSR to Dynamic supports move-only")
{
    using Property = std::unique_ptr<std::string>;
    using CSR = simple_undirected_graph::CSRGraph<Property, Property, Property>;
    using Dynamic = simple_undirected_graph::DynamicGraph<Property, Property, Property>;

    std::vector<Property> vertexData;
    vertexData.push_back(std::make_unique<std::string>("v1"));
    vertexData.push_back(std::make_unique<std::string>("v2"));
    vertexData.push_back(std::make_unique<std::string>("v3"));

    std::vector<Property> edgeData;
    edgeData.push_back(std::make_unique<std::string>("e1"));
    edgeData.push_back(std::make_unique<std::string>("e2"));

    CSR csr = CSR::fromValidCSRFormat(std::make_unique<std::string>("graph"), std::move(vertexData), std::move(edgeData), {0, 1, 3, 4}, {1, 0, 2, 1}, {0, 0, 1, 1});
    const Dynamic dynamic = simple_undirected_graph::Conversion::toDynamicGraph(std::move(csr));

    const std::span<const Dynamic::VertexId> vertices = dynamic.vertices();
    REQUIRE(vertices.size() == 3);
    REQUIRE(dynamic.edgeCount() == 2);
    REQUIRE(dynamic.graphData() != nullptr);
    REQUIRE(dynamic.vertexData(vertices[0]) != nullptr);
    REQUIRE(dynamic.vertexData(vertices[1]) != nullptr);
    REQUIRE(dynamic.vertexData(vertices[2]) != nullptr);

    CHECK(*dynamic.graphData() == "graph");
    CHECK(*dynamic.vertexData(vertices[0]) == "v1");
    CHECK(*dynamic.vertexData(vertices[1]) == "v2");
    CHECK(*dynamic.vertexData(vertices[2]) == "v3");

    const std::optional<Dynamic::EdgeId> e1 = dynamic.findEdgeId(vertices[0], vertices[1]);
    REQUIRE(e1.has_value());
    REQUIRE(dynamic.edgeData(*e1) != nullptr);
    CHECK(*dynamic.edgeData(*e1) == "e1");

    const std::optional<Dynamic::EdgeId> e2 = dynamic.findEdgeId(vertices[1], vertices[2]);
    REQUIRE(e2.has_value());
    REQUIRE(dynamic.edgeData(*e2) != nullptr);
    CHECK(*dynamic.edgeData(*e2) == "e2");
}

/**
 * Dynamic to CSR
 */

TEST_CASE("Conversion: Dynamic to CSR preserves graph")
{
    using Dynamic = simple_undirected_graph::DynamicGraph<double, bool, std::string>;
    using CSR = simple_undirected_graph::CSRGraph<double, simple_undirected_graph::byteBool, std::string>;

    Dynamic dynamic("dynamic graph");

    const Dynamic::VertexId vertexA = dynamic.addVertex(1.0);
    const Dynamic::VertexId vertexB = dynamic.addVertex(2.0);
    const Dynamic::VertexId vertexC = dynamic.addVertex(3.0);
    dynamic.addEdge(vertexA, vertexB, true);
    dynamic.addEdge(vertexB, vertexC, false);

    const CSR csr = simple_undirected_graph::Conversion::toCSRGraph(dynamic);
    CHECK(csr.vertexCount() == 3);
    CHECK(csr.edgeCount() == 2);
    CHECK(csr.graphData() == "dynamic graph");
    REQUIRE(CSR::isValidCSRFormat(csr.vertexCount(), csr.edgeCount(), csr.offsets(), csr.neighborIndices(), csr.adjacencyEdgeIndices()));

    const std::span<const double> vertices = csr.vertexData();
    REQUIRE(vertices.size() == 3);

    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        CHECK(vertices[i] == dynamic.vertexData(dynamic.vertices()[i]));
    }

    const std::optional<CSR::EdgeId> ab = csr.findEdgeId(0, 1);
    REQUIRE(ab.has_value());
    const std::optional<CSR::EdgeId> bc = csr.findEdgeId(1, 2);
    REQUIRE(bc.has_value());

    CHECK(csr.edgeData(*ab).value == 1);
    CHECK(csr.edgeData(*bc).value == 0);

    CHECK_FALSE(csr.hasEdge(0, 2));
    CHECK(dynamic.vertexData(vertexA) == 1.0);
    CHECK(dynamic.edgeCount() == 2);
}

TEST_CASE("Conversion: Dynamic to CSR supports move-only")
{
    using Property = std::unique_ptr<std::string>;
    using Dynamic = simple_undirected_graph::DynamicGraph<Property, Property, Property>;
    using CSR = simple_undirected_graph::CSRGraph<Property, Property, Property>;

    Dynamic dynamic(std::make_unique<std::string>("graph"));
    const Dynamic::VertexId v1 = dynamic.addVertex(std::make_unique<std::string>("v1"));
    const Dynamic::VertexId v2 = dynamic.addVertex(std::make_unique<std::string>("v2"));
    const Dynamic::VertexId v3 = dynamic.addVertex(std::make_unique<std::string>("v3"));
    dynamic.addEdge(v1, v2, std::make_unique<std::string>("e1"));
    dynamic.addEdge(v2, v3, std::make_unique<std::string>("e2"));

    const CSR csr = simple_undirected_graph::Conversion::toCSRGraph(std::move(dynamic));
    REQUIRE(csr.vertexCount() == 3);
    REQUIRE(csr.edgeCount() == 2);
    REQUIRE(CSR::isValidCSRFormat(csr.vertexCount(), csr.edgeCount(), csr.offsets(), csr.neighborIndices(), csr.adjacencyEdgeIndices()));
    REQUIRE(csr.graphData() != nullptr);
    REQUIRE(csr.vertexData(0) != nullptr);
    REQUIRE(csr.vertexData(1) != nullptr);
    REQUIRE(csr.vertexData(2) != nullptr);

    CHECK(*csr.graphData() == "graph");
    CHECK(*csr.vertexData(0) == "v1");
    CHECK(*csr.vertexData(1) == "v2");
    CHECK(*csr.vertexData(2) == "v3");

    const std::optional<CSR::EdgeId> e1 = csr.findEdgeId(0, 1);
    REQUIRE(e1.has_value());
    REQUIRE(csr.edgeData(*e1) != nullptr);
    CHECK(*csr.edgeData(*e1) == "e1");

    const std::optional<CSR::EdgeId> e2 = csr.findEdgeId(1, 2);
    REQUIRE(e2.has_value());
    REQUIRE(csr.edgeData(*e2) != nullptr);
    CHECK(*csr.edgeData(*e2) == "e2");
}

/**
 * special cases
 */
TEST_CASE("Conversion: empty graphs")
{
    using Dynamic = simple_undirected_graph::DynamicGraph<>;
    using CSR = simple_undirected_graph::CSRGraph<>;

    SECTION("Dynamic to CSR")
    {
        const Dynamic dynamic;
        const CSR csr = simple_undirected_graph::Conversion::toCSRGraph(dynamic);

        CHECK(csr.empty());
        CHECK(csr.vertexCount() == 0);
        CHECK(csr.edgeCount() == 0);
        CHECK(CSR::isValidCSRFormat(0, 0, csr.offsets(), csr.neighborIndices(), csr.adjacencyEdgeIndices()));
    }

    SECTION("CSR to Dynamic")
    {
        const CSR csr;
        const Dynamic dynamic = simple_undirected_graph::Conversion::toDynamicGraph(csr);

        CHECK(dynamic.empty());
        CHECK(dynamic.vertexCount() == 0);
        CHECK(dynamic.edgeCount() == 0);
    }
}

TEST_CASE("Conversion: isolated vertices")
{
    using Dynamic = simple_undirected_graph::DynamicGraph<std::string>;
    using CSR = simple_undirected_graph::CSRGraph<std::string>;

    SECTION("Dynamic to CSR")
    {
        Dynamic dynamic;
        dynamic.addVertex("v1");
        dynamic.addVertex("v2");

        const CSR csr = simple_undirected_graph::Conversion::toCSRGraph(dynamic);
        REQUIRE(csr.vertexCount() == 2);

        CHECK(csr.edgeCount() == 0);
        CHECK(csr.vertexData(0) == "v1");
        CHECK(csr.vertexData(1) == "v2");
        CHECK(CSR::isValidCSRFormat(2, 0, csr.offsets(), csr.neighborIndices(), csr.adjacencyEdgeIndices()));
    }

    SECTION("CSR to Dynamic")
    {
        const CSR csr = CSR::fromValidCSRFormat(simple_undirected_graph::NoProperties{}, {"v1", "v2"}, {}, {0, 0, 0}, {}, {});
        const Dynamic dynamic = simple_undirected_graph::Conversion::toDynamicGraph(csr);

        const std::span<const Dynamic::VertexId> vertices = dynamic.vertices();
        REQUIRE(vertices.size() == 2);

        CHECK(dynamic.edgeCount() == 0);
        CHECK(dynamic.vertexData(vertices[0]) == "v1");
        CHECK(dynamic.vertexData(vertices[1]) == "v2");
    }
}

/**
 * additional tests
 */

TEST_CASE("Conversion: Dynamic to CSR handles slot reuse")
{
    using Dynamic = simple_undirected_graph::DynamicGraph<std::string, std::string>;
    using CSR = simple_undirected_graph::CSRGraph<std::string, std::string>;

    Dynamic dynamic;
    const Dynamic::VertexId v1 = dynamic.addVertex("v1");
    const Dynamic::VertexId v2 = dynamic.addVertex("v2");
    const Dynamic::VertexId v3 = dynamic.addVertex("v3");

    const std::pair<Dynamic::EdgeId, bool> oldEdge = dynamic.addEdge(v1, v2, "old");
    REQUIRE(oldEdge.second);
    REQUIRE(dynamic.removeEdge(oldEdge.first));

    REQUIRE(dynamic.removeVertex(v2));

    const Dynamic::VertexId v4 = dynamic.addVertex("v4");
    const std::pair<Dynamic::EdgeId, bool> e1 = dynamic.addEdge(v1, v3, "e1");
    REQUIRE(e1.second);
    const std::pair<Dynamic::EdgeId, bool> e2 = dynamic.addEdge(v3, v4, "e2");
    REQUIRE(e2.second);

    // old Ids have to remain invalid
    CHECK_FALSE(dynamic.vertexExists(v2));
    CHECK_FALSE(dynamic.edgeExists(oldEdge.first));

    CHECK(dynamic.vertexExists(v4));
    CHECK(dynamic.edgeExists(e1.first));
    CHECK(dynamic.edgeExists(e2.first));

    const CSR csr = simple_undirected_graph::Conversion::toCSRGraph(dynamic);
    REQUIRE(csr.vertexCount() == 3);
    REQUIRE(csr.edgeCount() == 2);
    REQUIRE(CSR::isValidCSRFormat(csr.vertexCount(), csr.edgeCount(), csr.offsets(), csr.neighborIndices(), csr.adjacencyEdgeIndices()));

    const std::span<const Dynamic::VertexId> vertices = dynamic.vertices();
    REQUIRE(vertices.size() == csr.vertexCount());

    std::optional<CSR::VertexId> indexV1;
    std::optional<CSR::VertexId> indexV3;
    std::optional<CSR::VertexId> indexV4;

    for (CSR::VertexId i = 0; i < csr.vertexCount(); ++i)
    {
        CHECK(csr.vertexData(i) == dynamic.vertexData(vertices[i]));

        if (vertices[i] == v1)
        {
            indexV1 = i;
        }
        else if (vertices[i] == v3)
        {
            indexV3 = i;
        }
        else if (vertices[i] == v4)
        {
            indexV4 = i;
        }
    }
    REQUIRE(indexV1.has_value());
    REQUIRE(indexV3.has_value());
    REQUIRE(indexV4.has_value());

    const std::optional<CSR::EdgeId> csrE1 = csr.findEdgeId(*indexV1, *indexV3);
    REQUIRE(csrE1.has_value());
    CHECK(csr.edgeData(*csrE1) == "e1");

    const std::optional<CSR::EdgeId> csrE2 = csr.findEdgeId(*indexV3, *indexV4);
    REQUIRE(csrE2.has_value());
    CHECK(csr.edgeData(*csrE2) == "e2");

    CHECK_FALSE(csr.hasEdge(*indexV1, *indexV4));
}