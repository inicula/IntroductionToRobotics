/*
 *  Minimal implementations of some STL components:
 *      std::pair,
 *      std::for_each
 */

#pragma once

namespace Tiny {
/* <utility> */
template <typename T, typename U>
struct Pair {
    T first;
    U second;
};

/* <algorithm> */
template <typename Container, typename Callable>
void forEach(Container& cont, Callable call)
{
    for (auto& el : cont)
        call(el);
}
} /* namespace tiny */
