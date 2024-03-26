#define BOOST_TEST_MODULE sliding_window_mean_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include "livestats/sliding_window_mean_estimator.hpp"

BOOST_AUTO_TEST_SUITE(sliding_window_mean_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::sliding_window_mean_estimator_t<std::uint64_t> mean(3ul);
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.full() == false);

    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.full() == false);
    BOOST_TEST(mean.oldest() == 1ul);

    BOOST_TEST(mean.add(3ul) == 2ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.full() == false);
    BOOST_TEST(mean.oldest() == 1ul);

    BOOST_TEST(mean.add(17ul) == 7ul);
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 1ul);

    BOOST_TEST(mean.add(1ul) == 7ul);
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 3ul);

    BOOST_TEST(mean.add(1ul) == 6ul);
    BOOST_TEST(mean.get() == 6ul);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 17ul);

    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 1ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.full() == false);
    mean.push(1ul);
    mean.push(3ul);
    mean.push(17ul);
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);
    mean.push(1ul);
    mean.push(1ul);
    mean.push(1ul);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::sliding_window_mean_estimator_t<double> mean(3ul);
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.full() == false);

    BOOST_TEST(mean.add(1.0) == 1.0);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);
    BOOST_TEST(mean.full() == false);
    BOOST_TEST(mean.oldest() == 1.0);

    BOOST_TEST(mean.add(2.0) == 1.5);
    BOOST_TEST(mean.get() == 1.5);
    BOOST_TEST(mean.size() == 2ul);
    BOOST_TEST(mean.full() == false);
    BOOST_TEST(mean.oldest() == 1.0);

    BOOST_TEST(mean.add(12.0) == 5.0);
    BOOST_TEST(mean.get() == 5.0);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 1.0);

    BOOST_TEST(mean.add(4.5) == 18.5/3);
    BOOST_TEST(mean.get() == 18.5/3);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 2.0);

    BOOST_TEST(mean.add(4.5) == 7.0);
    BOOST_TEST(mean.get() == 7.0);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 12.0);

    BOOST_TEST(mean.add(4.5) == 4.5);
    BOOST_TEST(mean.get() == 4.5);
    BOOST_TEST(mean.size() == 3ul);
    BOOST_TEST(mean.full() == true);
    BOOST_TEST(mean.oldest() == 4.5);

    mean.reset();
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    BOOST_TEST(mean.full() == false);
    mean.push(1.0);
    mean.push(2.0);
    mean.push(12.0);
    BOOST_TEST(mean.get() == 5.0);
    BOOST_TEST(mean.size() == 3ul);
    mean.push(4.5);
    mean.push(4.5);
    mean.push(4.5);
    BOOST_TEST(mean.get() == 4.5);
    BOOST_TEST(mean.size() == 3ul);
}

BOOST_AUTO_TEST_SUITE_END()
