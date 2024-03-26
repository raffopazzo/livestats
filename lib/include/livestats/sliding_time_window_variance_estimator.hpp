#pragma once

#include "livestats/estimator.hpp"
#include "livestats/sliding_time_window_tags.hpp"

#include "livestats/math/non_negative.hpp"

#include <boost/hana.hpp>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <optional>

namespace livestats {

/**
 * Estimator to compute variance (and mean) of one or more sliding time windows.
 * It assumes the sum of all samples in each time window does not overflow.
 *
 * @tparam  ValueType       The type of samples, eg integer, floating point or vectorized numerical type.
 * @tparam  Milliseconds1   The size in milliseconds of the primary sliding window.
 * @tparam  MillisecondsN   The size in milliseconds of secondary sliding windows.
 */
template <typename ValueType, std::size_t Milliseconds1, std::size_t... MillisecondsN>
class sliding_time_window_variance_estimator_t
{
public:
    using value_type = ValueType;

private:
    static constexpr auto primary_window = sliding_time_window_tag<Milliseconds1>;

    /**
     * Each sample is associated with its timestamp.
     */
    struct sample_t
    {
        std::chrono::steady_clock::time_point timestamp; // steady_clock guarantees non-decreasing time order
        value_type value;
    };

    /**
     * Each window contains the total count, sum and "unscaled variance" of the samples in that window,
     * along with an iterator pointing to the beginning of the window.
     */
    class window_t
    {
        std::size_t n = 0ul;
        value_type sum = {};
        value_type n_s = {};
        std::optional<typename std::deque<sample_t>::const_iterator> begin = std::nullopt; // empty if `n == 0`

    public:

        value_type mean() const { return n > 0ul ? sum / n : value_type{}; }
        value_type variance() const { return n > 0ul ? n_s / n : value_type{}; }
        std::size_t size() const { return n; }

        void push(typename std::deque<sample_t>::const_iterator const samples_begin, value_type const value)
        {
            auto const old_mean = mean();
            ++n;
            sum += value;
            auto const new_mean = mean();
            math::non_negative::plus_equal_a_minus_b_times_c_minus_d(n_s, value, old_mean, value, new_mean);
            if (not begin)
                begin = samples_begin;
        }

        /**
         * Advance the beginning of the sliding window dropping all samples older than the given timestamp.
         * Return the new start of the sliding window or the end iterator if the window is now empty.
         */
        typename std::deque<sample_t>::const_iterator
        drop_before(
            std::chrono::steady_clock::time_point const t0,
            typename std::deque<sample_t>::const_iterator const end)
        {
            if (n == 0ul)
                return end;
            assert(begin.has_value()); // set by push()
            auto& it = *begin;
            while (it->timestamp < t0)
            {
                auto const old_mean = mean();
                if (--n == 0ul)
                {
                    reset();
                    return end;
                }
                auto const x = it->value;
                sum -= x;
                auto const new_mean = mean();
                if (n == 1ul)
                    n_s = value_type{};
                else
                    math::non_negative::minus_equal_a_minus_b_times_c_minus_d(n_s, x, old_mean, x, new_mean);
                ++it;
                assert(it != end);
            }
            return it;
        }

        void reset()
        {
            n = 0ul;
            sum = value_type{};
            n_s  = value_type{};
            begin.reset();
        }
    };

    using window_map_t = decltype(boost::hana::make_map( // associate a window to each duration
        boost::hana::make_pair(sliding_time_window_tag<Milliseconds1>, window_t{}),
        boost::hana::make_pair(sliding_time_window_tag<MillisecondsN>, window_t{})...
    ));

    std::deque<sample_t> samples;
    window_map_t windows;

public:
    /**
     * Push a new sample in all sliding windows and retrieve the new variance of the primary window.
     */
    value_type add(value_type const value)
    {
        push(value);
        return get();
    }

    /**
     * Push a new sample in all sliding windows.
     */
    void push(value_type const value)
    {
        push(value, std::chrono::steady_clock::now());
    }

    /**
     * Push a new sample in all sliding windows and advance them all to the given timestamp.
     * Samples must be pushed in non-decreasing time order;
     * if the given timestamp is older than the current latest sample,
     * the new sample will be discarded and this function performs nothing.
     */
    void push(value_type const value, std::chrono::steady_clock::time_point const timestamp)
    {
        if (samples.empty() or samples.back().timestamp <= timestamp)
        {
            samples.push_back(sample_t{timestamp, value});
            boost::hana::unpack(
                boost::hana::keys(windows),
                [&] (auto... w) { (windows[w].push(samples.begin(), value), ...); });
            advance(timestamp);
        }
    }

    /**
     * Update all sliding windows by discarding samples that fall outside of each window when compared to `now`.
     * Invocations to this function must happen in non-decreasing time order;
     * if `now` is older than the current latest sample, this function performs nothing.
     */
    void advance(std::chrono::steady_clock::time_point const now)
    {
        if (samples.empty() or now < samples.back().timestamp)
            return;
        auto const new_begin = boost::hana::unpack(boost::hana::keys(windows),
            [&] <std::size_t... Millis> (sliding_time_window_tag_t<Millis>... w)
            {
                return std::min({windows[w].drop_before(now - std::chrono::milliseconds(Millis), samples.end())...});
            });
        samples.erase(samples.begin(), new_begin);
    }

    /**
     * Discard everything and reset as if default-constructed.
     */
    void reset()
    {
        samples.clear();
        boost::hana::unpack(boost::hana::keys(windows), [&] (auto... w) { (windows[w].reset(), ...); });
    }

    /**
     * Return the current value of the variance of the primary window.
     */
    value_type get() const { return get(primary_window); }

    /**
     * Return the variance of the given time window
     */
    template <std::size_t Millis>
    value_type get(sliding_time_window_tag_t<Millis> const w) const
    {
        return windows[w].variance();
    }

    /**
     * Return the mean of the primary window.
     */
    value_type mean() const { return mean(primary_window); }

    /**
     * Return the mean of the given time window
     */
    template <std::size_t Millis>
    value_type mean(sliding_time_window_tag_t<Millis> const w) const
    {
        return windows[w].mean();
    }

    /**
     * Return the total number of samples currently in the primary window.
     */
    std::size_t size() const { return size(primary_window); }

    /**
     * Return the total number of samples currently in the given window.
     */
    template <std::size_t Millis>
    std::size_t size(sliding_time_window_tag_t<Millis> const w) const
    {
        return windows[w].size();
    }

    /**
     * Return the total number of samples currently stored in the internal buffer.
     */
    std::size_t sample_buffer_size() const
    {
        return samples.size();
    }
};
static_assert(Estimator<sliding_time_window_variance_estimator_t<double, 1ul>>);


} // namespace livestats

