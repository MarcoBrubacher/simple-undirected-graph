#include <catch2/catch_test_macros.hpp>
#include <csr_graph/include/csr_graph_fixture.hpp>
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

using Graph = csr_graph_test::Graph;

static_assert(std::unsigned_integral<Graph::Index>);

static_assert(std::same_as<decltype(Graph::isValidCSRFormat(
                                    csr_graph_test::vertexCount,
                                    csr_graph_test::edgeCount,
                                    csr_graph_test::offsets,
                                    csr_graph_test::neighbors,
                                    csr_graph_test::edgeIndices)),
                                    bool>);

static_assert(std::same_as<decltype(Graph::fromValidCSRFormat(
                                    std::declval<std::string>(),
                                    std::declval<std::vector<double>>(),
                                    std::declval<std::vector<csr_graph_test::ByteBool>>(),
                                    std::declval<std::vector<std::size_t>>(),
                                    std::declval<std::vector<Graph::Index>>(),
                                    std::declval<std::vector<Graph::Index>>())),
                                    Graph>);
                                    
TEST_CASE("CSR graph: valid CSR formats are accepted")
{
    CHECK(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                  csr_graph_test::edgeCount,
                                  csr_graph_test::offsets,
                                  csr_graph_test::neighbors,
                                  csr_graph_test::edgeIndices));

    const std::array<Graph::Index, 0> emptyIndices = {};

    SECTION("empty graph")
    {
        const std::array<std::size_t, 1> offsets = {0};
        CHECK(Graph::isValidCSRFormat(0, 0, offsets, emptyIndices, emptyIndices));
    }

    SECTION("graph with only isolated vertices is valid")
    {
        const std::array<std::size_t, 4> offsets = {0, 0, 0, 0};
        CHECK(Graph::isValidCSRFormat(3, 0, offsets, emptyIndices, emptyIndices));
    }
}

TEST_CASE("CSR graph: inconsistent CSR sizes are rejected")
{
    const std::array<std::size_t, 0> emptyOffsets = {};
    const std::array<Graph::Index, 0> emptyIndices = {};

    CHECK_FALSE(Graph::isValidCSRFormat(0, 0, emptyOffsets, emptyIndices, emptyIndices));

    CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount - 1,
                                        csr_graph_test::edgeCount,
                                        csr_graph_test::offsets,
                                        csr_graph_test::neighbors,
                                        csr_graph_test::edgeIndices));

    CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                        csr_graph_test::edgeCount + 1,
                                        csr_graph_test::offsets,
                                        csr_graph_test::neighbors,
                                        csr_graph_test::edgeIndices));

    std::vector<Graph::Index> neighbors(csr_graph_test::neighbors.begin(), csr_graph_test::neighbors.end());
    std::vector<Graph::Index> edgeIndices(csr_graph_test::edgeIndices.begin(), csr_graph_test::edgeIndices.end());

    SECTION("neighbor and edge index sizes differ")
    {
        edgeIndices.pop_back();

        CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                            csr_graph_test::edgeCount,
                                            csr_graph_test::offsets,
                                            neighbors,
                                            edgeIndices));
    }

    SECTION("adjacency entry count is odd")
    {
        neighbors.pop_back();
        edgeIndices.pop_back();

        CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                            csr_graph_test::edgeCount,
                                            csr_graph_test::offsets,
                                            neighbors,
                                            edgeIndices));
    }
}

TEST_CASE("CSR graph: invalid offsets are rejected")
{
    std::vector<std::size_t> offsets(csr_graph_test::offsets.begin(), csr_graph_test::offsets.end());

    SECTION("first offset is not zero")
    {
        offsets[0] = 1;
    }

    SECTION("last offset does not match adjacency size")
    {
        offsets.back() = 9;
    }

    SECTION("offsets are not monotonically increasing")
    {
        offsets[2] = 2;
    }

    SECTION("offset exceeds adjacency size")
    {
        offsets[1] = 11;
    }

    CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                        csr_graph_test::edgeCount,
                                        offsets,
                                        csr_graph_test::neighbors,
                                        csr_graph_test::edgeIndices));
}

TEST_CASE("CSR graph: invalid neighbor indices are rejected")
{
    std::vector<Graph::Index> neighbors(csr_graph_test::neighbors.begin(), csr_graph_test::neighbors.end());

    SECTION("neighbor index is out of bounds")
    {
        neighbors[0] = static_cast<Graph::Index>(csr_graph_test::vertexCount);
    }

    SECTION("self-loop")
    {
        neighbors[0] = 0;
    }

    SECTION("duplicate neighbor")
    {
        neighbors[1] = neighbors[0];
    }

    SECTION("missing reverse adjacency")
    {
        neighbors[7] = 1;
    }

    CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                        csr_graph_test::edgeCount,
                                        csr_graph_test::offsets,
                                        neighbors,
                                        csr_graph_test::edgeIndices));
}

TEST_CASE("CSR graph: invalid edge indices are rejected")
{
    std::vector<Graph::Index> edgeIndices(csr_graph_test::edgeIndices.begin(), csr_graph_test::edgeIndices.end());

    SECTION("edge index is out of bounds")
    {
        edgeIndices[0] = static_cast<Graph::Index>(csr_graph_test::edgeCount);
    }

    SECTION("opposite directions have different edge indices")
    {
        std::swap(edgeIndices[7], edgeIndices[8]);
    }

    SECTION("edge index occurs more than twice")
    {
        edgeIndices[7] = 3;
    }

    CHECK_FALSE(Graph::isValidCSRFormat(csr_graph_test::vertexCount,
                                        csr_graph_test::edgeCount,
                                        csr_graph_test::offsets,
                                        csr_graph_test::neighbors,
                                        edgeIndices));
}