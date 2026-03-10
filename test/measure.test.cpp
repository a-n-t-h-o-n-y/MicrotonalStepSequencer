#include "catch.hpp"

#include <variant>

#include "helper.hpp"
#include <sequence/timing.hpp>

using namespace sequence::test::helper;

TEST_CASE("create_cell", "[timing]")
{
    using namespace sequence;

    SECTION("throws if cell_resolution < 1")
    {
        REQUIRE_THROWS_AS(create_cell({4, 4}, 0), std::invalid_argument);
    }

    SECTION("returns a cell containing the correct number of cells")
    {
        auto const cell = create_cell({4, 4}, 1);
        REQUIRE(holds<Sequence>(cell));
        REQUIRE(get<Sequence>(cell).cells.size() == 4);
    }

    SECTION("returns a cell with the correct number of cells when cell_resolution > 1")
    {
        auto const cell = create_cell({4, 4}, 2);
        REQUIRE(holds<Sequence>(cell));
        REQUIRE(get<Sequence>(cell).cells.size() == 8);
    }

    SECTION("returns a cell with sequence of Rest cells")
    {
        auto const cell = create_cell({4, 4}, 1);
        REQUIRE(holds<Sequence>(cell));
        auto const &cells = get<Sequence>(cell).cells;

        REQUIRE(holds<Rest>(cells[0]));
        REQUIRE(holds<Rest>(cells[1]));
        REQUIRE(holds<Rest>(cells[2]));
        REQUIRE(holds<Rest>(cells[3]));
    }
}

TEST_CASE("samples_count", "[timing]")
{
    using namespace sequence;

    auto const cell = create_cell({4, 4}, 1);

    SECTION("uses time signature for 4/4 duration")
    {
        REQUIRE(samples_count(cell, TimeSignature{4, 4}, 44'100, 120.f) == 88'200);
    }

    SECTION("uses time signature for non-4/4 duration")
    {
        REQUIRE(samples_count(cell, TimeSignature{3, 8}, 48'000, 120.f) == 36'000);
    }
}
