#include "catch.hpp"

#include <vector>

#include <sequence/midi.hpp>
#include <sequence/sequence.hpp>

using namespace sequence;

namespace
{

constexpr auto base_frequency = 440.f;
constexpr auto pb_range = 1.f;

auto twelve_edo() -> Tuning
{
    return Tuning{
        {
            0.f,
            100.f,
            200.f,
            300.f,
            400.f,
            500.f,
            600.f,
            700.f,
            800.f,
            900.f,
            1000.f,
            1100.f,
        },
        1200.f,
        "12edo",
    };
}

auto grail_tuning() -> Tuning
{
    return Tuning{
        {
            0.f,
            86.869027f,
            195.623009f,
            304.376991f,
            391.246018f,
            504.376991f,
            578.080960f,
            695.623009f,
            795.623009f,
            895.623009f,
            1013.165056f,
            1086.869026f,
        },
        1200.f,
        "grail",
    };
}

} // namespace

TEST_CASE("flatten_to_midi validates input", "[midi]")
{
    auto const note = std::vector<MusicElement>{Note{}};

    SECTION("throws on empty tuning")
    {
        REQUIRE_THROWS_AS(
            midi::flatten_to_midi(note, 0, 100, Tuning{}, base_frequency, pb_range),
            std::invalid_argument);
    }

    SECTION("throws on nonpositive base frequency")
    {
        auto const tuning = twelve_edo();

        REQUIRE_THROWS_AS(midi::flatten_to_midi(note, 0, 100, tuning, 0.f, pb_range),
                          std::invalid_argument);
        REQUIRE_THROWS_AS(midi::flatten_to_midi(note, 0, 100, tuning, -440.f, pb_range),
                          std::invalid_argument);
    }

    SECTION("throws on nonpositive pitch bend range")
    {
        auto const tuning = twelve_edo();

        REQUIRE_THROWS_AS(
            midi::flatten_to_midi(note, 0, 100, tuning, base_frequency, 0.f),
            std::invalid_argument);
        REQUIRE_THROWS_AS(
            midi::flatten_to_midi(note, 0, 100, tuning, base_frequency, -1.f),
            std::invalid_argument);
    }

    SECTION("throws when a visited sequence has zero total weight")
    {
        auto const tuning = twelve_edo();
        auto const elements = std::vector<MusicElement>{
            Sequence{{Cell{{Note{0}}, 0.f}, Cell{{Note{1}}, 0.f}}},
        };

        REQUIRE_THROWS_AS(
            midi::flatten_to_midi(elements, 0, 100, tuning, base_frequency, pb_range),
            std::invalid_argument);
    }
}

TEST_CASE("flatten_to_midi converts a single note timing span", "[midi]")
{
    auto const tuning = twelve_edo();

    SECTION("uses the entire span with zero delay and full gate")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0, .velocity = 1.f, .delay = 0.f, .gate = 1.f}}, 12, 100,
            tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 12,
                               .end = 112,
                               .note = 69,
                               .velocity = 127,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("applies delay within the allocated span")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0, .velocity = 1.f, .delay = 0.25f, .gate = 1.f}}, 10, 100,
            tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 35,
                               .end = 110,
                               .note = 69,
                               .velocity = 127,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("applies fractional gate within the allocated span")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0, .velocity = 1.f, .delay = 0.f, .gate = 0.25f}}, 10, 100,
            tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 10,
                               .end = 35,
                               .note = 69,
                               .velocity = 127,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("allows zero gate")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0, .velocity = 1.f, .delay = 0.f, .gate = 0.f}}, 10, 100,
            tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 10,
                               .end = 10,
                               .note = 69,
                               .velocity = 127,
                               .pitch_bend = 8'192},
                          });
    }
}

TEST_CASE("flatten_to_midi converts velocity and pitch mapping", "[midi]")
{
    SECTION("converts representative velocity values")
    {
        auto const actual = midi::flatten_to_midi(
            {
                Note{.pitch = 0, .velocity = 0.f},
                Note{.pitch = 1, .velocity = 0.5f},
                Note{.pitch = 2, .velocity = 1.f},
            },
            0, 10, twelve_edo(), base_frequency, pb_range);

        REQUIRE(
            actual ==
            std::vector<midi::TimedMidiNote>{
                {.begin = 0, .end = 10, .note = 69, .velocity = 0, .pitch_bend = 8'192},
                {.begin = 0,
                 .end = 10,
                 .note = 70,
                 .velocity = 63,
                 .pitch_bend = 8'192},
                {.begin = 0,
                 .end = 10,
                 .note = 71,
                 .velocity = 127,
                 .pitch_bend = 8'192},
            });
    }

    SECTION("uses centered pitch bend for 12edo")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0}, Note{.pitch = 2}, Note{.pitch = 12}}, 0, 10,
            twelve_edo(), base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 10,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 0,
                               .end = 10,
                               .note = 71,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 0,
                               .end = 10,
                               .note = 81,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("produces noncenter pitch bend for microtonal tunings")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0}, Note{.pitch = 1}, Note{.pitch = 2}}, 0, 10,
            grail_tuning(), base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 10,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 0,
                               .end = 10,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 15'308},
                              {.begin = 0,
                               .end = 10,
                               .note = 70,
                               .velocity = 88,
                               .pitch_bend = 16'025},
                          });
    }

    SECTION("clamps very low pitches to midi note zero")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = -100}}, 0, 10, twelve_edo(), base_frequency, pb_range);

        REQUIRE(
            actual ==
            std::vector<midi::TimedMidiNote>{
                {.begin = 0, .end = 10, .note = 0, .velocity = 88, .pitch_bend = 8'192},
            });
    }

    SECTION("clamps very high pitches to midi note 127")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 100}}, 0, 10, twelve_edo(), base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 10,
                               .note = 127,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }
}

