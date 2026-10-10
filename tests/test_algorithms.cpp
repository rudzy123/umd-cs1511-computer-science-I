#include "cs1511/algorithms.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace {

struct Item {
    int key;
    int order;

    bool operator<(const Item& other) const { return key < other.key; }
};

template <typename T>
void expect_both_sorts_match_std_sort(std::vector<T> values) {
    std::vector<T> sorted = values;
    std::sort(sorted.begin(), sorted.end());

    std::vector<T> selected = values;
    cs1511::selection_sort(selected);
    std::vector<T> inserted = values;
    cs1511::insertion_sort(inserted);

    REQUIRE(selected == sorted);
    REQUIRE(inserted == sorted);
}

}  // namespace

TEST_CASE("linear search reports the first match and accepts an empty collection", "[algorithms]") {
    const std::vector<std::int64_t> values{7, 1, 7, 9};

    REQUIRE(cs1511::linear_search(values, std::int64_t{7}) == 0);
    REQUIRE(cs1511::linear_search(values, std::int64_t{1}) == 1);
    REQUIRE(cs1511::linear_search(values, std::int64_t{9}) == 3);
    REQUIRE_FALSE(cs1511::linear_search(values, std::int64_t{4}).has_value());

    const std::vector<std::int64_t> one{5};
    REQUIRE(cs1511::linear_search(one, std::int64_t{5}) == 0);
    REQUIRE_FALSE(cs1511::linear_search(one, std::int64_t{0}).has_value());
    REQUIRE_FALSE(cs1511::linear_search(std::vector<std::int64_t>{}, std::int64_t{1}).has_value());

    const std::int64_t raw[]{10, 20, 30};
    REQUIRE(cs1511::linear_search(raw, std::int64_t{20}) == 1);

    const std::string word = "search";
    REQUIRE(cs1511::linear_search(word, 'a') == 2);
    REQUIRE_FALSE(cs1511::linear_search(word, 'z').has_value());

    const std::vector<std::int64_t> reverse{3, 2, 1};
    REQUIRE(cs1511::linear_search(reverse, std::int64_t{2}) == 1);
}

