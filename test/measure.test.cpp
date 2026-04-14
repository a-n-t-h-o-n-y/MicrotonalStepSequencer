#include "catch.hpp"

#include <variant>

#include "helper.hpp"
#include <sequence/timing.hpp>

using namespace sequence::test::helper;

TEST_CASE("samples_count", "[timing]")
{
    using namespace sequence;

    auto const cell = Cell{Sequence{std::vector<Cell>{
                               Cell{Rest{}, 1.f},
                               Cell{Rest{}, 1.f},
                               Cell{Rest{}, 1.f},
                               Cell{Rest{}, 1.f},
                           }},
                           1.f};

    SECTION("uses time signature for 4/4 duration")
    {
        REQUIRE(samples_count(cell, TimeSignature{4, 4}, 44'100, 120.f) == 88'200);
    }

    SECTION("uses time signature for non-4/4 duration")
    {
        REQUIRE(samples_count(cell, TimeSignature{3, 8}, 48'000, 120.f) == 36'000);
    }

    SECTION("throws if time signature denominator is zero")
    {
        REQUIRE_THROWS_AS(samples_count(cell, TimeSignature{4, 0}, 44'100, 120.f),
                          std::invalid_argument);
    }

    SECTION("throws if sample rate is zero")
    {
        REQUIRE_THROWS_AS(samples_count(cell, TimeSignature{4, 4}, 0, 120.f),
                          std::invalid_argument);
    }

    SECTION("throws if bpm is not positive")
    {
        REQUIRE_THROWS_AS(samples_count(cell, TimeSignature{4, 4}, 44'100, 0.f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(samples_count(cell, TimeSignature{4, 4}, 44'100, -1.f),
                          std::invalid_argument);
    }
}
