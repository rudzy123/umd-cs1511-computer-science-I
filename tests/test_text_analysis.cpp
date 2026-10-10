#include "cs1511/text_analysis.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <string>
#include <vector>

TEST_CASE("normalize lowercases words and collapses separators", "[text]") {
    REQUIRE(cs1511::normalize("Hello, hello World!") == "hello hello world");
    REQUIRE(cs1511::normalize("   Rock-and-Roll   ") == "rock and roll");
    REQUIRE(cs1511::normalize("don't") == "don t");
    REQUIRE(cs1511::normalize("CS 1511") == "cs 1511");
    REQUIRE(cs1511::normalize("") == "");
    REQUIRE(cs1511::normalize("   \t  ") == "");
    REQUIRE(cs1511::normalize("...") == "");
}

TEST_CASE("word counts cover empty, punctuation, and duplicates", "[text]") {
    REQUIRE(cs1511::count_words("") == 0);
    REQUIRE(cs1511::count_words(" \n\t ") == 0);
    REQUIRE(cs1511::count_words("!!!") == 0);
    REQUIRE(cs1511::count_words("Hello") == 1);
    REQUIRE(cs1511::count_words("  hello!!!") == 1);
    REQUIRE(cs1511::count_words("Hello, hello World!") == 3);
    REQUIRE(cs1511::count_words("The the THE") == 3);
}

TEST_CASE("vowel counts are case-insensitive and ignore y", "[text]") {
    REQUIRE(cs1511::count_vowels("") == 0);
    REQUIRE(cs1511::count_vowels("rhythm") == 0);
    REQUIRE(cs1511::count_vowels("sky") == 0);
    REQUIRE(cs1511::count_vowels("AEIOUaeiou y") == 10);
    REQUIRE(cs1511::count_vowels("Hello, hello World!") == 5);
    REQUIRE(cs1511::count_vowels("Computer Science") == 6);
    REQUIRE(cs1511::count_vowels("CS 1511") == 0);
}

TEST_CASE("frequency maps are case-insensitive and alphabetically ordered", "[text]") {
    SECTION("repeated words combine") {
        const std::map<std::string, std::size_t> frequencies =
            cs1511::word_frequency("banana Apple banana APPLE apple");
        REQUIRE(frequencies.size() == 2);
        REQUIRE(frequencies.at("apple") == 3);
        REQUIRE(frequencies.at("banana") == 2);

        std::vector<std::string> keys;
        for (const auto& entry : frequencies) {
            keys.push_back(entry.first);
        }
        REQUIRE(keys == std::vector<std::string>{"apple", "banana"});
    }

    SECTION("empty and punctuation-only text produce an empty map") {
        REQUIRE(cs1511::word_frequency("").empty());
        REQUIRE(cs1511::word_frequency("... ---").empty());
    }

    SECTION("digits stay attached to a word") {
        const auto frequencies = cs1511::word_frequency("CS1511 cs1511");
        REQUIRE(frequencies.size() == 1);
        REQUIRE(frequencies.at("cs1511") == 2);
    }
}
