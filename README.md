# CS 1511 — Day 1: Programming Foundations

Day 1 of a 14-day computer science refresher aligned with an introductory CS 1511 course. The project reviews variables, control flow, functions, collections, and basic algorithms in modern C++20, then checks that work with automated tests.

The library computes. The demonstration program only reads lines and prints results.

## Purpose and learning objectives

After working through this repository you should be able to:

- Choose a type that matches the data: a fixed-width integer, a floating-point value, a size, or a Boolean.
- Write conditionals and loops whose termination condition you can explain.
- Split a problem into functions with an explicit contract for empty input and bad input.
- Store a sequence in an array or a `std::vector`, and pass it to a function without an unnecessary copy.
- Use `const` to say what a function is allowed to change.
- Implement and compare linear search, binary search, selection sort, insertion sort, the Euclidean algorithm, and a primality test.
- Describe the running time of each implementation with Big-O notation.
- Build the project with CMake, run the tests with CTest, and read a failing assertion as evidence about a contract.

## Prerequisites

- A C++20 compiler. GCC 11 or newer, or Clang 14 or newer, is sufficient. The standard library must provide `std::span` and `std::from_chars` for floating-point values.
- CMake 3.20 or newer.
- Git. The test build downloads [Catch2](https://github.com/catchorg/Catch2) 3.7.1 with CMake `FetchContent`. A system-wide Catch2 install is not required and is not used.
- A terminal that can run `cmake`, `ctest`, and the demonstration program.

The first configuration step needs network access so CMake can clone Catch2. Later builds reuse the copy stored under the build directory.

## Repository structure

```text
.
├── .github/workflows/ci.yml    # configure, build, and test on GitHub Actions
├── docs/
│   ├── cheat-sheet.md          # short syntax examples
│   └── learning-log.md         # prompts to fill in after a study session
├── include/cs1511/             # declarations, and template algorithm bodies
│   ├── algorithms.hpp
│   ├── input.hpp
│   ├── statistics.hpp
│   └── text_analysis.hpp
├── src/
│   ├── algorithms.cpp          # gcd and is_prime
│   ├── input.cpp               # parsing and validation
│   ├── main.cpp                # console demonstration only
│   ├── statistics.cpp
│   └── text_analysis.cpp
├── tests/                      # Catch2 tests, registered with CTest
├── CMakeLists.txt
├── LICENSE
└── README.md
```

`include/` is the contract a caller reads. `src/` is the implementation. Template functions are an exception: the compiler must see their bodies in order to instantiate them, so search and sort are implemented in `algorithms.hpp`. `gcd` and `is_prime` are ordinary functions, so only their declarations live in the header.

`src/main.cpp` is the only file that includes `<iostream>`.

## Build, test, and run

Configure an out-of-source build, then compile and test:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the demonstration program:

```bash
./build/cs1511_demo
```

Run one test by a substring of its name:

```bash
ctest --test-dir build -R "binary search" --output-on-failure
```

CMake asks Catch2 to register each `TEST_CASE` with CTest, so `ctest` runs the cases as separate tests. A release build uses the same commands with `-DCMAKE_BUILD_TYPE=Release`.

Do not commit the `build/` directory. It contains generated object files, the cloned Catch2 sources, and CTest's discovered test list. Those files are named in `.gitignore`.

GitHub Actions runs the same three commands on `ubuntu-latest`: configure, build, and `ctest`. The workflow file is `.github/workflows/ci.yml`.

The library targets compile with strict warnings (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow` and related checks, or `/W4` on MSVC) and treat warnings as errors. Those flags are applied to this project's targets, not to the fetched Catch2 sources.

## Demonstration program

The menu is:

| Choice | What it calls |
| --- | --- |
| 1 | Real or 64-bit integer statistics |
| 2 | Normalize, word count, vowel count, and word frequency |
| 3 | Linear search, iterative binary search, or recursive binary search |
| 4 | Selection sort or insertion sort |
| 5 | Euclidean GCD or a primality test |
| 0 | Quit |

Indexes printed by search are zero-based. Numeric collections are space-separated tokens on one line. A blank line is an empty collection. It is valid for search and sort, and invalid for statistics. Text analysis takes the entire line, including spaces.

Invalid input prints `Error:` and returns to the menu. End of input quits.

## Variables and types

A variable is a named object with a type and a lifetime. The type decides which operations are legal and how much storage the object uses.

| Type in this project | What it represents |
| --- | --- |
| `int` | A small menu choice |
| `std::int64_t` | An exact integer, including values near the 64-bit limits |
| `double` | A real statistic such as a mean or a median |
| `long double` | A wider accumulator for the mean of large integers |
| `bool` | The result of `is_prime` |
| `std::size_t` | A count or a zero-based index |
| `char` / `unsigned char` | A text byte; classification uses the unsigned value |
| `std::string` | Owned text |
| `std::string_view` | A non-owning view of text that must outlive the view |
| `std::vector<T>` | An owned, resizable sequence |
| `std::span<T>` | A non-owning view of a contiguous sequence |
| `std::map<std::string, std::size_t>` | Word frequencies in alphabetical order |
| `std::optional<std::size_t>` | An index, or "not found" |

Initialize a variable at the declaration. `double mean = 0.0` is a floating-point zero. `int total = 0` is an integer zero. Those are different types, and mixing them carelessly truncates fractions.

`auto` deduces the type from the initializer. It does not remove the need to know the type. `const auto summary = cs1511::summarize(values);` is a `RealSummary` or an `IntegerSummary` depending on which overload is selected.

`constexpr int kQuit = 0;` is a compile-time constant used as a menu label. The value cannot change, and the compiler can see it while compiling.

## Conditionals

A conditional selects a path. The statistics validator uses one to reject an empty collection before any algorithm runs:

```cpp
if (values.empty()) {
    throw std::invalid_argument("statistics require a non-empty collection");
}
```

`is_prime` uses a chain of conditions to return early for the cases that do not need a loop: values below 2, the primes 2 and 3, and numbers divisible by 2 or 3. The demo uses `switch` on the menu choice. Every case that continues the program ends in `break`, so control does not fall into the next case.

Comparison is not assignment. `choice == 0` tests equality. `choice = 0` stores zero and is not a test.

## Loops

A `for` loop is a good fit when the number of steps follows the size of a collection. Selection sort uses an outer index `i` and an inner index `j`. The inner loop starts at `i + 1` so each pass looks only at the unsorted suffix. The condition `i + 1 < count` also avoids forming `count - 1` when `count` is zero.

A `while` loop is a good fit when the stop condition is discovered during the loop. Iterative binary search continues while the unexplored interval is non-empty (`low < high`). The Euclidean algorithm continues while the remainder is not zero.

Range-based `for` visits every element without an index. Vowel counting uses it because the position of each character does not matter. Use an index when the algorithm needs the position, as search does.

Every loop in this project moves an index or a remainder toward a bound. That is the property to check when a loop looks as if it might run forever. In particular, binary search computes the midpoint as `low + (high - low) / 2`. The form `(low + high) / 2` can overflow when `low` and `high` are large, and an overflowed midpoint can keep the same interval forever.

## Functions

A function names a piece of work, lists the data it needs, and states what it returns. The declaration in the header is the contract. Callers should not need to read the body to know what happens on an empty collection.

```cpp
[[nodiscard]] double median(std::span<const double> values);
```

`[[nodiscard]]` tells the compiler to warn if a caller ignores the returned value. For a pure computation, discarding the result is almost always a bug.

Some functions return a value. `linear_search` returns `std::optional<std::size_t>`: either an index or "not found." Not found is a normal result, so it is not an exception. Some functions change their argument. `selection_sort` returns `void` and rearranges the caller's collection. Some functions report a broken contract by throwing. `minimum` throws `std::invalid_argument` for an empty collection. `sum` of 64-bit integers throws `std::overflow_error` instead of wrapping past the limit.

Exceptions are reserved for contracts the caller violated or for results that cannot be represented. They are not used to signal an ordinary search miss.

## Arrays and vectors

An array has a fixed length chosen when it is created:

```cpp
const std::int64_t values[]{10, 20, 30};
const auto found = cs1511::linear_search(values, std::int64_t{20});
```

A `std::vector` owns a resizable buffer. `push_back` may reallocate that buffer and move the elements to a new address. An index into a vector remains valid across reallocation. A pointer, reference, or `std::span` into the vector does not.

The library accepts any contiguous collection, so the same search and sort functions work for a C array, a `std::vector`, and a `std::span`. The tests cover both an array and a vector. Contiguous means the elements sit in one block of memory, which is what index arithmetic requires. A `std::list` is not contiguous and will not compile with these functions.

`operator[]` does not check bounds. `at` does, and throws `std::out_of_range`. The algorithms in this project stay inside `0 .. size-1` by construction instead of calling `at` on every access.

## References, views, and const correctness

A reference is another name for an existing object. It is not a copy.

```cpp
int value = 7;
int& alias = value;
alias = 8;                 // value is now 8
const int& frozen = value; // reading is allowed; assignment through frozen is not
```

`const` on an object means this name will not be used to modify it. `const` on a member function, such as `InputError::failure() const`, means the function does not modify the error object.

`std::span<const double>` is a view: a pointer and a length. It refers to elements owned by someone else, and the `const` on the element type means the function may read those elements and may not assign to them. `minimum` takes that view. `selection_sort` takes a mutable contiguous range because sorting writes new values into the same storage.

A view must not outlive the data it points at. Creating a `std::span` from a temporary `std::vector` and storing that span is a dangling view. Passing the temporary into a function is safe, because the temporary lives until the function returns. The statistics functions use their span only during the call.

`std::string_view` is the same idea for characters. `normalize` borrows the caller's text and returns a new `std::string` that owns the normalized result.

## Pass-by-value versus pass-by-reference

Pass-by-value copies the argument into the parameter. The function can change the parameter without affecting the caller. That is the right default for small, cheap objects such as `std::int64_t` and `int`:

```cpp
std::int64_t gcd(std::int64_t a, std::int64_t b);
```

`gcd` rewrites its local copies as it computes remainders. The caller's variables stay unchanged.

Pass-by-reference (`T&`) binds the parameter to the caller's object. There is no copy. Changes are visible to the caller. Use it when the function's job is to modify that object, or when copying would be expensive and mutation is intended. `selection_sort` and `insertion_sort` need this kind of access. Their range parameter is a non-const lvalue reference, so they will not silently sort a temporary and throw the result away.

Pass-by-const-reference (`const T&`) also avoids the copy, and it forbids modification through that parameter. Use it for large read-only objects. A `const std::string&` parameter can read a string of any length without copying the characters.

A span sits between these choices. Passing `std::span<const double>` copies the view (a pointer and a size, two words) and does not copy the numbers. Passing `std::vector<double>` by value would copy every number. The statistics functions therefore take a span, and a `std::vector<double>` converts to that span automatically because those functions are not templates. The search and sort functions are templates, so they accept the collection directly and build the span inside the function. Template argument deduction does not consider that implicit conversion, which is why those signatures are written on the range itself.

Return by value and let the compiler construct the result in place. `parse_integers` returns a `std::vector` by value. `word_frequency` returns a `std::map` by value. The caller owns the result.

## Basic Big-O

Big-O describes how the number of basic steps grows as the input grows. It is an upper bound on the growth rate, not a measurement in seconds. Constants and lower-order terms are dropped: a loop that does `3n + 10` element visits is O(n).

The letter inside the parentheses names the thing that grows. For a collection, `n` is the number of elements. For text, `n` is the number of characters. For primality, `n` is the integer being tested, so O(√n) means the work grows with the square root of that integer, not with the number of digits alone. For GCD, the input size is the magnitude of the smaller argument.

Common shapes in this project:

| Notation | Informal meaning | Example |
| --- | --- | --- |
| O(1) | The work does not grow with n | Recognizing that 4 is even |
| O(log n) | The work grows by one step when n doubles | One binary-search probe |
| O(n) | The work doubles when n doubles | A linear scan |
| O(n log n) | A log factor on top of a linear pass | Sorting a copy to find a median |
| O(n²) | Nested passes over the collection | Selection sort comparisons |
| O(√n) | A loop whose bound is the square root | Trial division |

Best, average, and worst case describe different inputs of the same size. Insertion sort is O(n) on an already sorted collection and O(n²) on a reverse-sorted one. Selection sort does O(n²) comparisons in every case, because it always scans the unsorted suffix.

Extra space is counted separately from the input. An in-place sort uses O(1) additional memory. A recursive binary search uses O(log n) stack frames. A median that copies its input uses O(n) additional memory.

These bounds count typical arithmetic, comparisons, and memory operations. They do not include the cost of a process switch or a cache miss, and they assume that comparing two values is O(1), which is true here for numbers and is a simplification for long strings.

## Complexity of the implemented algorithms

The search bounds below are for the search itself. Both binary-search functions first scan the collection to reject a decreasing pair. That validation is O(n) time and O(1) extra space. It is deliberate: an unsorted collection is invalid input, not a failed search.

| Algorithm | Best case | Average case | Worst case | Extra space | Notes |
| --- | --- | --- | --- | --- | --- |
| `minimum`, `maximum` | O(n) | O(n) | O(n) | O(1) | Every element can be the extreme |
| `sum`, `mean` | O(n) | O(n) | O(n) | O(1) | Real mean uses the real sum |
| `median` | O(n log n) | O(n log n) | O(n log n) | O(n) | Copies, then sorts; the caller's order is preserved |
| `normalize` | O(n) | O(n) | O(n) | O(n) | Builds a new string |
| `count_words` | O(n) | O(n) | O(n) | O(n) | Allocates the token list so the word rule has one definition |
| `count_vowels` | O(n) | O(n) | O(n) | O(1) | One pass, ASCII vowels only |
| `word_frequency` | O(n log u) | O(n log u) | O(n log u) | O(n) | `u` is the number of distinct words; `std::map` keeps keys sorted |
| Linear search | O(1) | O(n) | O(n) | O(1) | Best case: the target is the first element |
| Iterative binary search | O(1) | O(log n) | O(log n) | O(1) | Leftmost match; requires non-decreasing data |
| Recursive binary search | O(1) | O(log n) | O(log n) | O(log n) | Same result as the iterative search; depth is the stack cost |
| Selection sort | O(n²) | O(n²) | O(n²) | O(1) | Not stable |
| Insertion sort | O(n) | O(n²) | O(n²) | O(1) | Stable; the O(n) case is already sorted data |
| Euclidean `gcd` | O(1) | O(log min(\|a\|, \|b\|)) | O(log min(\|a\|, \|b\|)) | O(1) | Best case includes an argument of zero |
| `is_prime` | O(1) | O(√n) | O(√n) | O(1) | Best case includes evens and values below 2 |

A binary search that finds any match and then walks left to the first duplicate becomes O(n) when every element equals the target. Both implementations in this project keep searching the left half instead, so the leftmost index stays O(log n).

## Contracts and invalid input

Empty input is not one rule. Each operation says what empty means.

| Operation | Empty input | Other rejected input |
| --- | --- | --- |
| Statistics | Throws `std::invalid_argument` | Non-finite reals; a real sum that overflows to infinity; an integer sum that would overflow `std::int64_t` |
| Text analysis | Empty string, zero counts, empty map | None. Punctuation is a separator, not an error |
| Linear search | Not found | None. Unsorted data is allowed |
| Binary search | Not found, because an empty range is non-decreasing | A decreasing adjacent pair throws `std::invalid_argument` |
| Either sort | Already sorted; the result is empty | None |
| `gcd` | `gcd(0, n)` is `\|n\|` when `n` is not zero | `gcd(0, 0)` is undefined; `INT64_MIN` has no representable absolute value |
| `is_prime` | Not used | Values below 2, including negatives, are not prime |
| Menu parsing | Malformed numeric input | A choice outside the legal range; a token that is not an integer; an integer that does not fit in 64 bits |
| Numeric parsing | Empty collection, which the caller then accepts or rejects | Trailing junk, `nan`, `inf`, and out-of-range magnitudes |

A word is a maximal run of ASCII letters and digits. Hyphens and apostrophes separate words, so `"don't"` becomes `"don t"`. Frequency keys are lowercase. `y` is not counted as a vowel.

Integer `sum` uses a checked addition and throws `std::overflow_error` on overflow. Signed overflow is undefined behavior in C++, so the check happens before the addition. `INT64_MIN` is a special case because its negation does not fit in `std::int64_t`. Integer `mean` accumulates in `long double` and does not need the exact sum to fit in 64 bits; the result may still be inexact once it is returned as `double`, which has a 53-bit integer significand. `summarize` for integers includes the exact sum, so it throws when that sum overflows.

The even-sized median is the average of the two central values. Those values are halved before they are added, so a pair of large finite numbers does not overflow the addition.

## Common beginner mistakes

- Seeding `std::accumulate` with the integer `0`. The accumulation type follows the seed, so fractional parts disappear. The real `sum` seeds with `0.0`.
- Using integer division for a mean. In integer arithmetic, `5 / 2` is `2`. The mean functions divide by a floating-point count.
- Calling binary search on unsorted, or only reverse-sorted, data. The algorithm's index arithmetic is valid only for non-decreasing order. This project rejects that input. Linear search does not.
- Writing the midpoint as `(low + high) / 2` and overflowing the sum, or setting `low = mid` when `mid` did not move and looping forever.
- Returning an index into a sliced sub-range and forgetting to add the slice's offset. Both binary searches in this project pass absolute indexes.
- Treating a failed search as an exception, or treating a broken contract as `std::nullopt`. This project uses `std::nullopt` only for "not found."
- Letting a signed integer wrap. `int64_t` addition past the maximum is not defined to wrap. `std::abs(INT64_MIN)` is the same class of bug.
- Passing a possibly negative `char` to `std::isalnum` or `std::tolower`. Those functions require an `unsigned char` value or `EOF`. The text and input code casts first.
- Taking the minimum of an empty collection. There is no element to return. Dereferencing the end iterator is undefined.
- Defining `gcd(0, 0)`. Every integer divides 0, so there is no greatest one.
- Sorting through a `const` reference or a by-value parameter, then expecting the caller to see the new order. Sort must write through a mutable view of the caller's storage.
- Assuming selection sort keeps equal elements in their original order. It does not. Insertion sort does, because an equal key is not moved in front of the element already in the sorted prefix.
- Comparing a one-based mental index with a zero-based result. The value `5` in `{1, 3, 5, 7, 9}` is at index 2.
- Testing `i * i <= n` in a primality loop. When `n` is near the top of the 64-bit range, `i * i` overflows. `is_prime` uses `i <= n / i` instead.
- Copying a vector out of habit. Pass a span or a const reference when the function only reads.
- Putting `using namespace std;` in a header. It changes name lookup for every file that includes the header. This project uses explicit `std::` names.

## Example program output

The program prints prompts and results. It does not echo what you type. This input:

```text
1
r
10 20 30
2
Hello, hello World!
3
1 3 5 7 9
5
1
4
9 4 1 4
2
5
1
48 18
5
2
17
0
```

produces this output:

```text
UMD CS 1511 — Day 1: Programming Foundations
Statistics, text analysis, search, sort, GCD, and primality.
Library code computes. This program only reads lines and prints results.
Indexes are zero-based. Enter collections as space-separated tokens.

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- Statistics ---
Enter r for real numbers or i for 64-bit integers:
Enter space-separated values:
Value type: real
Values: 10 20 30
Minimum: 10
Maximum: 30
Sum: 60
Mean: 20
Median: 20

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- Text analysis ---
Enter a line of text:
Input: Hello, hello World!
Normalized: hello hello world
Word count: 3
Vowel count: 5
Frequencies:
  hello 2
  world 1

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- Search ---
Enter space-separated integers:
Enter the target integer:
Enter 1 for linear search, 2 for iterative binary search, or 3 for recursive binary search:
Collection: 1 3 5 7 9
Target: 5
Algorithm: linear search
Found at index 2.

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- Sort ---
Enter space-separated integers:
Enter 1 for selection sort or 2 for insertion sort:
Input: 9 4 1 4
Algorithm: insertion sort
Sorted: 1 4 4 9

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- GCD and primality ---
Enter 1 for the Euclidean GCD or 2 for a primality test:
Enter two space-separated integers:
GCD(48, 18) = 6

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:

--- GCD and primality ---
Enter 1 for the Euclidean GCD or 2 for a primality test:
Enter one integer:
17 is prime.

Menu
  1  Statistics
  2  Text analysis
  3  Search
  4  Sort
  5  GCD and primality
  0  Quit
Enter a menu choice:
Goodbye.
```

A second session exercises malformed input, an invalid menu choice, an empty collection, integer overflow, unsorted binary search, an empty sort, `gcd(0, 0)`, and a negative primality test. The input is:

```text
abc
9
1
i

1
i
9223372036854775807 1
3
9 1
1
2
4

1
5
1
0 0
5
2
-7
0
```

The blank lines are empty collections. Between errors the program reprints the menu. The distinctive results are:

```text
Error: malformed numeric input: 'abc'
Error: invalid menu choice: enter an integer from 0 to 5
Value type: integer
Values: (empty)
Error: statistics require a non-empty collection
Value type: integer
Values: 9223372036854775807 1
Error: integer sum overflow
Collection: 9 1
Target: 1
Error: binary search requires a non-decreasing collection
Input: (empty)
Empty collection: there are no values to sort.
Algorithm: selection sort
Sorted: (empty)
Error: gcd(0, 0) is undefined
-7 is not prime.
Goodbye.
```

## What I Practiced

- I declared variables with the narrowest type that still represented the data, including `std::int64_t` for exact integers and `double` for means and medians.
- I wrote `if` and `switch` paths for menu choices, empty collections, and the early exits in the primality test.
- I wrote counted `for` loops for sorting and searching, and `while` loops for binary search and the Euclidean algorithm.
- I separated contracts in headers from implementations in `.cpp` files, and I kept `main` limited to input and output.
- I stored sequences in arrays and vectors, passed them through spans, and used `const` so a read-only function cannot modify the caller.
- I compared pass-by-value for integers with pass-by-reference and views for collections.
- I implemented linear search, both binary searches, both sorts, GCD, and primality, including the empty, one-element, duplicate, sorted, and reverse-sorted cases.
- I described each algorithm with Big-O notation and checked the boundary behavior with Catch2.
- I built and tested the project with CMake and CTest, and I set the same commands up for GitHub Actions.

## Further exploration

- Replace the sorting median with `std::nth_element`. Average linear time is possible, and the even-sized case still needs the two central values.
- Count words in one pass with O(1) extra memory, sharing the same definition of a word that `normalize` uses.
- Time insertion sort and selection sort on sorted, reverse-sorted, and random collections with `std::chrono`. The complexity table predicts the shape of the curves.
- Compare these binary searches with `std::ranges::lower_bound` on the same inputs, including long runs of duplicates.
- Implement merge sort and decide, with a test like the `Item` stability test, whether the result must keep equal keys in their original order.
- Add a sieve of Eratosthenes and compare it with trial division when many primality questions share an upper bound.
- Try `std::unordered_map` for frequencies and write down what alphabetical output used to guarantee.
- Add a property test: `gcd(a, b)` divides both arguments, both sorts are permutations of the input, and the median is between the minimum and the maximum.
- Read integers from a file so the collection is not limited to one typed line.
- Sum floating-point values with Kahan summation and find an input where the left-to-right sum in this project overflows even though a reassociation would not.

## License

This project is released under the MIT License. See [LICENSE](LICENSE).
