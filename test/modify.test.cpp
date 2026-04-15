#include "catch.hpp"

#include <algorithm>
#include <vector>

#include <sequence/modify.hpp>
#include <sequence/sequence.hpp>

using namespace sequence;

namespace
{

auto note_cell(int pitch,
               float velocity = 0.7f,
               float delay = 0.f,
               float gate = 1.f,
               float weight = 1.f) -> Cell
{
    return {.elements = {Note{pitch, velocity, delay, gate}}, .weight = weight};
}

auto silent_cell(float weight = 1.f) -> Cell
{
    return {.elements = {}, .weight = weight};
}

auto sequence_cell(std::vector<Cell> cells, float weight = 1.f) -> Cell
{
    return {.elements = {Sequence{std::move(cells)}}, .weight = weight};
}

auto as_note(MusicElement const &element) -> Note const &
{
    return std::get<Note>(element);
}

auto as_note(Cell const &cell) -> Note const &
{
    return std::get<Note>(cell.elements.front());
}

auto as_sequence(MusicElement const &element) -> Sequence const &
{
    return std::get<Sequence>(element);
}

auto as_sequence(Cell const &cell) -> Sequence const &
{
    return std::get<Sequence>(cell.elements.front());
}

template <typename Fn>
auto for_each_note(MusicElement const &element, Fn &&fn) -> void
{
    std::visit(
        [&](auto const &value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::same_as<T, Note>)
            {
                fn(value);
            }
            else
            {
                for (auto const &cell : value.cells)
                {
                    for (auto const &nested : cell.elements)
                    {
                        for_each_note(nested, fn);
                    }
                }
            }
        },
        element);
}

template <typename Fn>
auto for_each_note(Cell const &cell, Fn &&fn) -> void
{
    for (auto const &element : cell.elements)
    {
        for_each_note(element, fn);
    }
}

auto collect_pitches(MusicElement const &element) -> std::vector<int>
{
    auto pitches = std::vector<int>{};
    for_each_note(element, [&](Note const &note) { pitches.push_back(note.pitch); });
    return pitches;
}

auto collect_pitches(Cell const &cell) -> std::vector<int>
{
    auto pitches = std::vector<int>{};
    for_each_note(cell, [&](Note const &note) { pitches.push_back(note.pitch); });
    return pitches;
}

} // namespace

TEST_CASE("note creates a note element and validates ranges", "[modify]")
{
    REQUIRE(modify::note(7, 0.4f, 0.2f, 0.8f) ==
            MusicElement{Note{7, 0.4f, 0.2f, 0.8f}});

    REQUIRE_THROWS_AS(modify::note(0, -0.1f, 0.f, 1.f), std::invalid_argument);
    REQUIRE_THROWS_AS(modify::note(0, 0.5f, 1.1f, 1.f), std::invalid_argument);
    REQUIRE_THROWS_AS(modify::note(0, 0.5f, 0.5f, -0.1f), std::invalid_argument);
}

