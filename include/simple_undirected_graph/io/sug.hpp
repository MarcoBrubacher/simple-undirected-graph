#pragma once

#include <simple_undirected_graph/csr_graph.hpp>
#include <filesystem>

namespace simple_undirected_graph::io
{
    template <typename VertexData, typename EdgeData, typename GraphData>
    void save(const CSRGraph<VertexData, EdgeData, GraphData> &graph, const std::filesystem::path &path);

    template <typename VertexData = NoProperties, typename EdgeData = NoProperties, typename GraphData = NoProperties>
    [[nodiscard]] CSRGraph<VertexData, EdgeData, GraphData> load(const std::filesystem::path &path);
}
#include <simple_undirected_graph/io/sug.tpp>