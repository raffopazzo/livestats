#define BOOST_TEST_MODULE welford_variance_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include <boost/multiprecision/cpp_int.hpp>

#include "livestats/naive_mean_estimator.hpp"
#include "livestats/welford_variance_estimator.hpp"

static const auto tiny = boost::test_tools::tolerance(1e-12);

BOOST_AUTO_TEST_SUITE(welford_variance_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::welford_variance_estimator_t<std::uint64_t> variance;
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);

    BOOST_TEST(variance.add(1ul) == 0ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 1ul);

    BOOST_TEST(variance.add(3ul) == 1ul);
    BOOST_TEST(variance.get() == 1ul);
    BOOST_TEST(variance.size() == 2ul);

    BOOST_TEST(variance.add(7ul) == 7ul);
    BOOST_TEST(variance.get() == 7ul);
    BOOST_TEST(variance.size() == 3ul);

    variance.reset();
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);
    variance.push(1ul);
    variance.push(3ul);
    variance.push(7ul);
    BOOST_TEST(variance.get() == 7ul);
    BOOST_TEST(variance.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::welford_variance_estimator_t<double> variance;
    auto const sqr = [] (double const x) { return x * x; };
    constexpr double mean = 11.0/3;
    constexpr double expected_variance = (sqr(1.0 - mean) + sqr(3.0 - mean) + sqr(7.0 - mean)) / 3;
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 0ul);

    BOOST_TEST(variance.add(1.0) == 0.0);
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 1ul);

    BOOST_TEST(variance.add(3.0) == 1.0);
    BOOST_TEST(variance.get() == 1.0);
    BOOST_TEST(variance.size() == 2ul);

    BOOST_TEST(variance.add(7.0) == expected_variance, tiny);
    BOOST_TEST(variance.get() == expected_variance, tiny);
    BOOST_TEST(variance.size() == 3ul);

    variance.reset();
    BOOST_TEST(variance.get() == 0.0);
    BOOST_TEST(variance.size() == 0ul);
    variance.push(1.0);
    variance.push(3.0);
    variance.push(7.0);
    BOOST_TEST(variance.get() == expected_variance, tiny);
    BOOST_TEST(variance.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_multiprecision)
{
    using cpp_int = boost::multiprecision::cpp_int;
    using multiprecision_mean_estimator = livestats::naive_mean_estimator_t<cpp_int>;
    using multiprecision_variance_estimator_t =
        livestats::welford_variance_estimator_t<
            cpp_int,
            multiprecision_mean_estimator>;
    multiprecision_variance_estimator_t variance;
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);

    auto const two_to_the_128 = cpp_int("340282366920938463463374607431768211456");
    auto const two_to_the_64 = cpp_int("170141183460469231731687303715884105728");
    auto const expected = cpp_int("7237005577332262213973186563042994240829374041602535252466099000494570602496");

    BOOST_TEST(variance.add(two_to_the_128) == 0ul);
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 1ul);

    BOOST_TEST(variance.add(two_to_the_64) == expected);
    BOOST_TEST(variance.get() == expected);
    BOOST_TEST(variance.size() == 2ul);

    variance.reset();
    BOOST_TEST(variance.get() == 0ul);
    BOOST_TEST(variance.size() == 0ul);
    variance.push(two_to_the_128);
    variance.push(two_to_the_64);
    BOOST_TEST(variance.get() == expected);
    BOOST_TEST(variance.size() == 2ul);
}

BOOST_AUTO_TEST_SUITE_END()
