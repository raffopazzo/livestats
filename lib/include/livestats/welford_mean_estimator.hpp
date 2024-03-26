#pragma once

#include <algorithm>
#include <cstdint>

#include "livestats/estimator.hpp"

namespace livestats {

/**
 * Incrementally computes the mean of a sequence,
 * by updating the previous estimate of the mean with the new sample.
 *
 * @tparam  ValueType   The type of samples, eg integer, floating point or vectorized numerical type.
 *
 * @remark  For integral types, the mean will be rounded towards the previous value of the mean.
 *          This behaviour is similar to `std::midpoint(a, b)` which rounds towards `a`.
 */
template <typename ValueType>
class welford_mean_estimator_t
{
    std::size_t n = 0ul;
    ValueType mean = ValueType{};

public:
    using value_type = ValueType;

    /**
     * Update the estimate with a new sample and return the new estimate.
     */
    value_type add(value_type const sample)
    {
        push(sample);
        return mean;
    }

    /**
     * Update the estimate with a new sample.
     */
    void push(value_type const sample)
    {
        ++n;
        value_type a = sample;
        value_type b = mean;
        value_type sign = 1;
        if (sample < mean)
        {
            std::swap(a, b);
            sign = -1;
        }
        mean += sign*((a - b)/n);
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        mean = value_type{};
        n = 0ul;
    }

    /**
     * Return the current value of the mean, without performing any computation.
     */
    value_type get() const { return mean; }

    /**
     * Return the total number of samples observed so far.
     */
    std::size_t size() const { return n; }
};
static_assert(Estimator<welford_mean_estimator_t<double>>);

} // namespace livestats
