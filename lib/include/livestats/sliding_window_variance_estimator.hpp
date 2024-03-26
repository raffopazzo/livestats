#pragma once

#include "livestats/estimator.hpp"
#include "livestats/sliding_window_mean_estimator.hpp"

#include "livestats/math/non_negative.hpp"

#include <cassert>
#include <type_traits>

namespace livestats {

/**
 * Estimator to compute the variance of a sliding window with at most N samples.
 *
 * @tparam  ValueType   The type of samples, eg integer, floating point or vectorized numerical type.
 */
template <typename ValueType>
class sliding_window_variance_estimator_t
{
    sliding_window_mean_estimator_t<ValueType> mean;
    ValueType n_s = ValueType{};

public:
    using value_type = ValueType;

    explicit sliding_window_variance_estimator_t(std::size_t const window_size) : mean(window_size) { }

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
        if (full()) [[likely]]
        {
            auto const oldest = mean.oldest();
            auto const old_mean = mean.get();
            auto const new_mean = mean.add(x);
            math::non_negative::plus_equal_a_minus_b_times_c_minus_d(n_s, x, oldest, x + oldest, new_mean + old_mean);
        }
        else
        {
            auto const old_mean = mean.get();
            auto const new_mean = mean.add(x);
            math::non_negative::plus_equal_a_minus_b_times_c_minus_d(n_s, x, old_mean, x, new_mean);
        }
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
     * Return the current value of the variance.
     */
    value_type get() const { return size() ? static_cast<value_type>(n_s / size()) : value_type{}; }

    /**
     * Return the total number of samples currently in the window.
     */
    std::size_t size() const { return mean.size(); }

    /**
     * Return true if the sliding window contains exactly N samples; false otherwise.
     */
    bool full() const { return mean.full(); }

    /**
     * Return the oldest sample in the current window.
     * Calling this method when `size() == 0` is undefined behaviour.
     */
    value_type oldest() const
    {
        assert(size() > 0ul);
        return mean.oldest();
    }
};
static_assert(Estimator<sliding_window_variance_estimator_t<double>>);


} // namespace livestats
