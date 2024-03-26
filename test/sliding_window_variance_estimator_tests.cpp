#define BOOST_TEST_MODULE sliding_window_variance_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include "livestats/sliding_window_variance_estimator.hpp"

static const auto tiny = boost::test_tools::tolerance(1e-12);

BOOST_AUTO_TEST_SUITE(sliding_window_variance_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::sliding_window_variance_estimator_t<std::uint64_t> variance(3ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);
    BOOST_TEST(variance.full() == false);

    BOOST_TEST(variance.add(1ul) == 0ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 1ul);
    BOOST_TEST(variance.full() == false);
    BOOST_TEST(variance.oldest() == 1ul);

    BOOST_TEST(variance.add(3ul) == 1ul);
    BOOST_TEST(variance.get() == 1ul);
    BOOST_TEST(variance.size() == 2ul);
    BOOST_TEST(variance.full() == false);
    BOOST_TEST(variance.oldest() == 1ul);

    BOOST_TEST(variance.add(17ul) == 50ul);
    BOOST_TEST(variance.get() == 50ul);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 1ul);

    BOOST_TEST(variance.add(1ul) == 50ul);
    BOOST_TEST(variance.get() == 50ul);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 3ul);

    BOOST_TEST(variance.add(1ul) == 56ul);
    BOOST_TEST(variance.get() == 56ul);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 17ul);

    BOOST_TEST(variance.add(1ul) == 0ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 1ul);

    variance.reset();
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);
    BOOST_TEST(variance.full() == false);
    variance.push(1ul);
    variance.push(3ul);
    variance.push(17ul);
    BOOST_TEST(variance.get() == 50ul);
    BOOST_TEST(variance.size() == 3ul);
    variance.push(1ul);
    variance.push(1ul);
    variance.push(1ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::sliding_window_variance_estimator_t<double> variance(3ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);
    BOOST_TEST(variance.full() == false);

    BOOST_TEST(variance.add(1.0) == 0.0);
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 1ul);
    BOOST_TEST(variance.full() == false);
    BOOST_TEST(variance.oldest() == 1.0);

    BOOST_TEST(variance.add(3.0) == 1.0);
    BOOST_TEST(variance.get() == 1.0);
    BOOST_TEST(variance.size() == 2ul);
    BOOST_TEST(variance.full() == false);
    BOOST_TEST(variance.oldest() == 1.0);

    BOOST_TEST(variance.add(17.0) == 50.666666666666, tiny);
    BOOST_TEST(variance.get() == 50.666666666666, tiny);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 1.0);

    BOOST_TEST(variance.add(1.0) == 50.666666666666, tiny);
    BOOST_TEST(variance.get() == 50.666666666666, tiny);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 3.0);

    BOOST_TEST(variance.add(1.0) == 56.888888888888, tiny);
    BOOST_TEST(variance.get() == 56.888888888888, tiny);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 17.0);

    BOOST_TEST(variance.add(1.0) == 0.0);
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 3ul);
    BOOST_TEST(variance.full() == true);
    BOOST_TEST(variance.oldest() == 1.0);

    variance.reset();
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 0ul);
    BOOST_TEST(variance.full() == false);
    variance.push(1.0);
    variance.push(3.0);
    variance.push(17.0);
    BOOST_TEST(variance.get() == 50.666666666666, tiny);
    BOOST_TEST(variance.size() == 3ul);
    variance.push(1.0);
    variance.push(1.0);
    variance.push(1.0);
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 3ul);
}

BOOST_AUTO_TEST_SUITE_END()
