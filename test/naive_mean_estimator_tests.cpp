#define BOOST_TEST_MODULE naive_mean_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include <boost/multiprecision/cpp_int.hpp>

#include "livestats/naive_mean_estimator.hpp"

static const auto tiny = boost::test_tools::tolerance(1e-12);

BOOST_AUTO_TEST_SUITE(naive_mean_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::naive_mean_estimator_t<std::uint64_t> mean;
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);

    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);

    BOOST_TEST(mean.add(3ul) == 2ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 2ul);

    BOOST_TEST(mean.add(17ul) == 7ul);
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    mean.push(1ul);
    mean.push(3ul);
    mean.push(17ul);
    BOOST_TEST(mean.get() == 7ul);
    BOOST_TEST(mean.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::naive_mean_estimator_t<double> mean;
    constexpr double expected_mean = (1.0 + 2.0 + 8.0)/3;
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);

    BOOST_TEST(mean.add(1.0) == 1.0);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);

    BOOST_TEST(mean.add(2.0) == 1.5);
    BOOST_TEST(mean.get() == 1.5);
    BOOST_TEST(mean.size() == 2ul);

    BOOST_TEST(mean.add(8.0) == expected_mean, tiny);
    BOOST_TEST(mean.get() == expected_mean, tiny);
    BOOST_TEST(mean.size() == 3ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    mean.push(1.0);
    mean.push(2.0);
    mean.push(8.0);
    BOOST_TEST(mean.get() == expected_mean, tiny);
    BOOST_TEST(mean.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_multiprecision)
{
    livestats::naive_mean_estimator_t<boost::multiprecision::cpp_int> mean;
    auto const two_to_the_128 = boost::multiprecision::cpp_int("340282366920938463463374607431768211456");
    auto const two_to_the_64 = boost::multiprecision::cpp_int("170141183460469231731687303715884105728");
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);

    BOOST_TEST(mean.add(two_to_the_128) == two_to_the_128);
    BOOST_TEST(mean.get() == two_to_the_128);
    BOOST_TEST(mean.size() == 1ul);

    BOOST_TEST(mean.add(0ul) == two_to_the_64);
    BOOST_TEST(mean.get() == two_to_the_64);
    BOOST_TEST(mean.size() == 2ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    mean.push(two_to_the_128);
    mean.push(0ul);
    BOOST_TEST(mean.get() == two_to_the_64);
    BOOST_TEST(mean.size() == 2ul);
}

BOOST_AUTO_TEST_SUITE_END()
