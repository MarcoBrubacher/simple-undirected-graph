#pragma once

#include <simple_undirected_graph/dynamic_graph.hpp>
#include <simple_undirected_graph/csr_graph.hpp>
#include <type_traits>
#include <concepts>

namespace simple_undirected_graph
{

    /**
     *  byteBool bool conversion
     */
    template <typename PropertyType>
    using CSRProperty = std::conditional_t<std::is_same_v<PropertyType, bool>, byteBool, PropertyType>;

    template <typename PropertyType>
    using DynamicProperty = std::conditional_t<std::is_same_v<PropertyType, byteBool>, bool, PropertyType>;

    template <typename VertexData, typename EdgeData, typename GraphData>
    using CSRConversionRes = CSRGraph<CSRProperty<VertexData>, CSRProperty<EdgeData>, GraphData>;

    template <typename VertexData, typename EdgeData, typename GraphData>
    using DynamicConversionRes = DynamicGraph<DynamicProperty<VertexData>, DynamicProperty<EdgeData>, GraphData>;

    /**
     *  Dynamic to CSR
     */
    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::copy_constructible<VertexData> && std::copy_constructible<EdgeData> && std::copy_constructible<GraphData>
    [[nodiscard]] CSRConversionRes<VertexData, EdgeData, GraphData> toCSRGraph(const DynamicGraph<VertexData, EdgeData, GraphData> &graph);

    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::move_constructible<VertexData> && std::move_constructible<EdgeData> && std::move_constructible<GraphData>
    [[nodiscard]] CSRConversionRes<VertexData, EdgeData, GraphData> toCSRGraph(DynamicGraph<VertexData, EdgeData, GraphData> &&graph);

    /**
     *  CSR to Dynamic
     */
    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::copy_constructible<VertexData> && std::copy_constructible<EdgeData> && std::copy_constructible<GraphData>
    [[nodiscard]] DynamicConversionRes<VertexData, EdgeData, GraphData> toDynamicGraph(CSRGraph<VertexData, EdgeData, GraphData> &&graph);

    template <typename VertexData, typename EdgeData, typename GraphData>
        requires std::move_constructible<VertexData> && std::move_constructible<EdgeData> && std::move_constructible<GraphData>
    [[nodiscard]] DynamicConversionRes<VertexData, EdgeData, GraphData> toDynamicGraph(const CSRGraph<VertexData, EdgeData, GraphData> &graph);
}

#include <simple_undirected_graph/conversion.tpp>