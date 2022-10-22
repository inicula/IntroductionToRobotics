#pragma once

namespace tiny
{
/* Containers */

template<typename T, unsigned N>
struct array {
private:
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;

public:
        constexpr reference
        operator[](const unsigned i)
        {
                return data[i];
        }

        constexpr const_reference
        operator[](const unsigned i) const
        {
                return data[i];
        }

        constexpr pointer
        begin()
        {
                return &data[0];
        }

        constexpr pointer
        end()
        {
                return &data[N];
        }

        constexpr const_pointer
        begin() const
        {
                return &data[0];
        }

        constexpr const_pointer
        end() const
        {
                return &data[N];
        }

        constexpr unsigned
        size() const
        {
                return N;
        }

public:
        T data[N];
};

template<typename T, typename U>
struct pair {
        T first;
        U second;
};

/* Algorithms */

template<typename Container, typename Callable>
void
for_each(Container& cont, Callable call)
{
        for (auto i1 = cont.begin(); i1 != cont.end(); ++i1)
                call(*i1);
}
} // namespace tiny
