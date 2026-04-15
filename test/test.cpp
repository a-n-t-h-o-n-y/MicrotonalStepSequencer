#include <filesystem>
#include <vector>

#include "catch.hpp"

#include <sequence/sequence.hpp>
#include <sequence/tuning.hpp>

using namespace sequence;

TEST_CASE("Cell and Sequence model", "[sequence]")
{
    SECTION("a cell may be silent")
    {
        auto const cell = Cell{};

        REQUIRE(cell.elements.empty());
        REQUIRE(cell.weight == 1.f);
    }

    SECTION("a cell may contain simultaneous elements")
    {
        auto const cell = Cell{
            .elements =
                {
                    Note{.pitch = 1, .velocity = 0.4f, .delay = 0.1f, .gate = 0.8f},
                    Note{.pitch = 8, .velocity = 0.7f, .delay = 0.f, .gate = 1.f},
                },
            .weight = 2.f,
        };

        REQUIRE(cell.elements.size() == 2);
        REQUIRE(std::holds_alternative<Note>(cell.elements[0]));
        REQUIRE(std::holds_alternative<Note>(cell.elements[1]));
        REQUIRE(std::get<Note>(cell.elements[0]).pitch == 1);
        REQUIRE(std::get<Note>(cell.elements[1]).pitch == 8);
        REQUIRE(cell.weight == 2.f);
    }

    SECTION("a sequence may contain nested sequence elements")
    {
        auto nested = Sequence{};
        nested.cells.push_back(Cell{.elements = {Note{.pitch = 3}}, .weight = 1.f});
        nested.cells.push_back(Cell{.elements = {}, .weight = 1.f});

        auto root = Sequence{};
        root.cells.push_back(Cell{.elements = {Note{.pitch = 0}}, .weight = 1.f});
        root.cells.push_back(Cell{.elements = {nested}, .weight = 3.f});

        REQUIRE(root.cells.size() == 2);
        REQUIRE(std::holds_alternative<Note>(root.cells[0].elements[0]));
        REQUIRE(std::holds_alternative<Sequence>(root.cells[1].elements[0]));

        auto const &child = std::get<Sequence>(root.cells[1].elements[0]);
        REQUIRE(child.cells.size() == 2);
        REQUIRE(std::get<Note>(child.cells[0].elements[0]).pitch == 3);
        REQUIRE(child.cells[1].elements.empty());
    }
}

TEST_CASE("Scala file import", "[sequence]")
{
    auto dir = std::filesystem::path{__FILE__};
    dir.remove_filename();
    auto const file = dir / "12-edo.scl";
    auto const tuning = from_scala(file);

    auto const expected = Tuning{
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
        1200,
        "",
    };

    REQUIRE(tuning == expected);
}

TEST_CASE("No scl archive files will throw errors", "[sequence]")
{
    auto dir = std::filesystem::path{__FILE__};
    dir.remove_filename();
    dir /= "scl-archive";

    for (auto const &entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.path().extension() == ".scl")
        {
            REQUIRE_NOTHROW(from_scala(entry.path()));
        }
    }
}

TEST_CASE("Sequence generation helpers via direct construction", "[sequence]")
{
    SECTION("constructing silent sequences")
    {
        auto const size = 4;
        auto const seq = Sequence{std::vector<Cell>(size, Cell{.elements = {}, .weight = 1.f})};

        REQUIRE(seq.cells.size() == size);
        for (auto const &cell : seq.cells)
        {
            REQUIRE(cell.elements.empty());
        }
    }

    SECTION("constructing note sequences")
    {
        auto const note = Note{5, 0.4f, 0.01f, 0.8f};
        auto const seq =
            Sequence{std::vector<Cell>(3, Cell{.elements = {note}, .weight = 1.f})};

        REQUIRE(seq.cells.size() == 3);
        for (auto const &cell : seq.cells)
        {
            REQUIRE(cell.elements.size() == 1);
            REQUIRE(std::get<Note>(cell.elements[0]) == note);
        }
    }
}
