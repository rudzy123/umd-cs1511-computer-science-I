#include "cs1511/input.hpp"

#include <cctype>
#include <charconv>
#include <cmath>
#include <system_error>
#include <utility>

namespace cs1511 {
namespace {

[[nodiscard]] std::vector<std::string> split_tokens(std::string_view text) {
    std::vector<std::string> tokens;
    std::string current;
    for (const char character : text) {
        if (std::isspace(static_cast<unsigned char>(character)) != 0) {
            if (!current.empty()) {
                tokens.push_back(std::move(current));
                current.clear();
            }
        } else {
            current.push_back(character);
        }
    }
    if (!current.empty()) {
        tokens.push_back(std::move(current));
    }
    return tokens;
}

[[nodiscard]] const char* skip_leading_plus(const char* begin, const char* end, bool allow_dot) {
    if (begin < end && *begin == '+') {
        const char* next = begin + 1;
        if (next < end && (std::isdigit(static_cast<unsigned char>(*next)) != 0 ||
                           (allow_dot && *next == '.'))) {
            return next;
        }
    }
    return begin;
}

[[nodiscard]] InputError malformed(const std::string& token) {
    return InputError{InputFailure::MalformedNumber, "malformed numeric input: '" + token + "'"};
}

}  // namespace

InputError::InputError(InputFailure failure, std::string message)
    : std::invalid_argument(std::move(message)), failure_(failure) {}

InputFailure InputError::failure() const noexcept {
    return failure_;
}

std::vector<double> parse_reals(std::string_view text) {
    const std::vector<std::string> tokens = split_tokens(text);
    std::vector<double> values;
    values.reserve(tokens.size());

    for (const std::string& token : tokens) {
        const char* begin = skip_leading_plus(token.data(), token.data() + token.size(), true);
        const char* end = token.data() + token.size();
        if (begin == end) {
            throw malformed(token);
        }

        double value = 0.0;
        const std::from_chars_result result =
            std::from_chars(begin, end, value, std::chars_format::general);
        if (result.ec == std::errc::result_out_of_range) {
            throw InputError{InputFailure::OutOfRange, "numeric value out of range: '" + token + "'"};
        }
        if (result.ec != std::errc{} || result.ptr != end || !std::isfinite(value)) {
            throw malformed(token);
        }
        values.push_back(value);
    }

    return values;
}

std::vector<std::int64_t> parse_integers(std::string_view text) {
    const std::vector<std::string> tokens = split_tokens(text);
    std::vector<std::int64_t> values;
    values.reserve(tokens.size());

    for (const std::string& token : tokens) {
        const char* begin = skip_leading_plus(token.data(), token.data() + token.size(), false);
        const char* end = token.data() + token.size();
        if (begin == end) {
            throw malformed(token);
        }

        std::int64_t value = 0;
        const std::from_chars_result result = std::from_chars(begin, end, value, 10);
        if (result.ec == std::errc::result_out_of_range) {
            throw InputError{InputFailure::OutOfRange,
                             "integer out of range: '" + token +
                                 "' does not fit in a 64-bit signed integer"};
        }
        if (result.ptr == begin || result.ec != std::errc{} || result.ptr != end) {
            throw malformed(token);
        }
        values.push_back(value);
    }

    return values;
}

int parse_menu_choice(std::string_view text, int minimum, int maximum) {
    const std::vector<std::string> tokens = split_tokens(text);
    if (tokens.size() != 1) {
        throw InputError{InputFailure::MalformedNumber,
                         "malformed numeric input: enter a single integer"};
    }

    const std::vector<std::int64_t> values = parse_integers(tokens.front());
    const std::int64_t choice = values.front();
    if (choice < minimum || choice > maximum) {
        throw InputError{InputFailure::InvalidChoice,
                         "invalid menu choice: enter an integer from " + std::to_string(minimum) +
                             " to " + std::to_string(maximum)};
    }
    return static_cast<int>(choice);
}

NumericKind parse_numeric_kind(std::string_view text) {
    const std::vector<std::string> tokens = split_tokens(text);
    if (tokens.size() != 1) {
        throw InputError{InputFailure::MalformedNumber,
                         "malformed numeric input: enter r for real numbers or i for integers"};
    }

    const std::string& token = tokens.front();
    if (token == "r" || token == "R") {
        return NumericKind::Real;
    }
    if (token == "i" || token == "I") {
        return NumericKind::Integer;
    }
    throw InputError{InputFailure::InvalidChoice,
                     "invalid choice: enter r for real numbers or i for integers"};
}

}  // namespace cs1511
