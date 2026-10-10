# C++ syntax cheat sheet

Day 1 of the CS 1511 refresher. Snippets are modern C++20 and match the style used in this repository.

## Declarations

```cpp
int count = 0;                         // value is copied into count
const double pi = 3.141592653589793;  // readable, not assignable
std::size_t index = 0;                 // unsigned size and index type
std::int64_t exact = 42;              // fixed-width integer
double average = 0.0;                  // floating point; write 0.0, not 0
bool found = false;
std::string name = "Ada";
std::string_view view = name;         // non-owning view; name must outlive view
auto total = 0.0;                      // deduced as double from the initializer
```

## Loops

```cpp
for (std::size_t i = 0; i < values.size(); ++i) {
    total += values[i];
}

for (const double value : values) {  // read-only visit
    total += value;
}

std::size_t i = 0;
while (i < values.size() && values[i] != target) {
    ++i;
}

do {
    std::getline(std::cin, line);
} while (line.empty() && std::cin);
```

## Conditionals

```cpp
if (values.empty()) {
    throw std::invalid_argument("statistics require a non-empty collection");
} else if (value < 0) {
    value = -value;
} else {
    ++non_negative;
}

switch (choice) {
case 0:
    return;
case 1:
    run_statistics();
    break;
default:
    report_error();
    break;
}
```

## Functions

```cpp
[[nodiscard]] double mean(std::span<const double> values);

double mean(std::span<const double> values) {
    const double total = sum(values);
    return total / static_cast<double>(values.size());
}

// A small function object passed to a standard algorithm.
const bool finite = std::all_of(values.begin(), values.end(), [](double value) {
    return std::isfinite(value);
});
```

## Vectors

```cpp
std::vector<int> scores;                  // empty
std::vector<int> primes{2, 3, 5, 7};      // size 4
scores.push_back(90);
scores[0] = 95;                           // no bounds check
scores.at(0) = 95;                        // throws std::out_of_range

std::vector<int> copy = primes;           // independent storage
std::span<const int> view{primes};        // no copy; invalidated if primes reallocates

int raw[]{90, 80, 70};                    // array length is fixed
std::span<int> array_view{raw};           // same algorithms can index an array
```

## Strings

```cpp
std::string text = "Hello";
text += ", world";
const char ch = text[0];
const std::size_t length = text.size();

std::string_view slice = text;            // does not own the characters
if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
    // classify the unsigned value; a plain char may be signed
}
```

## References

```cpp
int value = 7;
int& alias = value;          // another name for value
alias = 8;                   // value is now 8
const int& frozen = value;   // cannot assign through frozen

void scale(double& number) { number *= 2.0; }  // modifies the caller

std::size_t length_of(const std::string& text) {  // reads the caller's string
    return text.size();
}
```

## const

```cpp
const std::vector<double> values{1.0, 2.0};  // elements cannot be reassigned
void inspect(const std::vector<double>& values);
void edit(std::vector<std::int64_t>& values);  // sort functions take a mutable range

constexpr int kQuit = 0;     // compile-time constant
```

`const` on a parameter view promises that this function will not modify the object. It does not make a copy.

## Exceptions

```cpp
try {
    const double low = cs1511::minimum(values);
} catch (const std::overflow_error& error) {
    std::cout << error.what() << '\n';
} catch (const std::invalid_argument& error) {
    std::cout << error.what() << '\n';
}
```

Throw an exception for a broken contract, such as an empty collection passed to `minimum`. Return `std::optional` or an empty result when "not found" is a normal outcome, as `linear_search` does.

## Common standard algorithms

```cpp
const double low = *std::min_element(values.begin(), values.end());
const double high = *std::max_element(values.begin(), values.end());

// Seed with 0.0. An int seed of 0 truncates every fraction.
const double total = std::accumulate(values.begin(), values.end(), 0.0);

std::sort(copy.begin(), copy.end());  // requires operator<

const bool missing = std::any_of(values.begin(), values.end(), [](double value) {
    return !std::isfinite(value);
});
```

`std::min_element` and `std::max_element` return iterators. Dereference them only after proving the range is not empty.
