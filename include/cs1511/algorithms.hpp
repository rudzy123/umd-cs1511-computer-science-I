#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>

namespace cs1511 {

template <typename T>
concept OrderedByLess = requires(const T& left, const T& right) {
    { left < right } -> std::convertible_to<bool>;
};

// Returns the index of the first element equal to `target`, or std::nullopt.
// An empty collection is a normal "not found" result.
template <std::ranges::contiguous_range R>
    requires std::equality_comparable<std::ranges::range_value_t<R>>
[[nodiscard]] std::optional<std::size_t> linear_search(
    const R& data, const std::ranges::range_value_t<R>& target) {
    const std::span<const std::ranges::range_value_t<R>> items{data};
    for (std::size_t index = 0; index < items.size(); ++index) {
        if (items[index] == target) {
            return index;
        }
    }
    return std::nullopt;
}

namespace detail {

template <OrderedByLess T>
[[nodiscard]] bool is_non_decreasing(std::span<const T> items) {
    for (std::size_t index = 1; index < items.size(); ++index) {
        if (items[index] < items[index - 1]) {
            return false;
        }
    }
    return true;
}

// `low` is inclusive and `high` is exclusive. Both are indexes into `items`,
// so the returned index does not need to be shifted by a sub-range offset.
// On a match the search continues through the left half, which keeps the
// leftmost result O(log n) even when every element is equal to the target.
template <OrderedByLess T>
[[nodiscard]] std::optional<std::size_t> binary_search_recursive_range(
    std::span<const T> items, const T& target, std::size_t low, std::size_t high) {
    if (low >= high) {
        return std::nullopt;
    }

    const std::size_t mid = low + (high - low) / 2;
    if (items[mid] < target) {
        return binary_search_recursive_range(items, target, mid + 1, high);
    }
    if (target < items[mid]) {
        return binary_search_recursive_range(items, target, low, mid);
    }

    if (const std::optional<std::size_t> earlier =
            binary_search_recursive_range(items, target, low, mid)) {
        return earlier;
    }
    return mid;
}

}  // namespace detail

// Binary search requires a non-decreasing collection and throws
// std::invalid_argument otherwise. The validation scan is O(n). The search
// itself is O(log n) and returns the leftmost match, which makes duplicate
// tests deterministic. An empty collection is non-decreasing and yields
// std::nullopt.

template <std::ranges::contiguous_range R>
    requires OrderedByLess<std::ranges::range_value_t<R>>
[[nodiscard]] std::optional<std::size_t> binary_search_iterative(
    const R& data, const std::ranges::range_value_t<R>& target) {
    const std::span<const std::ranges::range_value_t<R>> items{data};
    if (!detail::is_non_decreasing(items)) {
        throw std::invalid_argument("binary search requires a non-decreasing collection");
    }

    std::size_t low = 0;
    std::size_t high = items.size();
    std::optional<std::size_t> found;

    while (low < high) {
        const std::size_t mid = low + (high - low) / 2;
        if (items[mid] < target) {
            low = mid + 1;
        } else if (target < items[mid]) {
            high = mid;
        } else {
            found = mid;
            high = mid;
        }
    }

    return found;
}

template <std::ranges::contiguous_range R>
    requires OrderedByLess<std::ranges::range_value_t<R>>
[[nodiscard]] std::optional<std::size_t> binary_search_recursive(
    const R& data, const std::ranges::range_value_t<R>& target) {
    const std::span<const std::ranges::range_value_t<R>> items{data};
    if (!detail::is_non_decreasing(items)) {
        throw std::invalid_argument("binary search requires a non-decreasing collection");
    }
    return detail::binary_search_recursive_range(items, target, 0, items.size());
}

// In-place sorts. Empty and one-element collections are already sorted.
// Selection sort is not stable. Insertion sort is stable because an equal key
// does not move past the element already in the sorted prefix.
// The argument must be a mutable contiguous collection: vector, array, or span.

template <std::ranges::contiguous_range R>
    requires OrderedByLess<std::ranges::range_value_t<R>> &&
             std::permutable<std::ranges::iterator_t<R>>
void selection_sort(R& data) {
    std::span items{data};
    const std::size_t count = items.size();
    for (std::size_t i = 0; i + 1 < count; ++i) {
        std::size_t min_index = i;
        for (std::size_t j = i + 1; j < count; ++j) {
            if (items[j] < items[min_index]) {
                min_index = j;
            }
        }
        if (min_index != i) {
            std::swap(items[i], items[min_index]);
        }
    }
}

template <std::ranges::contiguous_range R>
    requires OrderedByLess<std::ranges::range_value_t<R>> &&
             std::permutable<std::ranges::iterator_t<R>>
void insertion_sort(R& data) {
    std::span items{data};
    for (std::size_t i = 1; i < items.size(); ++i) {
        auto key = std::move(items[i]);
        std::size_t j = i;
        while (j > 0 && key < items[j - 1]) {
            items[j] = std::move(items[j - 1]);
            --j;
        }
        items[j] = std::move(key);
    }
}

// Euclidean greatest common divisor of the absolute values.
// Throws std::invalid_argument for gcd(0, 0), which is undefined.
// Throws std::overflow_error if either argument is INT64_MIN, because its
// absolute value is not representable as std::int64_t.
[[nodiscard]] std::int64_t gcd(std::int64_t a, std::int64_t b);

// Trial division. Numbers less than 2, including negatives, are not prime.
[[nodiscard]] bool is_prime(std::int64_t value) noexcept;

}  // namespace cs1511
