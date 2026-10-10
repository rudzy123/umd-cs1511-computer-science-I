#include "cs1511/statistics.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {

constexpr auto kMax = std::numeric_limits<std::int64_t>::max();
constexpr auto kMin = std::numeric_limits<std::int64_t>::min();

}  // namespace

TEST_CASE("real statistics cover the usual and boundary collections", "[statistics]") {
    SECTION("unsorted values keep the caller's order and report every statistic") {
        std::vector<double> values{4.0, 1.0, 3.0, 2.0};
        const cs1511::RealSummary summary = cs1511::summarize(values);

        REQUIRE(summary.minimum == 1.0);
        REQUIRE(summary.maximum == 4.0);
        REQUIRE(summary.sum == 10.0);
        REQUIRE(summary.mean == 2.5);
        REQUIRE(summary.median == 2.5);
        REQUIRE(values == std::vector<double>{4.0, 1.0, 3.0, 2.0});
    }

    SECTION("one element is its own minimum, maximum, sum, mean, and median") {
        const std::vector<double> values{42.0};
        REQUIRE(cs1511::minimum(values) == 42.0);
        REQUIRE(cs1511::maximum(values) == 42.0);
        REQUIRE(cs1511::sum(values) == 42.0);
        REQUIRE(cs1511::mean(values) == 42.0);
        REQUIRE(cs1511::median(values) == 42.0);
    }

    SECTION("duplicates do not change the median") {
        const std::vector<double> values{5.0, 5.0, 5.0};
        REQUIRE(cs1511::median(values) == 5.0);
        REQUIRE(cs1511::mean(values) == 5.0);
    }

    SECTION("an odd count selects the central value after sorting") {
        const std::vector<double> values{9.0, 1.0, 5.0, 3.0, 7.0};
        REQUIRE(cs1511::median(values) == 5.0);
    }

    SECTION("negative values are ordered by signed magnitude") {
        const std::vector<double> values{-5.0, -1.0, -3.0};
        REQUIRE(cs1511::minimum(values) == -5.0);
        REQUIRE(cs1511::maximum(values) == -1.0);
        REQUIRE(cs1511::sum(values) == -9.0);
        REQUIRE(cs1511::mean(values) == -3.0);
        REQUIRE(cs1511::median(values) == -3.0);
    }

    SECTION("the mean of two integers stored as doubles is not truncated") {
        const std::vector<double> values{1.0, 2.0};
        REQUIRE(cs1511::mean(values) == 1.5);
    }
}

TEST_CASE("real statistics reject empty and non-finite input", "[statistics]") {
    const std::vector<double> empty;
    REQUIRE_THROWS_WITH(cs1511::minimum(empty), "statistics require a non-empty collection");
    REQUIRE_THROWS_AS(cs1511::maximum(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1511::sum(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1511::mean(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1511::median(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1511::summarize(empty), std::invalid_argument);

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    REQUIRE_THROWS_WITH(cs1511::sum(std::vector<double>{1.0, nan, 2.0}),
                        "statistics require finite values");
    REQUIRE_THROWS_AS(cs1511::median(std::vector<double>{infinity}), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1511::minimum(std::vector<double>{-infinity, 1.0}), std::invalid_argument);
}

TEST_CASE("real sum and median treat overflow differently", "[statistics]") {
    const double huge = std::numeric_limits<double>::max();
    REQUIRE_THROWS_AS(cs1511::sum(std::vector<double>{huge, huge}), std::overflow_error);
    REQUIRE_THROWS_AS(cs1511::mean(std::vector<double>{huge, huge}), std::overflow_error);
    REQUIRE(cs1511::median(std::vector<double>{huge, huge}) == huge);
    REQUIRE(cs1511::minimum(std::vector<double>{huge, 1.0}) == 1.0);
}

TEST_CASE("integer statistics cover edges that floating types hide", "[statistics]") {
    SECTION("mixed signs and an exact sum") {
        const std::vector<std::int64_t> values{-2, 8, -2, 5, 0};
        const cs1511::IntegerSummary summary = cs1511::summarize(values);
        REQUIRE(summary.minimum == -2);
        REQUIRE(summary.maximum == 8);
        REQUIRE(summary.sum == 9);
        REQUIRE(summary.mean == Catch::Approx(1.8));
        REQUIRE(summary.median == 0.0);
    }

    SECTION("one element and duplicates") {
        REQUIRE(cs1511::median(std::vector<std::int64_t>{7}) == 7.0);
        REQUIRE(cs1511::sum(std::vector<std::int64_t>{4, 4, 4, 4}) == 16);
        REQUIRE(cs1511::median(std::vector<std::int64_t>{4, 4, 4, 4}) == 4.0);
    }

    SECTION("already sorted and reverse-sorted collections") {
        REQUIRE(cs1511::median(std::vector<std::int64_t>{1, 2, 3, 4}) == 2.5);
        REQUIRE(cs1511::median(std::vector<std::int64_t>{4, 3, 2, 1}) == 2.5);
        REQUIRE(cs1511::minimum(std::vector<std::int64_t>{4, 3, 2, 1}) == 1);
        REQUIRE(cs1511::maximum(std::vector<std::int64_t>{1, 2, 3, 4}) == 4);
    }

    SECTION("limits that still fit") {
        REQUIRE(cs1511::sum(std::vector<std::int64_t>{kMax, -1}) == kMax - 1);
        REQUIRE(cs1511::sum(std::vector<std::int64_t>{kMin, 1}) == kMin + 1);
        REQUIRE(cs1511::sum(std::vector<std::int64_t>{kMax, kMin}) == -1);
        REQUIRE(cs1511::sum(std::vector<std::int64_t>{kMax, -10, 5}) == kMax - 5);
        REQUIRE(cs1511::minimum(std::vector<std::int64_t>{kMin, kMax}) == kMin);
        REQUIRE(cs1511::maximum(std::vector<std::int64_t>{kMin, kMax}) == kMax);
    }

    SECTION("the exact sum overflows while the double mean still exists") {
        REQUIRE_THROWS_WITH(cs1511::sum(std::vector<std::int64_t>{kMax, 1}), "integer sum overflow");
        REQUIRE_THROWS_AS(cs1511::sum(std::vector<std::int64_t>{kMin, -1}), std::overflow_error);
        REQUIRE_THROWS_AS(cs1511::sum(std::vector<std::int64_t>{kMax, -5, 10}), std::overflow_error);
        REQUIRE_THROWS_AS(cs1511::summarize(std::vector<std::int64_t>{kMax, 1}), std::overflow_error);

        const double average = cs1511::mean(std::vector<std::int64_t>{kMax, kMax});
        REQUIRE(average == Catch::Approx(static_cast<double>(kMax)));
    }

    SECTION("an empty integer collection is rejected") {
        const std::vector<std::int64_t> empty;
        REQUIRE_THROWS_AS(cs1511::minimum(empty), std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::maximum(empty), std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::sum(empty), std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::mean(empty), std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::median(empty), std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::summarize(empty), std::invalid_argument);
    }
}
