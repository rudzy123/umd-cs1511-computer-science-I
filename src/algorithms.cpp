#include "cs1511/algorithms.hpp"

#include <limits>
#include <stdexcept>

namespace cs1511 {
namespace {

[[nodiscard]] std::int64_t checked_absolute(std::int64_t value) {
    if (value == std::numeric_limits<std::int64_t>::min()) {
        throw std::overflow_error(
            "gcd absolute value of INT64_MIN is not representable as a 64-bit signed integer");
    }
    return value < 0 ? -value : value;
}

}  // namespace

std::int64_t gcd(std::int64_t a, std::int64_t b) {
    a = checked_absolute(a);
    b = checked_absolute(b);
    if (a == 0 && b == 0) {
        throw std::invalid_argument("gcd(0, 0) is undefined");
    }

    // Replace the larger value with the remainder until the remainder is zero.
    // gcd(48, 18) -> (18, 12) -> (12, 6) -> (6, 0) -> 6.
    while (b != 0) {
        const std::int64_t remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

bool is_prime(std::int64_t value) noexcept {
    if (value <= 1) {
        return false;
    }
    if (value <= 3) {
        return true;
    }
    if (value % 2 == 0 || value % 3 == 0) {
        return false;
    }

    // Compare against value / i rather than i * i, which would overflow near INT64_MAX.
    for (std::int64_t i = 5; i <= value / i; i += 6) {
        if (value % i == 0 || value % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

}  // namespace cs1511
