#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace cs1511 {

enum class InputFailure {
    MalformedNumber,
    InvalidChoice,
    OutOfRange,
};

enum class NumericKind {
    Real,
    Integer,
};

// Parse failure with a stable reason code. `what()` is safe to show to a user.
class InputError : public std::invalid_argument {
  public:
    InputError(InputFailure failure, std::string message);

    [[nodiscard]] InputFailure failure() const noexcept;

  private:
    InputFailure failure_;
};

// Empty or whitespace-only input is an empty collection, not a parse error.
// Tokens must be base-10. A leading '+' is accepted. Real tokens may use a
// decimal point and an exponent. Non-finite values and trailing junk are rejected.

[[nodiscard]] std::vector<double> parse_reals(std::string_view text);
[[nodiscard]] std::vector<std::int64_t> parse_integers(std::string_view text);

// Requires exactly one integer in the closed interval [minimum, maximum].
[[nodiscard]] int parse_menu_choice(std::string_view text, int minimum, int maximum);

// Accepts a single token: r/R for real values, or i/I for 64-bit integers.
[[nodiscard]] NumericKind parse_numeric_kind(std::string_view text);

}  // namespace cs1511
