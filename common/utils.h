/*
 *  Minimal implementations of some STL components:
 *      std::pair,
 *      std::for_each
 */

#pragma once

namespace tiny
{
/* <utility> */

template<typename T, typename U>
struct pair {
        T first;
        U second;
};

/* <algorithm> */

template<typename Container, typename Callable>
void
for_each(Container& cont, Callable call)
{
        for (auto& el : cont)
                call(el);
}
} /* namespace tiny */
