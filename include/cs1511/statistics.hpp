#pragma once

#include <cstdint>
#include <span>

namespace cs1511 {

// Aggregate of the five real-valued statistics.
// Every field is finite when construction succeeds.
struct RealSummary {
    double minimum;
    double maximum;
    double sum;
    double mean;
    double median;
};

// Aggregate of the five integer statistics.
// `sum` is exact. `mean` and `median` are computed in double precision.
struct IntegerSummary {
    std::int64_t minimum;
    std::int64_t maximum;
    std::int64_t sum;
    double mean;
    double median;
};

// Each real-valued function rejects an empty collection and any non-finite
// value (NaN or infinity) with std::invalid_argument.
// `sum` and `mean` also throw std::overflow_error when the running total is
// not finite. Accumulation is left to right, so a mathematically finite sum
// can still overflow. `median` copies the values and does not modify the caller.

[[nodiscard]] double minimum(std::span<const double> values);
[[nodiscard]] double maximum(std::span<const double> values);
[[nodiscard]] double sum(std::span<const double> values);
[[nodiscard]] double mean(std::span<const double> values);
[[nodiscard]] double median(std::span<const double> values);
[[nodiscard]] RealSummary summarize(std::span<const double> values);

// Integer functions reject an empty collection with std::invalid_argument.
// `sum` throws std::overflow_error instead of wrapping: signed overflow is
// undefined behavior, so the addition is checked before it happens.
// `mean` does not require the exact sum to fit in 64 bits; it accumulates in
// long double and may lose precision past the 53-bit integer range of double.
// `summarize` includes the exact sum, so it throws when that sum overflows.
// An even-sized median is the average of the two central values.

[[nodiscard]] std::int64_t minimum(std::span<const std::int64_t> values);
[[nodiscard]] std::int64_t maximum(std::span<const std::int64_t> values);
[[nodiscard]] std::int64_t sum(std::span<const std::int64_t> values);
[[nodiscard]] double mean(std::span<const std::int64_t> values);
[[nodiscard]] double median(std::span<const std::int64_t> values);
[[nodiscard]] IntegerSummary summarize(std::span<const std::int64_t> values);

}  // namespace cs1511
