#include <filesystem>
#include <variant>

#include <sequence/generate.hpp>
#include <sequence/scale.hpp>
#include <sequence/sequence.hpp>

#include "catch.hpp"

namespace seq = sequence;

template <typename T, typename X>
[[nodiscard]] inline auto holds(X const &x) -> bool
{
    return std::holds_alternative<T>(x);
}

TEST_CASE("Sequence", "[sequence]")
{
    auto s = seq::Sequence{};
    s.cells.push_back(seq::NoteOn{0, 0.5, 0.5, 0.5});
    s.cells.push_back(seq::NoteOff{});
    s.cells.push_back(seq::Rest{});
    s.cells.push_back(seq::Sequence{});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::NoteOn{0, 0.5, 0.5, 0.5});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::NoteOff{});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::Rest{});
    REQUIRE(holds<seq::NoteOn>(s.cells[0]));
    REQUIRE(holds<seq::NoteOff>(s.cells[1]));
    REQUIRE(holds<seq::Rest>(s.cells[2]));
    REQUIRE(holds<seq::Sequence>(s.cells[3]));
    REQUIRE(holds<seq::NoteOn>(std::get<seq::Sequence>(s.cells[3]).cells[0]));
    REQUIRE(holds<seq::NoteOff>(std::get<seq::Sequence>(s.cells[3]).cells[1]));
    REQUIRE(holds<seq::Rest>(std::get<seq::Sequence>(s.cells[3]).cells[2]));
}

TEST_CASE("Scala file import", "[sequence]")
{
    constexpr auto file = "../test/12-edo.scl";
    auto const scale = seq::from_scala(file);
    REQUIRE(scale.size() == 12 + 1);
    REQUIRE(scale[0] == 0.f);
    REQUIRE(scale[1] == 100.f);
    REQUIRE(scale[2] == 200.f);
    REQUIRE(scale[3] == 300.f);
    REQUIRE(scale[4] == 400.f);
    REQUIRE(scale[5] == 500.f);
    REQUIRE(scale[6] == 600.f);
    REQUIRE(scale[7] == 700.f);
    REQUIRE(scale[8] == 800.f);
    REQUIRE(scale[9] == 900.f);
    REQUIRE(scale[10] == 1000.f);
    REQUIRE(scale[11] == 1100.f);
}

TEST_CASE("No scl archive files will throw errors", "[sequence]")
{
    constexpr auto dir = "../test/scl-archive/";

    for (auto const &entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.path().extension() == ".scl")
        {
            REQUIRE_NOTHROW(seq::from_scala(entry.path().c_str()));
        }
    }
}

TEST_CASE("Generate Empty Sequences", "[sequence]")
{
    SECTION("Zero Cells")
    {
        auto const size = 0;
        auto const s = seq::generate::empty(size);
        REQUIRE(s.cells.size() == size);
    }

    SECTION("4 Cells")
    {
        auto const size = 4;
        auto const s = seq::generate::empty(size);
        REQUIRE(s.cells.size() == size);

        for (auto const &cell : s.cells)
        {
            REQUIRE(holds<seq::Rest>(cell));
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
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::full(size, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("100 Cells")
    {
        auto const size = 100;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::full(size, note);

        REQUIRE(s.cells.size() == size);

        for (auto const &cell : s.cells)
        {
            REQUIRE((holds<seq::NoteOn>(cell) && std::get<seq::NoteOn>(cell) == note));
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
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::interval(size, 2, 1, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("100 Cells")
    {
        auto const size = 100;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::interval(size, 2, 1, note);

        REQUIRE(s.cells.size() == size);

        for (int i = 0; i < size; ++i)
        {
            if (i % 2 == 0)
            {
                REQUIRE(holds<seq::Rest>(s.cells[i]));
            }
            else
            {
                REQUIRE((holds<seq::NoteOn>(s.cells[i]) &&
                         std::get<seq::NoteOn>(s.cells[i]) == note));
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
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);
    }

    SECTION("1000 Cells")
    {
        auto const size = 1000;
        auto const density = 0.5f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        auto rest_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<seq::NoteOn>(cell))
            {
                REQUIRE(std::get<seq::NoteOn>(cell) == note);
                ++note_on_count;
            }
            else if (holds<seq::Rest>(cell))
            {
                ++rest_count;
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
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        auto rest_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<seq::NoteOn>(cell))
            {
                REQUIRE(std::get<seq::NoteOn>(cell) == note);
                ++note_on_count;
            }
            else if (holds<seq::Rest>(cell))
            {
                ++rest_count;
            }
            else
            {
                REQUIRE(false);
            }
        }
        REQUIRE(note_on_count == 0);
    }

    SECTION("Density One")
    {
        auto const size = 1000;
        auto const density = 1.f;
        auto const velocity = 0.4f;
        auto const delay = 0.01f;
        auto const gate = 0.8f;
        auto const note = seq::NoteOn{5, velocity, delay, gate};
        auto const s = seq::generate::random(size, density, note);

        REQUIRE(s.cells.size() == size);

        auto note_on_count = 0;
        auto rest_count = 0;
        for (auto const &cell : s.cells)
        {
            if (holds<seq::NoteOn>(cell))
            {
                REQUIRE(std::get<seq::NoteOn>(cell) == note);
                ++note_on_count;
            }
            else if (holds<seq::Rest>(cell))
            {
                ++rest_count;
            }
            else
            {
                REQUIRE(false);
            }
        }
        REQUIRE(note_on_count == size);
    }
}