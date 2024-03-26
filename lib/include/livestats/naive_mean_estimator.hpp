#pragma once

#include <algorithm>
#include <cstdint>

#include "livestats/estimator.hpp"

namespace livestats {

/**
 * Computes the mean of a sequence by dividing the cumulative sum by the total number of observations.
 * This estimator is fast but unsafe in the presence of over/underflows; use only with sanity-checked data.
 * You can use this in combination with `boost::multiprecision::cpp_int` to achieve arbitrary precision.
 *
 * @tparam  ValueType   The type of samples, eg integer, floating point or vectorized numerical type.
 */
template <typename ValueType>
class naive_mean_estimator_t
{
    std::size_t n = 0ul;
    ValueType sum = ValueType{};

public:
    using value_type = ValueType;

    /**
     * Update the estimate with a new sample and return the new estimate.
     */
    value_type add(value_type const sample)
    {
        push(sample);
        return get();
    }

    /**
     * Update the estimate with a new sample.
     */
    void push(value_type const sample)
    {
        ++n;
        sum += sample;
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        sum = value_type{};
        n = 0ul;
    }

    /**
     * Return the current value of the mean.
     */
    value_type get() const { return n ? sum / n : value_type{}; }

    /**
     * Return the total number of samples observed so far.
     */
    std::size_t size() const { return n; }
};
static_assert(Estimator<naive_mean_estimator_t<double>>);

} // namespace livestats

