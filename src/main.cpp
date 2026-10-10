#include "cs1511/algorithms.hpp"
#include "cs1511/input.hpp"
#include "cs1511/statistics.hpp"
#include "cs1511/text_analysis.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr int kQuit = 0;
constexpr int kStatistics = 1;
constexpr int kText = 2;
constexpr int kSearch = 3;
constexpr int kSort = 4;
constexpr int kNumbers = 5;

void print_intro() {
    std::cout << "UMD CS 1511 — Day 1: Programming Foundations\n"
              << "Statistics, text analysis, search, sort, GCD, and primality.\n"
              << "Library code computes. This program only reads lines and prints results.\n"
              << "Indexes are zero-based. Enter collections as space-separated tokens.\n\n";
}

void print_menu() {
    std::cout << "Menu\n"
              << "  1  Statistics\n"
              << "  2  Text analysis\n"
              << "  3  Search\n"
              << "  4  Sort\n"
              << "  5  GCD and primality\n"
              << "  0  Quit\n";
}

// Returns nullopt when standard input is exhausted.
[[nodiscard]] std::optional<std::string> read_line(std::string_view prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return std::nullopt;
    }
    return line;
}

void report_error(const std::exception& error) {
    std::cout << "Error: " << error.what() << '\n';
}

void print_reals(const std::vector<double>& values) {
    if (values.empty()) {
        std::cout << "(empty)";
        return;
    }
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            std::cout << ' ';
        }
        std::cout << values[index];
    }
}

void print_integers(const std::vector<std::int64_t>& values) {
    if (values.empty()) {
        std::cout << "(empty)";
        return;
    }
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            std::cout << ' ';
        }
        std::cout << values[index];
    }
}

void print_real_summary(const cs1511::RealSummary& summary) {
    std::cout << "Minimum: " << summary.minimum << '\n'
              << "Maximum: " << summary.maximum << '\n'
              << "Sum: " << summary.sum << '\n'
              << "Mean: " << summary.mean << '\n'
              << "Median: " << summary.median << '\n';
}

void print_integer_summary(const cs1511::IntegerSummary& summary) {
    std::cout << "Minimum: " << summary.minimum << '\n'
              << "Maximum: " << summary.maximum << '\n'
              << "Sum: " << summary.sum << '\n'
              << "Mean: " << summary.mean << '\n'
              << "Median: " << summary.median << '\n';
}

