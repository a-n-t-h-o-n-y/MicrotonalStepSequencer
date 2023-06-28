#include "catch.hpp"

#include <sequence/measure.hpp>

TEST_CASE("create_measure", "[measure]")
{
    using namespace sequence;

    SECTION("throws if cell_resolution < 1")
    {
        REQUIRE_THROWS_AS(create_measure({4, 4}, 0), std::invalid_argument);
    }

    SECTION("returns a measure with the correct time signature")
    {
        auto const measure = create_measure({4, 4}, 1);
        REQUIRE(measure.time_signature.numerator == 4);
        REQUIRE(measure.time_signature.denominator == 4);
    }

    SECTION("returns a measure with the correct number of cells")
    {
        auto const measure = create_measure({4, 4}, 1);
        REQUIRE(measure.sequence.cells.size() == 4);
    }

    SECTION(
        "returns a measure with the correct number of cells when cell_resolution > 1")
    {
        auto const measure = create_measure({4, 4}, 2);
        REQUIRE(measure.sequence.cells.size() == 8);
    }

    SECTION("returns a measure with sequence of Rest cells")
    {
        auto const measure = create_measure({4, 4}, 1);
        REQUIRE(std::holds_alternative<Rest>(measure.sequence.cells[0]));
        REQUIRE(std::holds_alternative<Rest>(measure.sequence.cells[1]));
        REQUIRE(std::holds_alternative<Rest>(measure.sequence.cells[2]));
        REQUIRE(std::holds_alternative<Rest>(measure.sequence.cells[3]));
    }
}