TEST_CASE("randomize family supports element and cell targets", "[modify]")
{
    SECTION("randomize_pitch updates nested notes in a music element")
    {
        auto const target = MusicElement{Sequence{{note_cell(0),
                                                   silent_cell(),
                                                   sequence_cell(
                                                       {note_cell(1), note_cell(2)})}}};

        auto const randomized = modify::randomize_pitch(target, {0, {1}}, 10, 12);
        auto const pitches = collect_pitches(randomized);

        REQUIRE(pitches.size() == 3);
        REQUIRE(std::ranges::all_of(
            pitches, [](int pitch) { return pitch >= 10 && pitch <= 12; }));
        REQUIRE(as_sequence(randomized).cells[1].elements.empty());
    }

    SECTION("randomize_velocity preserves cell weight and note count")
    {
        auto const target = Cell{
            .elements =
                {
                    Note{0, 0.2f, 0.1f, 0.3f},
                    Sequence{{note_cell(1, 0.9f), silent_cell(), note_cell(2, 0.4f)}},
                },
            .weight = 2.5f,
        };

        auto const randomized = modify::randomize_velocity(target, {0, {1}}, 0.3f, 0.6f);

        REQUIRE(randomized.weight == target.weight);
        REQUIRE(as_sequence(randomized.elements[1]).cells[1].elements.empty());
        for_each_note(randomized, [](Note const &note) {
            REQUIRE(note.velocity >= 0.3f);
            REQUIRE(note.velocity <= 0.6f);
        });
    }

    SECTION("randomize_delay validates bounds and updates note delays")
    {
        auto const target =
            MusicElement{Sequence{{note_cell(0, 0.7f, 0.1f), note_cell(1, 0.7f, 0.8f)}}};

        auto const randomized = modify::randomize_delay(target, {0, {1}}, 0.2f, 0.7f);
        for_each_note(randomized, [](Note const &note) {
            REQUIRE(note.delay >= 0.2f);
            REQUIRE(note.delay <= 0.7f);
        });

        REQUIRE_THROWS_AS(modify::randomize_delay(target, {0, {1}}, 0.8f, 0.2f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(modify::randomize_delay(target, {0, {1}}, -0.1f, 0.2f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(modify::randomize_delay(target, {0, {1}}, 0.2f, 1.1f),
                          std::invalid_argument);
    }

    SECTION("randomize_gate validates bounds and updates note gates")
    {
        auto const target = Cell{
            .elements = {Sequence{{note_cell(0, 0.7f, 0.1f, 0.2f),
                                   note_cell(1, 0.7f, 0.1f, 0.9f)}}},
            .weight = 1.25f,
        };

        auto const randomized = modify::randomize_gate(target, {0, {1}}, 0.2f, 0.7f);

        REQUIRE(randomized.weight == target.weight);
        for_each_note(randomized, [](Note const &note) {
            REQUIRE(note.gate >= 0.2f);
            REQUIRE(note.gate <= 0.7f);
        });

        REQUIRE_THROWS_AS(modify::randomize_gate(target, {0, {1}}, 0.8f, 0.2f),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(modify::randomize_gate(target, {0, {1}}, 0.2f, -0.1f),
                          std::invalid_argument);
    }
}

TEST_CASE("shift family supports element and cell targets", "[modify]")
{
    SECTION("shift_pitch updates nested pitches in an element")
    {
        auto const target =
            MusicElement{Sequence{{note_cell(1), sequence_cell({note_cell(2), note_cell(3)})}}};

        auto const shifted = modify::shift_pitch(target, {0, {1}}, 5);

        REQUIRE(collect_pitches(shifted) == std::vector<int>{6, 7, 8});
    }

    SECTION("shift_velocity clamps into [0, 1]")
    {
        auto const shifted = modify::shift_velocity(
            Cell{.elements = {Note{0, 0.2f, 0.1f, 0.3f}, Note{1, 0.9f, 0.2f, 0.4f}}},
            {0, {1}}, 0.3f);

        REQUIRE(as_note(shifted.elements[0]).velocity == 0.5f);
        REQUIRE(as_note(shifted.elements[1]).velocity == 1.f);
    }

    SECTION("shift_delay clamps into [0, 1]")
    {
        auto const shifted = modify::shift_delay(
            Cell{.elements = {Note{0, 0.2f, 0.1f, 0.3f}, Note{1, 0.9f, 0.8f, 0.4f}}},
            {0, {1}}, -0.3f);

        REQUIRE(as_note(shifted.elements[0]).delay == 0.f);
        REQUIRE(as_note(shifted.elements[1]).delay == 0.5f);
    }

    SECTION("shift_gate clamps into [0, 1]")
    {
        auto const shifted = modify::shift_gate(
            Cell{.elements = {Note{0, 0.2f, 0.1f, 0.3f}, Note{1, 0.9f, 0.8f, 0.9f}}},
            {0, {1}}, 0.3f);

        REQUIRE(as_note(shifted.elements[0]).gate == 0.6f);
        REQUIRE(as_note(shifted.elements[1]).gate == 1.f);
    }
}

TEST_CASE("set family supports element and cell targets", "[modify]")
{
    SECTION("set_pitch updates note pitch in an element target")
    {
        auto const target =
            MusicElement{Sequence{{note_cell(1), sequence_cell({note_cell(2), note_cell(3)})}}};

        auto const updated = modify::set_pitch(target, {0, {1}}, 9);

        REQUIRE(collect_pitches(updated) == std::vector<int>{9, 9, 9});
    }

    SECTION("set_octave preserves scale degree")
    {
        auto const target =
            MusicElement{Sequence{{note_cell(-1), note_cell(14), note_cell(24)}}};

        auto const updated = modify::set_octave(target, {0, {1}}, 3, 12);

        REQUIRE(collect_pitches(updated) == std::vector<int>{47, 38, 36});
        REQUIRE_THROWS_AS(modify::set_octave(target, {0, {1}}, 3, 0),
                          std::invalid_argument);
    }

    SECTION("set_velocity, set_delay, and set_gate clamp in a cell target")
    {
        auto const target = Cell{
            .elements = {Note{0, 0.2f, 0.1f, 0.3f}, Note{1, 0.9f, 0.8f, 0.9f}},
            .weight = 4.f,
        };

        auto const velocity = modify::set_velocity(target, {0, {1}}, 2.f);
        auto const delay = modify::set_delay(target, {0, {1}}, -0.5f);
        auto const gate = modify::set_gate(target, {0, {1}}, 1.5f);

        REQUIRE(velocity.weight == target.weight);
        REQUIRE(as_note(velocity.elements[0]).velocity == 1.f);
        REQUIRE(as_note(velocity.elements[1]).velocity == 1.f);

        REQUIRE(as_note(delay.elements[0]).delay == 0.f);
        REQUIRE(as_note(delay.elements[1]).delay == 0.f);

        REQUIRE(as_note(gate.elements[0]).gate == 1.f);
        REQUIRE(as_note(gate.elements[1]).gate == 1.f);
    }
}

TEST_CASE("rotate, mirror, and reverse transform sequential content", "[modify]")
{
    SECTION("rotate rotates sequence cells but not simultaneous elements")
    {
        auto const target = Cell{
            .elements =
                {
                    Note{9, 0.4f, 0.1f, 0.7f},
                    Sequence{{note_cell(0), note_cell(1), note_cell(2), note_cell(3)}},
                },
            .weight = 2.f,
        };

        auto const rotated = modify::rotate(target, 1);
        auto const &seq = as_sequence(rotated.elements[1]);

        REQUIRE(std::holds_alternative<Note>(rotated.elements[0]));
        REQUIRE(collect_pitches(rotated) == std::vector<int>{9, 3, 0, 1, 2});
        REQUIRE(seq.cells.size() == 4);
    }

    SECTION("mirror respects center note across nested sequences")
    {
        auto const target =
            MusicElement{Sequence{{note_cell(0), sequence_cell({note_cell(2), note_cell(4)})}}};

        auto const mirrored = modify::mirror(target, {0, {1}}, 5);

        REQUIRE(collect_pitches(mirrored) == std::vector<int>{10, 8, 6});
    }

    SECTION("reverse recurses through nested sequences")
    {
        auto const target = Cell{
            .elements =
                {
                    Note{9, 0.4f, 0.1f, 0.7f},
                    Sequence{{note_cell(0),
                              sequence_cell({note_cell(1), note_cell(2)}),
                              silent_cell(),
                              note_cell(3)}},
                },
            .weight = 1.f,
        };

        auto const reversed = modify::reverse(target);
        auto const &seq = as_sequence(reversed.elements[1]);

        REQUIRE(std::holds_alternative<Note>(reversed.elements[0]));
        REQUIRE(as_note(reversed.elements[0]).pitch == 9);
        REQUIRE(as_note(seq.cells[0]).pitch == 3);
        REQUIRE(seq.cells[1].elements.empty());
        REQUIRE(as_sequence(seq.cells[2]).cells[0] == note_cell(2));
        REQUIRE(as_sequence(seq.cells[2]).cells[1] == note_cell(1));
        REQUIRE(as_note(seq.cells[3]).pitch == 0);
    }
}

TEST_CASE("repeat, stretch, and compress reshape sequential structure", "[modify]")
{
    SECTION("repeat duplicates a music element into a sequence")
    {
        auto const repeated = modify::repeat(modify::note(7, 0.4f, 0.2f, 0.8f), 3);
        auto const &seq = as_sequence(repeated);

        REQUIRE(seq.cells.size() == 3);
        REQUIRE(seq.cells[0] == note_cell(7, 0.4f, 0.2f, 0.8f));
        REQUIRE(seq.cells[1] == note_cell(7, 0.4f, 0.2f, 0.8f));
        REQUIRE(seq.cells[2] == note_cell(7, 0.4f, 0.2f, 0.8f));
    }

    SECTION("repeat duplicates a cell and preserves wrapper weight")
    {
        auto const cell = note_cell(2, 0.5f, 0.1f, 0.6f, 3.f);
        auto const repeated = modify::repeat(cell, 2);
        auto const &seq = as_sequence(repeated);

        REQUIRE(repeated.weight == 3.f);
        REQUIRE(seq.cells == std::vector<Cell>{cell, cell});
        REQUIRE_THROWS_AS(modify::repeat(cell, 0), std::invalid_argument);
    }

    SECTION("stretch wraps selected note cells in repeated sequences")
    {
        auto const target = sequence_cell({note_cell(0), silent_cell(), note_cell(1)});
        auto const stretched = modify::stretch(target, {0, {1}}, 2);
        auto const &seq = as_sequence(stretched);

        REQUIRE(as_sequence(seq.cells[0]).cells ==
                std::vector<Cell>{note_cell(0), note_cell(0)});
        REQUIRE(seq.cells[1].elements.empty());
        REQUIRE(as_sequence(seq.cells[2]).cells ==
                std::vector<Cell>{note_cell(1), note_cell(1)});

        REQUIRE_THROWS_AS(modify::stretch(target, {0, {1}}, 0), std::invalid_argument);
    }

    SECTION("compress filters sequence cells and leaves simultaneous notes alone")
    {
        auto const target = Cell{
            .elements =
                {
                    Note{9, 0.4f, 0.1f, 0.7f},
                    Sequence{{note_cell(0), silent_cell(), note_cell(1), note_cell(2)}},
                },
            .weight = 1.f,
        };

        auto const compressed = modify::compress(target, {0, {2}});
        auto const &seq = as_sequence(compressed.elements[1]);

        REQUIRE(as_note(compressed.elements[0]).pitch == 9);
        REQUIRE(seq.cells == std::vector<Cell>{note_cell(0), note_cell(1)});
        REQUIRE_THROWS_AS(modify::compress(target, {0, {}}), std::invalid_argument);
    }
}

TEST_CASE("shuffle preserves sequence contents while reordering sequence cells", "[modify]")
{
    auto const target = Cell{
        .elements =
            {
                Note{9, 0.4f, 0.1f, 0.7f},
                Sequence{{note_cell(0), note_cell(1), note_cell(2), note_cell(3)}},
            },
        .weight = 1.f,
    };

    auto const shuffled = modify::shuffle(target);
    auto const &original_seq = as_sequence(target.elements[1]);
    auto const &shuffled_seq = as_sequence(shuffled.elements[1]);

    REQUIRE(std::holds_alternative<Note>(shuffled.elements[0]));
    REQUIRE(as_note(shuffled.elements[0]).pitch == 9);
    REQUIRE(shuffled_seq.cells.size() == original_seq.cells.size());
    REQUIRE(std::ranges::is_permutation(original_seq.cells, shuffled_seq.cells));
}

TEST_CASE("cell and element overloads support common plugin-style selection", "[modify]")
{
    auto const selected_element = MusicElement{Note{3, 0.5f, 0.2f, 0.4f}};
    auto const selected_cell = Cell{
        .elements =
            {
                Note{0, 0.5f, 0.f, 1.f},
                Note{7, 0.5f, 0.f, 1.f},
            },
        .weight = 2.f,
    };

    auto const shifted_element = modify::shift_pitch(selected_element, {0, {1}}, 12);
    auto const set_cell = modify::set_pitch(selected_cell, {0, {1}}, 4);

    REQUIRE(as_note(shifted_element).pitch == 15);
    REQUIRE(collect_pitches(set_cell) == std::vector<int>{4, 4});
    REQUIRE(set_cell.weight == selected_cell.weight);
}
