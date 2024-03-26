#pragma once

#include "livestats/estimator.hpp"

#include <boost/circular_buffer.hpp>

#include <cassert>

namespace livestats {

/**
 * Estimator to compute the mean of a sliding window with at most N samples.
 * It assumes the sum of all samples in the current slidiing window does not overflow.
 *
 * @tparam  ValueType   The type of samples, eg integer, floating point or vectorized numerical type.
 */
template <typename ValueType>
class sliding_window_mean_estimator_t
{
    boost::circular_buffer<ValueType> samples;
    ValueType sum = ValueType{};

public:
    using value_type = ValueType;

    explicit sliding_window_mean_estimator_t(std::size_t const window_size) : samples(window_size) { }

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
        if (full()) [[likely]]
            sum -= oldest();
        samples.push_back(sample);
        sum += sample;
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        samples.clear();
        sum = value_type{};
    }

    /**
     * Return the current value of the mean.
     */
    value_type get() const { return samples.size() ? sum / samples.size() : value_type{}; }

    /**
     * Return the total number of samples currently in the window.
     */
    std::size_t size() const { return samples.size(); }

    /**
     * Return true if the sliding window contains exactly N samples; false otherwise.
     */
    bool full() const { return samples.full(); }

    /**
     * Return the oldest sample in the current window.
     * Calling this method when `size() == 0` is undefined behaviour.
     */
    value_type oldest() const
    {
        assert(size() > 0ul);
        return samples.front();
    }

    value_type get_sum() const { return sum; }
};
static_assert(Estimator<sliding_window_mean_estimator_t<double>>);


} // namespace livestats
