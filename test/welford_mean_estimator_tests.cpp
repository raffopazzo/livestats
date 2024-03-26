#define BOOST_TEST_MODULE welford_mean_estimator_tests
#include <boost/test/included/unit_test.hpp>

#include "livestats/welford_mean_estimator.hpp"

#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/multiprecision/cpp_int.hpp>

static const auto tiny = boost::test_tools::tolerance(1e-12);

BOOST_AUTO_TEST_SUITE(welford_mean_estimator_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    livestats::welford_mean_estimator_t<std::uint64_t> mean;
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);

    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);

    BOOST_TEST(mean.add(3ul) == 2ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 2ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0ul);
    BOOST_TEST(mean.size() == 0ul);
    mean.push(1ul);
    mean.push(3ul);
    BOOST_TEST(mean.get() == 2ul);
    BOOST_TEST(mean.size() == 2ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    livestats::welford_mean_estimator_t<double> mean;
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);

    BOOST_TEST(mean.add(1.0) == 1.0);
    BOOST_TEST(mean.get() == 1ul);
    BOOST_TEST(mean.size() == 1ul);

    BOOST_TEST(mean.add(2.0) == 1.5);
    BOOST_TEST(mean.get() == 1.5);
    BOOST_TEST(mean.size() == 2ul);

    mean.reset();
    BOOST_TEST(mean.get() == 0.0);
    BOOST_TEST(mean.size() == 0ul);
    mean.push(1.0);
    mean.push(2.0);
    BOOST_TEST(mean.get() == 1.5);
    BOOST_TEST(mean.size() == 2ul);
}

BOOST_AUTO_TEST_CASE(rounding_uint64)
{
    livestats::welford_mean_estimator_t<std::uint64_t> mean;
    // for integral types, `mean({1,3,3}) == 2`
    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.add(3ul) == 2ul); // total sum is even, result is exact
    BOOST_TEST(mean.add(3ul) == 2ul); // total sum is odd, result rounded towards previous result: 2.333 -> 2

    // but `mean({3,3,1}) == 3`
    mean.reset();
    BOOST_TEST(mean.add(3ul) == 3ul);
    BOOST_TEST(mean.add(3ul) == 3ul); // total sum is even, result is exact
    BOOST_TEST(mean.add(1ul) == 3ul); // total sum is odd, result rounded towards previous result, so 2.333 -> 3
}

BOOST_AUTO_TEST_CASE(edge_case_uint64_max_max_one)
{
    // edge case adding `max + max + 1`
    livestats::welford_mean_estimator_t<std::uint64_t> mean;

    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    auto const expected_mean = [&]
    {
        auto v = boost::multiprecision::cpp_int(max);
        v += boost::multiprecision::cpp_int(max);
        v += 1;
        v /= 3;
        // add 1 because we expect rounding towards `max` but `convert_to` truncates
        return v.template convert_to<std::uint64_t>() + 1ul;
    }();

    BOOST_TEST(mean.add(max) == max);
    BOOST_TEST(mean.add(max) == max);
    BOOST_TEST(mean.add(1ul) == expected_mean);
}

BOOST_AUTO_TEST_CASE(edge_case_uint64_one_max_max)
{
    // edge case adding `1 + max + max`
    livestats::welford_mean_estimator_t<std::uint64_t> mean;

    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    auto const expected_mean = [&]
    {
        auto v = boost::multiprecision::cpp_int(max);
        v += boost::multiprecision::cpp_int(max);
        v += 1;
        v /= 3;
        // `convert_to` truncates, which is fine in this case
        return v.template convert_to<std::uint64_t>();
    }();

    BOOST_TEST(mean.add(1ul) == 1ul);
    BOOST_TEST(mean.add(max) == (std::uint64_t(1ul) << 63));
    BOOST_TEST(mean.add(max) == expected_mean);
}

BOOST_AUTO_TEST_CASE(edge_case_double_max_max_one)
{
    // edge case taking mean of `max + max + 1`
    livestats::welford_mean_estimator_t<double> mean;

    constexpr auto max = std::numeric_limits<double>::max();
    auto const expected_mean = [&]
    {
        auto v = boost::multiprecision::cpp_int(max);
        v += boost::multiprecision::cpp_int(max);
        v += 1;
        v /= 3;
        return v.template convert_to<double>();
    }();

    BOOST_TEST(mean.add(max) == max);
    BOOST_TEST(mean.add(max) == max);
    BOOST_TEST(boost::math::relative_difference(mean.add(1.0), expected_mean) == 0.0, tiny);
}

BOOST_AUTO_TEST_CASE(edge_case_double_one_max_max)
{
    // edge case taking mean of `max + max + 1`
    livestats::welford_mean_estimator_t<double> mean;

    constexpr auto max = std::numeric_limits<double>::max();
    auto const expected_mean = [&]
    {
        auto v = boost::multiprecision::cpp_int(max);
        v += boost::multiprecision::cpp_int(max);
        v += 1;
        v /= 3;
        return v.template convert_to<double>();
    }();

    BOOST_TEST(mean.add(1.0) == 1.0);
    BOOST_TEST(mean.add(max) == (std::numeric_limits<double>::max() / 2)); // 1.0 is negligible and can be ignored
    BOOST_TEST(boost::math::relative_difference(mean.add(max), expected_mean) == 0.0, tiny);
}

BOOST_AUTO_TEST_SUITE_END()