TEST_CASE("binary search finds the leftmost match only in non-decreasing data", "[algorithms]") {
    const std::vector<std::int64_t> values{1, 2, 2, 2, 9};

    SECTION("normal positions") {
        REQUIRE(cs1511::binary_search_iterative(values, std::int64_t{1}) == 0);
        REQUIRE(cs1511::binary_search_recursive(values, std::int64_t{1}) == 0);
        REQUIRE(cs1511::binary_search_iterative(values, std::int64_t{2}) == 1);
        REQUIRE(cs1511::binary_search_recursive(values, std::int64_t{2}) == 1);
        REQUIRE(cs1511::binary_search_iterative(values, std::int64_t{9}) == 4);
        REQUIRE(cs1511::binary_search_recursive(values, std::int64_t{9}) == 4);
    }

    SECTION("missing, empty, and one-element collections") {
        REQUIRE_FALSE(cs1511::binary_search_iterative(values, std::int64_t{7}).has_value());
        REQUIRE_FALSE(cs1511::binary_search_recursive(values, std::int64_t{7}).has_value());
        REQUIRE_FALSE(
            cs1511::binary_search_iterative(std::vector<std::int64_t>{}, std::int64_t{1})
                .has_value());
        REQUIRE_FALSE(
            cs1511::binary_search_recursive(std::vector<std::int64_t>{}, std::int64_t{1})
                .has_value());

        const std::vector<std::int64_t> one{4};
        REQUIRE(cs1511::binary_search_iterative(one, std::int64_t{4}) == 0);
        REQUIRE_FALSE(cs1511::binary_search_recursive(one, std::int64_t{5}).has_value());
    }

    SECTION("every element matches") {
        const std::vector<std::int64_t> duplicates{2, 2, 2, 2};
        REQUIRE(cs1511::binary_search_iterative(duplicates, std::int64_t{2}) == 0);
        REQUIRE(cs1511::binary_search_recursive(duplicates, std::int64_t{2}) == 0);
    }

    SECTION("targets outside the range") {
        REQUIRE_FALSE(cs1511::binary_search_iterative(values, std::int64_t{0}).has_value());
        REQUIRE_FALSE(cs1511::binary_search_recursive(values, std::int64_t{100}).has_value());
    }

    SECTION("an unsorted collection is invalid for both implementations") {
        const std::vector<std::int64_t> reverse{9, 4, 1};
        REQUIRE_THROWS_WITH(cs1511::binary_search_iterative(reverse, std::int64_t{4}),
                            "binary search requires a non-decreasing collection");
        REQUIRE_THROWS_AS(cs1511::binary_search_recursive(reverse, std::int64_t{4}),
                          std::invalid_argument);
        REQUIRE(cs1511::linear_search(reverse, std::int64_t{4}) == 1);

        const std::vector<std::int64_t> almost{1, 3, 2};
        REQUIRE_THROWS_AS(cs1511::binary_search_iterative(almost, std::int64_t{2}),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(cs1511::binary_search_recursive(almost, std::int64_t{2}),
                          std::invalid_argument);
    }

    SECTION("both searches agree with linear search on a sorted collection") {
        const std::vector<std::int64_t> sorted{1, 3, 5, 7, 9};
        for (std::int64_t target = 0; target <= 10; ++target) {
            const auto linear = cs1511::linear_search(sorted, target);
            REQUIRE(cs1511::binary_search_iterative(sorted, target) == linear);
            REQUIRE(cs1511::binary_search_recursive(sorted, target) == linear);
        }
    }
}

TEST_CASE("selection sort and insertion sort order the required collection shapes", "[algorithms]") {
    expect_both_sorts_match_std_sort<std::int64_t>({});
    expect_both_sorts_match_std_sort<std::int64_t>({1});
    expect_both_sorts_match_std_sort<std::int64_t>({1, 1});
    expect_both_sorts_match_std_sort<std::int64_t>({1, 2, 3, 4});
    expect_both_sorts_match_std_sort<std::int64_t>({4, 3, 2, 1});
    expect_both_sorts_match_std_sort<std::int64_t>({4, 4, 1, 4});
    expect_both_sorts_match_std_sort<std::int64_t>({-2, -8, 0, 5, -2});
    expect_both_sorts_match_std_sort<std::int64_t>({9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    expect_both_sorts_match_std_sort<std::string>({"pear", "apple", "plum"});

    std::int64_t raw[]{3, 1, 2};
    cs1511::selection_sort(raw);
    REQUIRE(raw[0] == 1);
    REQUIRE(raw[1] == 2);
    REQUIRE(raw[2] == 3);

    std::int64_t reversed[]{3, 2, 1};
    cs1511::insertion_sort(reversed);
    REQUIRE(reversed[0] == 1);
    REQUIRE(reversed[1] == 2);
    REQUIRE(reversed[2] == 3);
}

TEST_CASE("insertion sort is stable and selection sort is not", "[algorithms]") {
    const std::vector<Item> original{{4, 0}, {3, 1}, {2, 2}, {4, 3}, {1, 4}};

    std::vector<Item> inserted = original;
    cs1511::insertion_sort(inserted);
    REQUIRE(inserted[0].key == 1);
    REQUIRE(inserted[0].order == 4);
    REQUIRE(inserted[1].order == 2);
    REQUIRE(inserted[2].order == 1);
    REQUIRE(inserted[3].key == 4);
    REQUIRE(inserted[3].order == 0);
    REQUIRE(inserted[4].key == 4);
    REQUIRE(inserted[4].order == 3);

    std::vector<Item> selected = original;
    cs1511::selection_sort(selected);
    REQUIRE(selected[3].key == 4);
    REQUIRE(selected[3].order == 3);
    REQUIRE(selected[4].key == 4);
    REQUIRE(selected[4].order == 0);
}

TEST_CASE("Euclidean gcd handles signs, zero, and the 64-bit edge", "[algorithms]") {
    REQUIRE(cs1511::gcd(48, 18) == 6);
    REQUIRE(cs1511::gcd(18, 48) == 6);
    REQUIRE(cs1511::gcd(12, 12) == 12);
    REQUIRE(cs1511::gcd(17, 13) == 1);
    REQUIRE(cs1511::gcd(0, 5) == 5);
    REQUIRE(cs1511::gcd(5, 0) == 5);
    REQUIRE(cs1511::gcd(-48, 18) == 6);
    REQUIRE(cs1511::gcd(48, -18) == 6);
    REQUIRE(cs1511::gcd(-48, -18) == 6);
    REQUIRE(cs1511::gcd(0, -4) == 4);
    REQUIRE(cs1511::gcd(1, 1) == 1);
    REQUIRE(cs1511::gcd(std::numeric_limits<std::int64_t>::max(),
                        std::numeric_limits<std::int64_t>::max()) ==
            std::numeric_limits<std::int64_t>::max());

    REQUIRE_THROWS_WITH(cs1511::gcd(0, 0), "gcd(0, 0) is undefined");
    REQUIRE_THROWS_AS(cs1511::gcd(std::numeric_limits<std::int64_t>::min(), 2),
                      std::overflow_error);
    REQUIRE_THROWS_AS(cs1511::gcd(2, std::numeric_limits<std::int64_t>::min()),
                      std::overflow_error);
}

TEST_CASE("primality treats integers below 2 as composite and checks the usual cases",
          "[algorithms]") {
    REQUIRE_FALSE(cs1511::is_prime(-7));
    REQUIRE_FALSE(cs1511::is_prime(0));
    REQUIRE_FALSE(cs1511::is_prime(1));
    REQUIRE(cs1511::is_prime(2));
    REQUIRE(cs1511::is_prime(3));
    REQUIRE_FALSE(cs1511::is_prime(4));
    REQUIRE_FALSE(cs1511::is_prime(9));
    REQUIRE_FALSE(cs1511::is_prime(15));
    REQUIRE_FALSE(cs1511::is_prime(21));
    REQUIRE_FALSE(cs1511::is_prime(25));
    REQUIRE_FALSE(cs1511::is_prime(49));
    REQUIRE_FALSE(cs1511::is_prime(121));
    REQUIRE_FALSE(cs1511::is_prime(561));
    REQUIRE(cs1511::is_prime(97));
    REQUIRE(cs1511::is_prime(7919));
    REQUIRE(cs1511::is_prime(1'000'003));
    REQUIRE_FALSE(cs1511::is_prime(1'000'005));
    REQUIRE(cs1511::is_prime(2'147'483'647));
    REQUIRE_FALSE(cs1511::is_prime(std::numeric_limits<std::int64_t>::min()));
}
