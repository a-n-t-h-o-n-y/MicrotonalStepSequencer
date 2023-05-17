#include "catch.hpp"

#include <variant>

#include <sequence/sequence.hpp>
#include <sequence/generate.hpp>

TEST_CASE("Sequence")
{
    namespace seq = sequence;
    auto s = seq::Sequence{};
    s.cells.push_back(seq::NoteOn{0, 0.5, 0.5});
    s.cells.push_back(seq::NoteOff{});
    s.cells.push_back(seq::Rest{});
    s.cells.push_back(seq::Sequence{});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::NoteOn{0, 0.5, 0.5});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::NoteOff{});
    std::get<seq::Sequence>(s.cells[3]).cells.push_back(seq::Rest{});
    REQUIRE(std::holds_alternative<seq::NoteOn>(s.cells[0]));
    REQUIRE(std::holds_alternative<seq::NoteOff>(s.cells[1]));
    REQUIRE(std::holds_alternative<seq::Rest>(s.cells[2]));
    REQUIRE(std::holds_alternative<seq::Sequence>(s.cells[3]));
    REQUIRE(std::holds_alternative<seq::NoteOn>(std::get<seq::Sequence>(s.cells[3]).cells[0]));
    REQUIRE(std::holds_alternative<seq::NoteOff>(std::get<seq::Sequence>(s.cells[3]).cells[1]));
    REQUIRE(std::holds_alternative<seq::Rest>(std::get<seq::Sequence>(s.cells[3]).cells[2]));
}