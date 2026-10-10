#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <string_view>

namespace cs1511 {

// A word is a maximal run of ASCII letters and digits. Punctuation, including
// apostrophes and hyphens, separates words. Matching is case-insensitive:
// callers always see lowercase words. Empty text is valid and yields an empty
// result rather than an error. Only the ASCII vowels a, e, i, o, and u are
// counted; y is not a vowel.

[[nodiscard]] std::string normalize(std::string_view text);
[[nodiscard]] std::size_t count_words(std::string_view text);
[[nodiscard]] std::size_t count_vowels(std::string_view text);

// Keys are lowercase words. std::map keeps them in alphabetical order.
[[nodiscard]] std::map<std::string, std::size_t> word_frequency(std::string_view text);

}  // namespace cs1511
