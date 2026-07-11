#include "catch.hpp"

#include <limits>

#include <sequence/timing.hpp>

TEST_CASE("samples_count", "[timing]")
{
    using namespace sequence;

    SECTION("uses time signature for 4/4 duration")
    {
        REQUIRE(samples_count(TimeSignature{4, 4}, 44'100, 120.f) == 88'200);
    }

    SECTION("uses time signature for non-4/4 duration")
    {
        REQUIRE(samples_count(TimeSignature{3, 8}, 48'000, 120.f) == 36'000);
    }

    SECTION("throws if time signature denominator is zero")
    {
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 0}, 44'100, 120.f),
                          std::invalid_argument);
    }

    SECTION("throws if sample rate is zero")
    {
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 4}, 0, 120.f),
                          std::invalid_argument);
    }

    SECTION("throws if bpm is not positive")
    {
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 4}, 44'100, 0.f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 4}, 44'100, -1.f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 4}, 44'100,
                                        std::numeric_limits<float>::quiet_NaN()),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 4}, 44'100,
                                        std::numeric_limits<float>::infinity()),
                          std::invalid_argument);
    }

    SECTION("throws if the sample count overflows")
    {
        REQUIRE_THROWS_AS(samples_count(TimeSignature{4, 1},
                                        std::numeric_limits<std::uint32_t>::max(), 1.f),
                          std::overflow_error);
    }
}
