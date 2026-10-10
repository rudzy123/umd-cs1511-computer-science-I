cmake_minimum_required(VERSION 3.20)

project(umd_cs1511_computer_science_I
    VERSION 1.0.0
    DESCRIPTION "Day 1 CS 1511 programming foundations refresher"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CS1511_WARNING_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CS1511_WARNING_FLAGS
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Werror
    )
elseif(MSVC)
    set(CS1511_WARNING_FLAGS /W4 /permissive- /WX)
endif()

# Warnings apply to this project's targets only. Catch2 is fetched as source and
# is not rebuilt under these -Werror flags.
function(cs1511_apply_warnings target)
    target_compile_options(${target} PRIVATE ${CS1511_WARNING_FLAGS})
endfunction()

add_library(cs1511_foundations STATIC
    src/algorithms.cpp
    src/input.cpp
    src/statistics.cpp
    src/text_analysis.cpp
)
target_include_directories(cs1511_foundations PUBLIC include)
target_compile_features(cs1511_foundations PUBLIC cxx_std_20)
cs1511_apply_warnings(cs1511_foundations)

add_executable(cs1511_demo src/main.cpp)
target_link_libraries(cs1511_demo PRIVATE cs1511_foundations)
cs1511_apply_warnings(cs1511_demo)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(cs1511_tests
        tests/test_algorithms.cpp
        tests/test_input.cpp
        tests/test_statistics.cpp
        tests/test_text_analysis.cpp
    )
    target_link_libraries(cs1511_tests PRIVATE cs1511_foundations Catch2::Catch2WithMain)
    cs1511_apply_warnings(cs1511_tests)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(Catch)
    catch_discover_tests(cs1511_tests)
endif()