[[nodiscard]] bool run_statistics() {
    std::cout << "\n--- Statistics ---\n";
    const auto kind_line = read_line("Enter r for real numbers or i for 64-bit integers:\n");
    if (!kind_line) {
        return false;
    }

    cs1511::NumericKind kind = cs1511::NumericKind::Real;
    try {
        kind = cs1511::parse_numeric_kind(*kind_line);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    const auto value_line = read_line("Enter space-separated values:\n");
    if (!value_line) {
        return false;
    }

    try {
        if (kind == cs1511::NumericKind::Real) {
            const std::vector<double> values = cs1511::parse_reals(*value_line);
            std::cout << "Value type: real\nValues: ";
            print_reals(values);
            std::cout << '\n';
            print_real_summary(cs1511::summarize(values));
        } else {
            const std::vector<std::int64_t> values = cs1511::parse_integers(*value_line);
            std::cout << "Value type: integer\nValues: ";
            print_integers(values);
            std::cout << '\n';
            print_integer_summary(cs1511::summarize(values));
        }
    } catch (const std::exception& error) {
        report_error(error);
    }
    return true;
}

[[nodiscard]] bool run_text() {
    std::cout << "\n--- Text analysis ---\n";
    const auto line = read_line("Enter a line of text:\n");
    if (!line) {
        return false;
    }

    const std::string normalized = cs1511::normalize(*line);
    const std::size_t words = cs1511::count_words(*line);
    const std::size_t vowels = cs1511::count_vowels(*line);
    const std::map<std::string, std::size_t> frequencies = cs1511::word_frequency(*line);

    std::cout << "Input: " << *line << '\n';
    if (line->empty()) {
        std::cout << "Empty text: the normalized form is empty and every count is zero.\n";
    }
    std::cout << "Normalized: " << (normalized.empty() ? "(empty)" : normalized) << '\n'
              << "Word count: " << words << '\n'
              << "Vowel count: " << vowels << '\n'
              << "Frequencies:\n";
    if (frequencies.empty()) {
        std::cout << "  (none)\n";
    } else {
        for (const auto& entry : frequencies) {
            std::cout << "  " << entry.first << ' ' << entry.second << '\n';
        }
    }
    return true;
}

void print_search_result(std::string_view algorithm, std::optional<std::size_t> index) {
    std::cout << "Algorithm: " << algorithm << '\n';
    if (index) {
        std::cout << "Found at index " << *index << ".\n";
    } else {
        std::cout << "Not found.\n";
    }
}

[[nodiscard]] bool run_search() {
    std::cout << "\n--- Search ---\n";
    const auto collection_line = read_line("Enter space-separated integers:\n");
    if (!collection_line) {
        return false;
    }

    std::vector<std::int64_t> values;
    try {
        values = cs1511::parse_integers(*collection_line);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    const auto target_line = read_line("Enter the target integer:\n");
    if (!target_line) {
        return false;
    }

    std::vector<std::int64_t> targets;
    try {
        targets = cs1511::parse_integers(*target_line);
        if (targets.size() != 1) {
            throw cs1511::InputError{cs1511::InputFailure::MalformedNumber,
                                     "malformed numeric input: enter a single integer"};
        }
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    const auto algorithm_line =
        read_line("Enter 1 for linear search, 2 for iterative binary search, "
                  "or 3 for recursive binary search:\n");
    if (!algorithm_line) {
        return false;
    }

    int algorithm = 0;
    try {
        algorithm = cs1511::parse_menu_choice(*algorithm_line, 1, 3);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    std::cout << "Collection: ";
    print_integers(values);
    std::cout << "\nTarget: " << targets.front() << '\n';
    if (values.empty()) {
        std::cout << "Empty collection: a target cannot occur in an empty collection.\n";
    }

    try {
        const std::int64_t target = targets.front();
        switch (algorithm) {
        case 1:
            print_search_result("linear search", cs1511::linear_search(values, target));
            break;
        case 2:
            print_search_result("iterative binary search",
                                cs1511::binary_search_iterative(values, target));
            break;
        case 3:
            print_search_result("recursive binary search",
                                cs1511::binary_search_recursive(values, target));
            break;
        default:
            break;
        }
    } catch (const std::exception& error) {
        report_error(error);
    }
    return true;
}

[[nodiscard]] bool run_sort() {
    std::cout << "\n--- Sort ---\n";
    const auto collection_line = read_line("Enter space-separated integers:\n");
    if (!collection_line) {
        return false;
    }

    std::vector<std::int64_t> values;
    try {
        values = cs1511::parse_integers(*collection_line);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    const auto algorithm_line =
        read_line("Enter 1 for selection sort or 2 for insertion sort:\n");
    if (!algorithm_line) {
        return false;
    }

    int algorithm = 0;
    try {
        algorithm = cs1511::parse_menu_choice(*algorithm_line, 1, 2);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    std::cout << "Input: ";
    print_integers(values);
    std::cout << '\n';
    if (values.empty()) {
        std::cout << "Empty collection: there are no values to sort.\n";
    }

    if (algorithm == 1) {
        cs1511::selection_sort(values);
        std::cout << "Algorithm: selection sort\n";
    } else {
        cs1511::insertion_sort(values);
        std::cout << "Algorithm: insertion sort\n";
    }
    std::cout << "Sorted: ";
    print_integers(values);
    std::cout << '\n';
    return true;
}

[[nodiscard]] bool run_numbers() {
    std::cout << "\n--- GCD and primality ---\n";
    const auto choice_line =
        read_line("Enter 1 for the Euclidean GCD or 2 for a primality test:\n");
    if (!choice_line) {
        return false;
    }

    int choice = 0;
    try {
        choice = cs1511::parse_menu_choice(*choice_line, 1, 2);
    } catch (const std::exception& error) {
        report_error(error);
        return true;
    }

    if (choice == 1) {
        const auto line = read_line("Enter two space-separated integers:\n");
        if (!line) {
            return false;
        }
        try {
            const std::vector<std::int64_t> values = cs1511::parse_integers(*line);
            if (values.size() != 2) {
                throw cs1511::InputError{cs1511::InputFailure::MalformedNumber,
                                         "malformed numeric input: enter exactly two integers"};
            }
            const std::int64_t divisor = cs1511::gcd(values[0], values[1]);
            std::cout << "GCD(" << values[0] << ", " << values[1] << ") = " << divisor << '\n';
        } catch (const std::exception& error) {
            report_error(error);
        }
        return true;
    }

    const auto line = read_line("Enter one integer:\n");
    if (!line) {
        return false;
    }
    try {
        const std::vector<std::int64_t> values = cs1511::parse_integers(*line);
        if (values.size() != 1) {
            throw cs1511::InputError{cs1511::InputFailure::MalformedNumber,
                                     "malformed numeric input: enter a single integer"};
        }
        const bool prime = cs1511::is_prime(values.front());
        std::cout << values.front() << (prime ? " is prime.\n" : " is not prime.\n");
    } catch (const std::exception& error) {
        report_error(error);
    }
    return true;
}

}  // namespace

int main() {
    print_intro();

    while (true) {
        print_menu();
        const auto line = read_line("Enter a menu choice:\n");
        if (!line) {
            break;
        }

        int choice = 0;
        try {
            choice = cs1511::parse_menu_choice(*line, kQuit, kNumbers);
        } catch (const std::exception& error) {
            report_error(error);
            std::cout << '\n';
            continue;
        }

        bool keep_running = true;
        switch (choice) {
        case kQuit:
            keep_running = false;
            break;
        case kStatistics:
            keep_running = run_statistics();
            break;
        case kText:
            keep_running = run_text();
            break;
        case kSearch:
            keep_running = run_search();
            break;
        case kSort:
            keep_running = run_sort();
            break;
        case kNumbers:
            keep_running = run_numbers();
            break;
        default:
            break;
        }

        if (!keep_running) {
            break;
        }
        std::cout << '\n';
    }

    std::cout << "Goodbye.\n";
    return 0;
}