TEST_CASE("flatten_to_midi handles silence and polyphony", "[midi]")
{
    auto const tuning = twelve_edo();

    SECTION("empty top level elements emit no notes")
    {
        auto const actual =
            midi::flatten_to_midi({}, 0, 100, tuning, base_frequency, pb_range);

        REQUIRE(actual.empty());
    }

    SECTION("multiple notes in one cell are simultaneous and preserve order")
    {
        auto const actual = midi::flatten_to_midi(
            {Note{.pitch = 0}, Note{.pitch = 4}, Note{.pitch = 7}}, 20, 30, tuning,
            base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 20,
                               .end = 50,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 20,
                               .end = 50,
                               .note = 73,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 20,
                               .end = 50,
                               .note = 76,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("empty child cells consume time but emit no notes")
    {
        auto const actual = midi::flatten_to_midi(
            {Sequence{{Cell{{}, 1.f}, Cell{{Note{.pitch = 0}}, 1.f}}}}, 0, 100, tuning,
            base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 50,
                               .end = 100,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("multiple notes in a nested sequence cell are simultaneous within that child span")
    {
        auto const actual = midi::flatten_to_midi(
            {Sequence{{Cell{{Note{.pitch = 0}, Note{.pitch = 4}}, 1.f},
                       Cell{{Note{.pitch = 7}}, 1.f}}}},
            0, 100, tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 50,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 0,
                               .end = 50,
                               .note = 73,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 50,
                               .end = 100,
                               .note = 76,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }
}

TEST_CASE("flatten_to_midi subdivides sequences by weight", "[midi]")
{
    auto const tuning = twelve_edo();

    SECTION("equal weights split the span evenly")
    {
        auto const actual = midi::flatten_to_midi(
            {Sequence{{Cell{{Note{.pitch = 0}}, 1.f}, Cell{{Note{.pitch = 2}}, 1.f}}}},
            0, 100, tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 50,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 50,
                               .end = 100,
                               .note = 71,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("unequal weights split the span proportionally")
    {
        auto const actual = midi::flatten_to_midi(
            {Sequence{{Cell{{Note{.pitch = 0}}, 1.f}, Cell{{Note{.pitch = 2}}, 2.f},
                       Cell{{Note{.pitch = 4}}, 1.f}}}},
            0, 80, tuning, base_frequency, pb_range);

        REQUIRE(actual == std::vector<midi::TimedMidiNote>{
                              {.begin = 0,
                               .end = 20,
                               .note = 69,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 20,
                               .end = 60,
                               .note = 71,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                              {.begin = 60,
                               .end = 80,
                               .note = 73,
                               .velocity = 88,
                               .pitch_bend = 8'192},
                          });
    }

    SECTION("rounds child boundaries and forces the last child to end exactly")
    {
        auto const actual = midi::flatten_to_midi(
            {Sequence{{Cell{{Note{.pitch = 0}}, 1.f}, Cell{{Note{.pitch = 1}}, 1.f},
                       Cell{{Note{.pitch = 2}}, 1.f}}}},
            0, 10, tuning, base_frequency, pb_range);

        REQUIRE(
            actual ==
            std::vector<midi::TimedMidiNote>{
                {.begin = 0, .end = 3, .note = 69, .velocity = 88, .pitch_bend = 8'192},
                {.begin = 3, .end = 7, .note = 70, .velocity = 88, .pitch_bend = 8'192},
                {.begin = 7,
                 .end = 10,
                 .note = 71,
                 .velocity = 88,
                 .pitch_bend = 8'192},
            });
    }
}

TEST_CASE("flatten_to_midi handles nested simultaneous and sequential structure",
          "[midi]")
{
    auto const actual = midi::flatten_to_midi(
        {Note{.pitch = 0}, Sequence{{Cell{{Note{.pitch = 1}}, 1.f}, Cell{{}, 1.f},
                                     Cell{{Note{.pitch = 4}}, 2.f}}}},
        10, 40, twelve_edo(), base_frequency, pb_range);

    REQUIRE(
        actual ==
        std::vector<midi::TimedMidiNote>{
            {.begin = 10, .end = 50, .note = 69, .velocity = 88, .pitch_bend = 8'192},
            {.begin = 10, .end = 20, .note = 70, .velocity = 88, .pitch_bend = 8'192},
            {.begin = 30, .end = 50, .note = 73, .velocity = 88, .pitch_bend = 8'192},
        });
}
