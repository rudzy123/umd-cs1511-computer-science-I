#include "cs1511/input.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <cstdint>
#include <utility>
#include <vector>

namespace {

template <typename Action>
cs1511::InputFailure failure_kind(Action&& action) {
    try {
        std::forward<Action>(action)();
    } catch (const cs1511::InputError& error) {
        return error.failure();
    }
    FAIL("expected cs1511::InputError");
    return cs1511::InputFailure::MalformedNumber;
}

}  // namespace

TEST_CASE("real parsing accepts decimals and rejects bad tokens", "[input]") {
    REQUIRE(cs1511::parse_reals("1 2 3") == std::vector<double>{1.0, 2.0, 3.0});
    REQUIRE(cs1511::parse_reals("  1.5\t-2  +3.25 ") == std::vector<double>{1.5, -2.0, 3.25});
    REQUIRE(cs1511::parse_reals(".5 5. 1.5e1") == std::vector<double>{0.5, 5.0, 15.0});
    REQUIRE(cs1511::parse_reals("").empty());
    REQUIRE(cs1511::parse_reals("   \t").empty());

    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("1 2 abc"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("1.2.3"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("nan"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("inf"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("--1"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_reals("1e9999"); }) ==
            cs1511::InputFailure::OutOfRange);
}

TEST_CASE("integer parsing is base 10 and range-checked", "[input]") {
    REQUIRE(cs1511::parse_integers("48 18") == std::vector<std::int64_t>{48, 18});
    REQUIRE(cs1511::parse_integers("+7 -4") == std::vector<std::int64_t>{7, -4});
    REQUIRE(cs1511::parse_integers("9223372036854775807") ==
            std::vector<std::int64_t>{9223372036854775807LL});
    REQUIRE(cs1511::parse_integers("-9223372036854775808") ==
            std::vector<std::int64_t>{-9223372036854775807LL - 1});
    REQUIRE(cs1511::parse_integers("").empty());

    REQUIRE(failure_kind([] { (void)cs1511::parse_integers("18.5"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_integers("12abc"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_integers("++5"); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_integers("9223372036854775808"); }) ==
            cs1511::InputFailure::OutOfRange);
    REQUIRE(failure_kind([] { (void)cs1511::parse_integers("-9223372036854775809"); }) ==
            cs1511::InputFailure::OutOfRange);
    REQUIRE_THROWS_WITH(cs1511::parse_integers("1e2"), "malformed numeric input: '1e2'");
}

TEST_CASE("menu choices distinguish malformed text from an out-of-range choice", "[input]") {
    REQUIRE(cs1511::parse_menu_choice("0", 0, 5) == 0);
    REQUIRE(cs1511::parse_menu_choice("  4 ", 0, 5) == 4);
    REQUIRE(cs1511::parse_menu_choice("+3", 0, 5) == 3);

    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("", 0, 5); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("abc", 0, 5); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("1 2", 0, 5); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("1.0", 0, 5); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("9", 0, 5); }) ==
            cs1511::InputFailure::InvalidChoice);
    REQUIRE(failure_kind([] { (void)cs1511::parse_menu_choice("-1", 0, 5); }) ==
            cs1511::InputFailure::InvalidChoice);
    REQUIRE(failure_kind([] {
                (void)cs1511::parse_menu_choice("999999999999999999999", 0, 5);
            }) == cs1511::InputFailure::OutOfRange);

    REQUIRE_THROWS_WITH(cs1511::parse_menu_choice("9", 0, 5),
                        "invalid menu choice: enter an integer from 0 to 5");
}

TEST_CASE("numeric kind accepts only r and i", "[input]") {
    REQUIRE(cs1511::parse_numeric_kind("r") == cs1511::NumericKind::Real);
    REQUIRE(cs1511::parse_numeric_kind("R") == cs1511::NumericKind::Real);
    REQUIRE(cs1511::parse_numeric_kind(" i ") == cs1511::NumericKind::Integer);
    REQUIRE(cs1511::parse_numeric_kind("I") == cs1511::NumericKind::Integer);

    REQUIRE(failure_kind([] { (void)cs1511::parse_numeric_kind(""); }) ==
            cs1511::InputFailure::MalformedNumber);
    REQUIRE(failure_kind([] { (void)cs1511::parse_numeric_kind("real"); }) ==
            cs1511::InputFailure::InvalidChoice);
    REQUIRE(failure_kind([] { (void)cs1511::parse_numeric_kind("x"); }) ==
            cs1511::InputFailure::InvalidChoice);
}
