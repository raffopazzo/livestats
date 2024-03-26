#pragma once

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace livestats::math::non_negative {

/**
 * Computes `r -= (a-b)*(c-d)` in a domain of non-negative numbers.
 */
template <typename T> T minus_equal_a_minus_b_times_c_minus_d(T& r, T a, T b, T c, T d);

/**
 * Computes `r += (a-b)*(c-d)` in a domain of non-negative numbers.
 */
template <typename T> T plus_equal_a_minus_b_times_c_minus_d(T& r, T a, T b, T c, T d);

// implementation

template <typename T>
T plus_equal_a_minus_b_times_c_minus_d(T& r, T a, T b, T c, T d)
{
    auto const zero = T{};
    assert(r >= zero);
    assert(a >= zero);
    assert(b >= zero);
    assert(c >= zero);
    assert(d >= zero);
    if constexpr (std::is_unsigned_v<T>)
    {
        int sign = 1;
        if (a < b)
        {
            std::swap(a, b);
            sign *= -1;
        }
        if (c < d)
        {
            std::swap(c, d);
            sign *= -1;
        }
        if (sign < 0)
        {
            auto const tmp = (a-b)*(c-d);
            r = r < tmp ? zero : r-tmp;
            return r;
        }
        else
            return r += (a-b)*(c-d);
    }
    else
    {
        r += (a-b)*(c-d);
        if (r < zero) [[unlikely]]
            r = zero;
        return r;
    }
}

template <typename T>
T minus_equal_a_minus_b_times_c_minus_d(T& r, T a, T b, T c, T d)
{
    return plus_equal_a_minus_b_times_c_minus_d(r, b, a, c, d);
}

} // namespace livestats::math::non_negative
