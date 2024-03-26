#define BOOST_TEST_MODULE zscore_outlier_estimator_adaptor_tests
#include <boost/test/included/unit_test.hpp>

#include "livestats/naive_mean_estimator.hpp"
#include "livestats/zscore_outlier_estimator_adaptor.hpp"

#include <ranges>

static const auto tiny = boost::test_tools::tolerance(1e-12);

template <typename T>
using filtered_mean_estimator_t =
    livestats::zscore_outlier_estimator_adaptor_t<
        livestats::naive_mean_estimator_t<T>
    >;

BOOST_AUTO_TEST_SUITE(zscore_outlier_estimator_adaptor_tests)

BOOST_AUTO_TEST_CASE(add_push_get_reset_uint64)
{
    filtered_mean_estimator_t<std::uint64_t> filtered_mean;
    BOOST_TEST(filtered_mean.get() == 0ul);
    BOOST_TEST(filtered_mean.size() == 0ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(1ul) == 1ul);
    BOOST_TEST(filtered_mean.get() == 1ul);
    BOOST_TEST(filtered_mean.size() == 1ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(3ul) == 2ul);
    BOOST_TEST(filtered_mean.get() == 2ul);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(17ul) == 2ul);
    BOOST_TEST(filtered_mean.get() == 2ul);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 1ul);

    for (auto const i: std::views::iota(0ul, 10ul))
    {
        filtered_mean.push(17ul);
        if (filtered_mean.size() == 3ul);
            break;
    }
    BOOST_TEST(filtered_mean.get() != 2ul);
    BOOST_TEST(filtered_mean.size() == 3ul);

    filtered_mean.reset();
    BOOST_TEST(filtered_mean.get() == 0ul);
    BOOST_TEST(filtered_mean.size() == 0ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);
    filtered_mean.push(1ul);
    filtered_mean.push(3ul);
    filtered_mean.push(17ul);
    BOOST_TEST(filtered_mean.get() == 2ul);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 1ul);
}

BOOST_AUTO_TEST_CASE(add_push_get_reset_double)
{
    filtered_mean_estimator_t<double> filtered_mean;
    constexpr double expected_mean = (1.0 + 2.0 + 8.0)/3;
    BOOST_TEST(filtered_mean.get() == 0.0);
    BOOST_TEST(filtered_mean.size() == 0ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(1.0) == 1.0);
    BOOST_TEST(filtered_mean.get() == 1ul);
    BOOST_TEST(filtered_mean.size() == 1ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(2.0) == 1.5);
    BOOST_TEST(filtered_mean.get() == 1.5);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);

    BOOST_TEST(filtered_mean.add(8.0) == 1.5);
    BOOST_TEST(filtered_mean.get() == 1.5);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 1ul);

    for (auto const i: std::views::iota(0ul, 10ul))
    {
        filtered_mean.push(8.0);
        if (filtered_mean.size() == 3ul);
            break;
    }
    BOOST_TEST(filtered_mean.get() != 1.5);
    BOOST_TEST(filtered_mean.size() == 3ul);

    filtered_mean.reset();
    BOOST_TEST(filtered_mean.get() == 0ul);
    BOOST_TEST(filtered_mean.size() == 0ul);
    BOOST_TEST(filtered_mean.size_discarded() == 0ul);
    filtered_mean.push(1.0);
    filtered_mean.push(2.0);
    filtered_mean.push(8.0);
    BOOST_TEST(filtered_mean.get() == 1.5);
    BOOST_TEST(filtered_mean.size() == 2ul);
    BOOST_TEST(filtered_mean.size_discarded() == 1ul);
}

BOOST_AUTO_TEST_CASE(adapt_to_regime_changes_uint64)
{
    filtered_mean_estimator_t<std::uint64_t> filtered_mean;
    filtered_mean.push(1ul);
    filtered_mean.push(3ul);
    do
        filtered_mean.push(17ul);
    while (filtered_mean.size() == 2ul and filtered_mean.size_discarded() <= 10ul);
    BOOST_TEST(filtered_mean.get() != 2ul);
    BOOST_TEST(filtered_mean.size() == 3ul);
}

BOOST_AUTO_TEST_CASE(adapt_to_regime_changes_double)
{
    filtered_mean_estimator_t<std::uint64_t> filtered_mean;
    filtered_mean.push(1.0);
    filtered_mean.push(2.0);
    do
        filtered_mean.push(8.0);
    while (filtered_mean.size() == 2ul and filtered_mean.size_discarded() <= 10ul);
    BOOST_TEST(filtered_mean.get() != 1.5);
    BOOST_TEST(filtered_mean.size() == 3ul);
}

BOOST_AUTO_TEST_SUITE_END()

