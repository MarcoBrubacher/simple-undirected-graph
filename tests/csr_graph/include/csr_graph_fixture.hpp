#pragma once

#include <simple_undirected_graph/csr_graph.hpp>
#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace csr_graph_test
{
    using ByteBool = simple_undirected_graph::byteBool;
    using Graph = simple_undirected_graph::CSRGraph<double, ByteBool, std::string>;

    inline constexpr std::size_t vertexCount = 6;
    inline constexpr std::size_t edgeCount = 5;

    inline constexpr std::array<Graph::Index, edgeCount * 2> neighbors = {3, 1, 2,
                                                                          4, 0,
                                                                          0,
                                                                          4, 0,
                                                                          3, 1};

    inline constexpr std::array<std::size_t, vertexCount + 1> offsets = {0, 3, 5, 6, 8, 10, 10};

    inline constexpr std::array<Graph::Index, edgeCount * 2> edgeIndices = {4, 3, 0,
                                                                            1, 3,
                                                                            0,
                                                                            2, 4,
                                                                            2, 1};

    inline constexpr std::array<std::pair<Graph::VertexId, Graph::VertexId>, edgeCount> expectedEndpoints = {{{0, 2},
                                                                                                              {1, 4},
                                                                                                              {3, 4},
                                                                                                              {0, 1},
                                                                                                              {0, 3}}};

    inline const std::string graphProperty = "CSR graph";

    inline constexpr std::array<double, vertexCount> vertexProperties = {1.5, 2.5, 3.5, 4.5, 5.5, 6.5};

    inline constexpr std::array<ByteBool, edgeCount> edgeProperties = {ByteBool{1}, ByteBool{0}, ByteBool{1}, ByteBool{0}, ByteBool{1}};

    inline Graph constructGraph()
    {
        return Graph::fromValidCSRFormat(graphProperty,
                                         std::vector<double>(vertexProperties.begin(), vertexProperties.end()),
                                         std::vector<ByteBool>(edgeProperties.begin(), edgeProperties.end()),
                                         std::vector<std::size_t>(offsets.begin(), offsets.end()),
                                         std::vector<Graph::Index>(neighbors.begin(), neighbors.end()),
                                         std::vector<Graph::Index>(edgeIndices.begin(), edgeIndices.end()));
    }
}