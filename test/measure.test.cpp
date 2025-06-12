#include "catch.hpp"

#include <variant>

#include "helper.hpp"
#include <sequence/measure.hpp>

using namespace sequence::test::helper;

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
        REQUIRE(measure.time_signature ==
                TimeSignature{.numerator = 4, .denominator = 4});
    }

    SECTION("returns a measure with the correct number of cells")
    {
        auto const measure = create_measure({4, 4}, 1);
        REQUIRE(holds<Sequence>(measure.cell));
        REQUIRE(get<Sequence>(measure.cell).cells.size() == 4);
    }

    SECTION(
        "returns a measure with the correct number of cells when cell_resolution > 1")
    {
        auto const measure = create_measure({4, 4}, 2);
        REQUIRE(holds<Sequence>(measure.cell));
        REQUIRE(get<Sequence>(measure.cell).cells.size() == 8);
    }

    SECTION("returns a measure with sequence of Rest cells")
    {
        auto const measure = create_measure({4, 4}, 1);
        REQUIRE(holds<Sequence>(measure.cell));
        auto const &cells = get<Sequence>(measure.cell).cells;

        REQUIRE(holds<Rest>(cells[0]));
        REQUIRE(holds<Rest>(cells[1]));
        REQUIRE(holds<Rest>(cells[2]));
        REQUIRE(holds<Rest>(cells[3]));
    }
}