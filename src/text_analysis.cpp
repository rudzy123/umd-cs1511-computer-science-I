#include "cs1511/text_analysis.hpp"

#include <cctype>
#include <utility>
#include <vector>

namespace cs1511 {
namespace {

[[nodiscard]] bool is_ascii_alphanumeric(unsigned char character) {
    return std::isalnum(character) != 0;
}

[[nodiscard]] char ascii_tolower(unsigned char character) {
    return static_cast<char>(std::tolower(character));
}

[[nodiscard]] bool is_ascii_vowel(unsigned char character) {
    switch (ascii_tolower(character)) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return true;
    default:
        return false;
    }
}

[[nodiscard]] std::vector<std::string> tokenize(std::string_view text) {
    std::vector<std::string> words;
    std::string current;

    for (const char character : text) {
        const auto byte = static_cast<unsigned char>(character);
        if (is_ascii_alphanumeric(byte)) {
            current.push_back(ascii_tolower(byte));
        } else if (!current.empty()) {
            words.push_back(std::move(current));
            current.clear();
        }
    }

    if (!current.empty()) {
        words.push_back(std::move(current));
    }
    return words;
}

}  // namespace

std::string normalize(std::string_view text) {
    const std::vector<std::string> words = tokenize(text);
    std::string normalized;
    for (std::size_t index = 0; index < words.size(); ++index) {
        if (index != 0) {
            normalized.push_back(' ');
        }
        normalized += words[index];
    }
    return normalized;
}

std::size_t count_words(std::string_view text) {
    return tokenize(text).size();
}

std::size_t count_vowels(std::string_view text) {
    std::size_t count = 0;
    for (const char character : text) {
        if (is_ascii_vowel(static_cast<unsigned char>(character))) {
            ++count;
        }
    }
    return count;
}

std::map<std::string, std::size_t> word_frequency(std::string_view text) {
    std::map<std::string, std::size_t> frequencies;
    for (const std::string& word : tokenize(text)) {
        ++frequencies[word];
    }
    return frequencies;
}

}  // namespace cs1511
