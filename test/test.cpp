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
    s.cells.push_back(seq::NoteOn{0, 0.5, 0.5});
    s.cells.push_back(seq::NoteOff{});
    s.cells.push_back(seq::Rest{});
    s.cells.push_back(seq::Sequence{});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::NoteOn{0, 0.5, 0.5});
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