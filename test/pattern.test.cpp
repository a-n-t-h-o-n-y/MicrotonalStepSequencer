#include "catch.hpp"

#include <vector>

#include <sequence/pattern.hpp>

TEST_CASE("pattern_contains", "[pattern]")
{
    using namespace sequence;

    auto const pattern = Pattern{1, {2, 3}};

    REQUIRE_FALSE(pattern_contains(pattern, 0));
    REQUIRE(pattern_contains(pattern, 1));
    REQUIRE_FALSE(pattern_contains(pattern, 2));
    REQUIRE(pattern_contains(pattern, 3));
    REQUIRE_FALSE(pattern_contains(pattern, 4));
    REQUIRE_FALSE(pattern_contains(pattern, 5));
    REQUIRE(pattern_contains(pattern, 6));
    REQUIRE(pattern_contains(pattern, 8));
}

TEST_CASE("contains_valid_pattern", "[pattern]")
{
    using namespace sequence;

    REQUIRE(contains_valid_pattern(""));
    REQUIRE(contains_valid_pattern("1 2 3 rotate"));
    REQUIRE(contains_valid_pattern("+2 3 4 reverse"));

    REQUIRE_FALSE(contains_valid_pattern("0 2"));
    REQUIRE_FALSE(contains_valid_pattern("+x 1 2"));
    REQUIRE_FALSE(contains_valid_pattern("+"));
}

TEST_CASE("parse_pattern", "[pattern]")
{
    using namespace sequence;

    REQUIRE(parse_pattern("") == Pattern{0, {1}});
    REQUIRE(parse_pattern("2 3 mirror") == Pattern{0, {2, 3}});
    REQUIRE(parse_pattern("+4 2 1 shift") == Pattern{4, {2, 1}});
    REQUIRE(parse_pattern("+4 rotate") == Pattern{4, {1}});

    REQUIRE_THROWS_AS(parse_pattern("0 2"), std::invalid_argument);
}

TEST_CASE("pop_pattern_chars", "[pattern]")
{
    using namespace sequence;

    REQUIRE(pop_pattern_chars("+4 2 1 shift") == " shift");
    REQUIRE(pop_pattern_chars("2 3 mirror") == " mirror");
    REQUIRE(pop_pattern_chars("reverse") == "reverse");
}

TEST_CASE("PatternView", "[pattern]")
{
    using namespace sequence;

    auto values = std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7};
    auto visited = std::vector<int>{};
    auto const pattern = Pattern{1, {2, 3}};
    auto const empty_pattern = Pattern{0, {}};

    for (auto &value : PatternView<int>{values, pattern})
    {
        visited.push_back(value);
        value *= 10;
    }

    REQUIRE(visited == std::vector<int>{1, 3, 6});
    REQUIRE(values == std::vector<int>{0, 10, 2, 30, 4, 5, 60, 7});
    REQUIRE_THROWS_AS((PatternView<int>{values, empty_pattern}), std::invalid_argument);
}

TEST_CASE("ConstPatternView", "[pattern]")
{
    using namespace sequence;

    auto const values = std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7};
    auto visited = std::vector<int>{};
    auto const pattern = Pattern{1, {2, 3}};
    auto const empty_pattern = Pattern{0, {}};

    for (auto const &value : ConstPatternView<int>{values, pattern})
    {
        visited.push_back(value);
    }

    REQUIRE(visited == std::vector<int>{1, 3, 6});
    REQUIRE_THROWS_AS((ConstPatternView<int>{values, empty_pattern}),
                      std::invalid_argument);
}
