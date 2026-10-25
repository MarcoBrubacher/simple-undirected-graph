#pragma once

namespace simple_undirected_graph::detail
{
    inline std::size_t combineHash(std::size_t seed, std::size_t value) noexcept
    {
        // simple baseline mixer. TODO: replace after benchmarking hash strategies
        constexpr std::size_t bitCount = sizeof(std::size_t) * 8;

        value ^= value >> (bitCount / 2);
        seed ^= value;
        seed += (seed << 5) + (seed >> 3) + 1;

        return seed;
    }
}