/*
 *  Minimal implementations of some STL components:
 *      std::pair,
 *      std::for_each,
 *      std::clamp
 */

#pragma once
#include <stdint.h>

namespace Tiny {
/* <utility> */
template <typename T, typename U> struct Pair {
    T first;
    U second;
};

/* <algorithm> */
template <typename Container, typename Callable> void forEach(Container& cont, Callable call)
{
    for (auto& el : cont)
        call(el);
}

template <typename T> const T& clamp(const T& x, const T& low, const T& high)
{
    if (x < low)
        return low;
    if (x > high)
        return high;
    return x;
}

template <typename T> const T& clamp(const T& x, const Pair<T, T>& range)
{
    if (x < range.first)
        return range.first;
    if (x > range.second)
        return range.second;
    return x;
}
}

#define UNREACHABLE __builtin_unreachable()

using u8 = uint8_t;
using u32 = uint32_t;
