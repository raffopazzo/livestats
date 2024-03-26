#pragma once

#include <algorithm>
#include <cstdint>

#include "livestats/estimator.hpp"
#include "livestats/welford_mean_estimator.hpp"

#include "livestats/math/non_negative.hpp"

namespace livestats {

/**
 * Incrementally computes the variance of a sequence,
 * by updating the previous estimate of the variance with the new sample.
 *
 * @tparam  ValueType           The type of samples, eg integer, floating point or vectorized numerical type.
 * @tparam  MeanEstimatorType   The estimator to use to estimate the running mean.
 */
template <typename ValueType, typename MeanEstimatorType = welford_mean_estimator_t<ValueType>>
class welford_variance_estimator_t
{
    MeanEstimatorType mean;
    ValueType n_s = ValueType{};

public:
    using value_type = ValueType;

    /**
     * Update the estimate with a new sample and return the new estimate.
     */
    value_type add(value_type const x)
    {
        push(x);
        return get();
    }

    /**
     * Update the estimate with a new sample.
     */
    void push(value_type const x)
    {
        auto const old_mean = mean.get();
        auto const new_mean = mean.add(x);
        math::non_negative::plus_equal_a_minus_b_times_c_minus_d(n_s, x, old_mean, x, new_mean);
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        mean.reset();
        n_s = value_type{};
    }

    /**
     * Return the current value of the variance,
     * without performing any computation.
     */
    value_type get() const { return size() ? n_s / size() : n_s; }

    /**
     * Return the total number of samples observed so far.
     */
    std::size_t size() const { return mean.size(); }
};
static_assert(Estimator<welford_variance_estimator_t<double>>);

} // namespace livestats
