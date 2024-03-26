#pragma once

#include <concepts>

namespace livestats {

/**
 * An estimator is a device that allows to compute some statistics from real-time observations.
 * A new observation is supplied to the estimator using `add()`,
 * which must update the current estimate and return the new result.
 * If one only desires to update the current estimate, `push()` can be used instead.
 * Whether or not `add()` and `push()` have the same performance cost is implementation-defined.
 * The current estimate can be queried via `get()`; it may perform operations but its complexity must be O(1).
 * Some estimators can be combined to construct an estimation pipeline,
 * for example placing an outlier filter before a mean estimator.
 */
template <typename T>
concept Estimator = requires(T estimator, T const& const_estimator)
{
    typename T::value_type;
    { estimator.add(std::declval<typename T::value_type>()) } -> std::same_as<typename T::value_type>;
    { estimator.push(std::declval<typename T::value_type>()) }; // may return void but not required
    { estimator.reset() }; // may return void but not required
    { const_estimator.get() } -> std::same_as<typename T::value_type>;
    { const_estimator.size() } -> std::same_as<std::size_t>;
};

/**
 * An example of an estimator, useful to static assert that estimator adatoprs are themeselves estimators.
 */
struct estimator_archetype_t
{
    using value_type = double;
    value_type add(value_type) { return 0.0; }
    void push(value_type) { }
    void reset() { }
    value_type get() const { return 0.0; }
    std::size_t size() const { return 0; }
};
static_assert(Estimator<estimator_archetype_t>);

} // namespace livestats
