#pragma once

#include "livestats/estimator.hpp"
#include "livestats/welford_mean_estimator.hpp"
#include "livestats/welford_variance_estimator.hpp"

#include <cassert>

namespace livestats {

/**
 * An estimator adaptor that filters outliers before pushing new samples into the underlying estimator.
 * New samples are considered outliers if they are more than 3 standard deviations away from current mean and variance.
 * In order to allow this filter to adjust to new regimes in the incoming data stream,
 * mean and variance include all discarded samples as well as all accepted samples.
 *
 * @tparam  EstimatorType       The underlying estimator being adapted.
 * @tparam  MeanValueEstimator  The estimator for the mean of all incoming data.
 * @tparam  VarianceEstimator   The estimator for the variance of all incoming data.
 */
template <
    Estimator EstimatorType,
    Estimator MeanValueEstimator = welford_mean_estimator_t<typename EstimatorType::value_type>,
    Estimator VarianceEstimator = welford_variance_estimator_t<typename EstimatorType::value_type>
>
class zscore_outlier_estimator_adaptor_t
{
    EstimatorType estimator;
    MeanValueEstimator mean;
    VarianceEstimator variance;

    std::size_t n_discarded = 0ul;

public:
    using value_type = typename EstimatorType::value_type;

    /**
     * Check if the input value is an outlier:
     *   - if not pass it to the underlying estimator and return the new result;
     *   - otherwise discard the sample and return the previuos result from the underlying estimator.
     */
    value_type add(value_type const x)
    {
        push(x);
        return get();
    }

    /**
     * Check if the input value is an outlier:
     *   - if not pass it to the underlying estimator and return the new result;
     *   - otherwise discard the sample.
     */
    void push(value_type const x)
    {
        if (size() == 0ul or variance.get() == value_type{} or not is_outlier(x))
            estimator.push(x);
        else
            ++n_discarded;
        mean.push(x);
        variance.push(x);
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        estimator.reset();
        mean.reset();
        variance.reset();
        n_discarded = 0ul;
    }

    /**
     * Return the current value of the variance,
     * without performing any computation.
     */
    value_type get() const { return estimator.get(); }

    /**
     * Return the total number of samples observed so far, excluding discarded samples.
     * Use `size_discarded()` to retrieve the number of samples discarded so far.
     */
    std::size_t size() const { return estimator.size(); }

    /**
     * Return the number of samples discarded so far.
     * Use `size()` to retrieve the number of samples accepted so far.
     */
    std::size_t size_discarded() const { return n_discarded; }

private:
    bool is_outlier(value_type const x) const
    {
        // use squared of zscore instead of normal zscore because value_type may not support sqrt and abs
        assert(variance.get() != value_type{});
        auto const sqr = [] (auto const x) { return x*x; };
        auto const zscore2 = sqr(x - mean.get()) / variance.get();
        return zscore2 > value_type{9};
    }
};
static_assert(Estimator<zscore_outlier_estimator_adaptor_t<estimator_archetype_t>>);

} // namespace livestats

