#include "cs1511/statistics.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace cs1511 {
namespace {

void validate_reals(std::span<const double> values) {
    if (values.empty()) {
        throw std::invalid_argument("statistics require a non-empty collection");
    }
    const bool has_non_finite = std::any_of(values.begin(), values.end(), [](double value) {
        return !std::isfinite(value);
    });
    if (has_non_finite) {
        throw std::invalid_argument("statistics require finite values");
    }
}

void validate_integers(std::span<const std::int64_t> values) {
    if (values.empty()) {
        throw std::invalid_argument("statistics require a non-empty collection");
    }
}

[[nodiscard]] std::int64_t checked_add(std::int64_t total, std::int64_t value) {
    constexpr auto kMax = std::numeric_limits<std::int64_t>::max();
    constexpr auto kMin = std::numeric_limits<std::int64_t>::min();

    if (value >= 0) {
        if (total > kMax - value) {
            throw std::overflow_error("integer sum overflow");
        }
    } else if (value == kMin) {
        // std::abs(INT64_MIN) does not fit in std::int64_t, and kMin - value would overflow.
        if (total < 0) {
            throw std::overflow_error("integer sum overflow");
        }
    } else if (total < kMin - value) {
        throw std::overflow_error("integer sum overflow");
    }

    return total + value;
}

[[nodiscard]] double average_of_neighbors(double left, double right) {
    // Halve before adding so a pair of large finite values does not overflow.
    return left / 2.0 + right / 2.0;
}

}  // namespace

double minimum(std::span<const double> values) {
    validate_reals(values);
    return *std::min_element(values.begin(), values.end());
}

double maximum(std::span<const double> values) {
    validate_reals(values);
    return *std::max_element(values.begin(), values.end());
}

double sum(std::span<const double> values) {
    validate_reals(values);
    // The seed must be 0.0. An integer seed would truncate every fractional part.
    const double total = std::accumulate(values.begin(), values.end(), 0.0);
    if (!std::isfinite(total)) {
        throw std::overflow_error("real sum is not finite");
    }
    return total;
}

double mean(std::span<const double> values) {
    const double total = sum(values);
    return total / static_cast<double>(values.size());
}

double median(std::span<const double> values) {
    validate_reals(values);
    std::vector<double> ordered(values.begin(), values.end());
    std::sort(ordered.begin(), ordered.end());

    const std::size_t mid = ordered.size() / 2;
    if (ordered.size() % 2 == 1) {
        return ordered[mid];
    }
    return average_of_neighbors(ordered[mid - 1], ordered[mid]);
}

RealSummary summarize(std::span<const double> values) {
    return RealSummary{
        minimum(values),
        maximum(values),
        sum(values),
        mean(values),
        median(values),
    };
}

std::int64_t minimum(std::span<const std::int64_t> values) {
    validate_integers(values);
    return *std::min_element(values.begin(), values.end());
}

std::int64_t maximum(std::span<const std::int64_t> values) {
    validate_integers(values);
    return *std::max_element(values.begin(), values.end());
}

std::int64_t sum(std::span<const std::int64_t> values) {
    validate_integers(values);
    std::int64_t total = 0;
    for (const std::int64_t value : values) {
        total = checked_add(total, value);
    }
    return total;
}

double mean(std::span<const std::int64_t> values) {
    validate_integers(values);
    long double total = 0.0L;
    for (const std::int64_t value : values) {
        total += static_cast<long double>(value);
    }
    return static_cast<double>(total / static_cast<long double>(values.size()));
}

double median(std::span<const std::int64_t> values) {
    validate_integers(values);
    std::vector<std::int64_t> ordered(values.begin(), values.end());
    std::sort(ordered.begin(), ordered.end());

    const std::size_t mid = ordered.size() / 2;
    if (ordered.size() % 2 == 1) {
        return static_cast<double>(ordered[mid]);
    }
    return average_of_neighbors(static_cast<double>(ordered[mid - 1]),
                                static_cast<double>(ordered[mid]));
}

IntegerSummary summarize(std::span<const std::int64_t> values) {
    return IntegerSummary{
        minimum(values),
        maximum(values),
        sum(values),
        mean(values),
        median(values),
    };
}

}  // namespace cs1511
