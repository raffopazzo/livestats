#define BOOST_TEST_MODULE sliding_time_window_mean_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include <thread>

#include "livestats/sliding_time_window_mean_estimator.hpp"

static const auto tiny = boost::test_tools::tolerance(1e-12);

BOOST_AUTO_TEST_SUITE(sliding_time_window_mean_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::sliding_time_window_mean_estimator_t<std::uint64_t, 1, 50, 100> mean;
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);

    BOOST_TEST(mean.add(2ul) == 2ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    // after 150ms all windows are now empty
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    auto const t0 = std::chrono::steady_clock::now();
    mean.advance(t0);
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);
    // and we can add a new sample again
    BOOST_TEST(mean.add(2ul) == 2ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);

    mean.push(1ul, t0);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    mean.push(3ul, t0 + std::chrono::microseconds(10));
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.sample_buffer_size() == 2ul);

    mean.push(17ul, t0 + std::chrono::microseconds(20));
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 7ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after 1.010 millis
    //  - the 1ms window contains only 2 samples
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'010));
    BOOST_TEST(mean.get() == 10ul);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 10ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 20us:
    //  - the 1ms window contains only 1 sample
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'020));
    BOOST_TEST(mean.get() == 17ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 17ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 30us:
    //  - the 1ms window contains is empty
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'050));
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after 50.010ms from the start:
    //  - the primary window is empty
    //  - the 50ms window contains only 2 samples
    //  - the 100ms window is still unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(50'010));
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 10ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 50ms:
    //  - the primary window is empty
    //  - the 50ms window is empty
    //  - the 100ms window contains only 2 samples
    //  - the internal buffer contains only 2 samples
    mean.advance(t0 + std::chrono::microseconds(100'010));
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 10ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.sample_buffer_size() == 2ul);

    // after 150ms from the start, everything is empty
    mean.advance(t0 + std::chrono::milliseconds(150));
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::sliding_time_window_mean_estimator_t<double, 1, 50, 100> mean;
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);

    BOOST_TEST(mean.add(2.0) == 2.0);
    BOOST_TEST(mean.get() == 2.0);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    // after 150ms all windows are now empty
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    auto const t0 = std::chrono::steady_clock::now();
    mean.advance(t0);
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);
    // and we can add a new sample again
    BOOST_TEST(mean.add(2.0) == 2.0);
    BOOST_TEST(mean.get() == 2.0);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);

    mean.push(1.0, t0);
    BOOST_TEST(mean.get() == 1.0);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 1.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 1.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 1.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 1ul);
    BOOST_TEST(mean.sample_buffer_size() == 1ul);

    mean.push(4.0, t0 + std::chrono::microseconds(10));
    BOOST_TEST(mean.get() == 2.5);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 2.5);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 2.5);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 2.5);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.sample_buffer_size() == 2ul);

    mean.push(17.0, t0 + std::chrono::microseconds(20));
    BOOST_TEST(mean.get() == 7.333333333333, tiny);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 7.333333333333, tiny);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7.333333333333, tiny);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7.333333333333, tiny);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after 1.010 millis
    //  - the 1ms window contains only 2 samples
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'010));
    BOOST_TEST(mean.get() == 10.5);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 10.5);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7.333333333333, tiny);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7.333333333333, tiny);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 20us:
    //  - the 1ms window contains only 1 sample
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'020));
    BOOST_TEST(mean.get() == 17.0);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 17.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7.333333333333, tiny);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7.333333333333, tiny);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 1ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 30us:
    //  - the 1ms window contains is empty
    //  - the 50ms and 100ms windows are left unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(1'050));
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 7.333333333333, tiny);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7.333333333333, tiny);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 3ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after 50.010ms from the start:
    //  - the primary window is empty
    //  - the 50ms window contains only 2 samples
    //  - the 100ms window is still unchanged
    //  - the internal buffer contains all sample (required by the 100ms) window
    mean.advance(t0 + std::chrono::microseconds(50'010));
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 10.5);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 7.333333333333, tiny);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 2ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 3ul);
    BOOST_TEST(mean.sample_buffer_size() == 3ul);

    // after further 50ms:
    //  - the primary window is empty
    //  - the 50ms window is empty
    //  - the 100ms window contains only 2 samples
    //  - the internal buffer contains only 2 samples
    mean.advance(t0 + std::chrono::microseconds(100'010));
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 10.5);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 2ul);
    BOOST_TEST(mean.sample_buffer_size() == 2ul);

    // after 150ms from the start, everything is empty
    mean.advance(t0 + std::chrono::milliseconds(150));
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<1>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<50>) == 0.0);
    BOOST_TEST(mean.get(livestats::sliding_time_window_tag<100>) == 0.0);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<1>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<50>) == 0ul);
    BOOST_TEST(mean.size(livestats::sliding_time_window_tag<100>) == 0ul);
    BOOST_TEST(mean.sample_buffer_size() == 0ul);
}

BOOST_AUTO_TEST_SUITE_END()
