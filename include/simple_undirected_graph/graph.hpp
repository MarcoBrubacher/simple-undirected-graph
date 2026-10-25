#pragma once
#include <cstddef>
namespace simple_undirected_graph
{
    // empty property type used when no data is attached
    struct NoProperties
    {
    };

    namespace detail
    {
        // combines a hash seed with another value
        inline std::size_t combineHash(std::size_t seed, std::size_t value) noexcept;
    }
}
#include <simple_undirected_graph/graph.tpp>