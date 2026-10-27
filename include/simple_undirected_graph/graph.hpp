#pragma once
#include <cstddef>

namespace simple_undirected_graph
{
    // empty property type used when no data is attached
    struct NoProperties
    {
    };

    /**
     * stores a boolean value in one byte
     * allows CSRGraph to use a mock bool type for vertex and edge data, since std::vector<bool> may store values as individual bits
     *
     * used by the graph converter when converting boolean properties or by users when directly using CSRGraph
     */
    struct byteBool
    {
        std::uint8_t value = 0;
    };
}