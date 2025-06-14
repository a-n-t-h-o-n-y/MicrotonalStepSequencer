#include <filesystem>
#include <variant>

#include "catch.hpp"

#include "helper.hpp"
#include <sequence/generate.hpp>
#include <sequence/sequence.hpp>
#include <sequence/tuning.hpp>

using namespace sequence;
using namespace sequence::test::helper;

TEST_CASE("Sequence", "[sequence]")
{
    auto s = Sequence{};
    s.cells.push_back({Note{0, 0.5, 0.5, 0.5}});
    s.cells.push_back({Rest{}});
    s.cells.push_back({Rest{}});
    s.cells.push_back({Sequence{}});
    get<Sequence>(s.cells[3]).cells.push_back({Note{0, 0.5, 0.5, 0.5}});
    get<Sequence>(s.cells[3]).cells.push_back({Rest{}});
    get<Sequence>(s.cells[3]).cells.push_back({Rest{}});
    REQUIRE(holds<Note>(s.cells[0]));
    REQUIRE(holds<Rest>(s.cells[1]));
    REQUIRE(holds<Rest>(s.cells[2]));
    REQUIRE(holds<Sequence>(s.cells[3]));
    REQUIRE(holds<Note>(get<Sequence>(s.cells[3]).cells[0]));
    REQUIRE(holds<Rest>(get<Sequence>(s.cells[3]).cells[1]));
    REQUIRE(holds<Rest>(get<Sequence>(s.cells[3]).cells[2]));
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

TEST_CASE("Generate Empty Sequences", "[sequence]")
{
    SECTION("Zero Cells")
    {
        auto const size = 0;
        auto const s = generate::empty(size);
        REQUIRE(s.cells.size() == size);
    }

    SECTION("4 Cells")
    {
        auto const size = 4;
        auto const s = generate::empty(size);
        REQUIRE(s.cells.size() == size);

        for (auto const &cell : s.cells)
        {
            REQUIRE(holds<Rest>(cell));
        }
    }
}

TEST_CASE("Generate Full Sequences", "[sequence]")
{
    SECTION("Zero Cells")
    {
        auto const size = 0;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::full(size, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("100 Cells")
    {
        auto const size = 100;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::full(size, note);

        REQUIRE(s.cells.size() == size);

        for (auto const &cell : s.cells)
        {
            REQUIRE((holds<Note>(cell) && get<Note>(cell) == note));
        }
    }
}

TEST_CASE("Generate Interval Sequences", "[sequence]")
{
    SECTION("Zero Cells")
    {
        auto const size = 0;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::interval(size, 2, 1, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("100 Cells")
    {
        auto const size = 100;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::interval(size, 2, 1, note);

        REQUIRE(s.cells.size() == size);

        for (int i = 0; i < size; ++i)
        {
            if (i % 2 == 0)
            {
                REQUIRE(holds<Rest>(s.cells[i]));
            }
            else
            {
                REQUIRE((holds<Note>(s.cells[i]) && get<Note>(s.cells[i]) == note));
            }
        }
    }
}

TEST_CASE("Generate Random Sequences", "[sequence]")
{
    SECTION("Size Zero")
    {
        auto const size = 0;
        auto const density = 0.5f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("1000 Cells")
    {
        auto const size = 1000;
        auto const density = 0.5f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<Note>(cell))
            {
                REQUIRE(get<Note>(cell) == note);
                ++note_on_count;
            }
            else if (holds<Rest>(cell))
            {
            }
            else
            {
                REQUIRE(false);
            }
        }
        auto const percent = static_cast<float>(note_on_count) / s.cells.size();
        REQUIRE(percent == Approx(density).epsilon(0.1f));
    }

    SECTION("Density Zero")
    {
        auto const size = 1000;
        auto const density = 0.f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        auto rest_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<Note>(cell))
            {
                REQUIRE(get<Note>(cell) == note);
                ++note_on_count;
            }
            else if (holds<Rest>(cell))
            {
                ++rest_count;
            }
            else
            {
                REQUIRE(false);
            }
        }
        REQUIRE(note_on_count == 0);
        REQUIRE(rest_count == size);
    }

    SECTION("Density One")
    {
        auto const size = 1000;
        auto const density = 1.f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = Note{5, velocity, delay, gate};
        auto const s = generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        auto rest_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<Note>(cell))
            {
                REQUIRE(get<Note>(cell) == note);
                ++note_on_count;
            }
            else if (holds<Rest>(cell))
            {
                ++rest_count;
            }
            else
            {
                REQUIRE(false);
            }
        }
        REQUIRE(note_on_count == size);
        REQUIRE(rest_count == 0);
    }
}