#include "catch.hpp"

#include <cstdint>
#include <iostream>
#include <map>

#include "helper.hpp"
#include <sequence/measure.hpp>
#include <sequence/midi.hpp>
#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

using namespace sequence;

TEST_CASE("create_midi_note", "[midi]")
{
    SECTION("throw if tuning is empty")
    {
        REQUIRE_THROWS_AS(midi::create_midi_note(0, {}, 0.f, 1), std::invalid_argument);
    }

    SECTION("12edo (12 equal divisions of the octave)")
    {
        auto const tuning = Tuning{
            {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
            1200,
            "description",
        };
        auto const tuning_base = 60.f;

        for (auto i = std::size_t{0}; i < tuning.intervals.size() + 1; ++i)
        {
            REQUIRE(midi::create_midi_note(i, tuning, tuning_base, 1) ==
                    midi::MicrotonalNote{
                        .note = (std::uint8_t)(60 + i),
                        .pitch_bend = 8'192,
                    });
        }
    }

    SECTION("grail (microtonal)")
    {
        auto const tuning = Tuning{
            {0.f, 86.869027f, 195.623009f, 304.376991f, 391.246018f, 504.376991f,
             578.080960f, 695.623009f, 795.623009f, 895.623009f, 1013.165056f,
             1086.869026f},
            1200.f,
            "description",
        };

        SECTION("tuning_base = 60")
        {
            auto const tuning_base = 60.f;

            auto const table = std::map<int, midi::MicrotonalNote>{
                {0, {.note = 60, .pitch_bend = 8'192}},
                {1, {.note = 60, .pitch_bend = 15'308}},
                {2, {.note = 61, .pitch_bend = 16'025}},
                {12, {.note = 72, .pitch_bend = 8'192}},
            };

            for (auto const &[pitch, expected] : table)
            {
                REQUIRE(midi::create_midi_note(pitch, tuning, tuning_base, 1) ==
                        expected);
            }
        }

        SECTION("tuning_base = 10.5")
        {
            auto const tuning_base = 10.5f;

            auto const table = std::map<int, midi::MicrotonalNote>{
                {0, {.note = 10, .pitch_bend = 12'288}},
                {1, {.note = 11, .pitch_bend = 11'212}},
                {2, {.note = 12, .pitch_bend = 11'929}},
                {12, {.note = 22, .pitch_bend = 12'288}},
                {13, {.note = 23, .pitch_bend = 11'212}},
                {14, {.note = 24, .pitch_bend = 11'929}},
                {15, {.note = 25, .pitch_bend = 12'646}},
                {-1, {.note = 9, .pitch_bend = 11'212}},
                {-2, {.note = 8, .pitch_bend = 13'366}},
                {-3, {.note = 7, .pitch_bend = 11'929}},
            };

            for (auto const &[pitch, expected] : table)
            {
                REQUIRE(midi::create_midi_note(pitch, tuning, tuning_base, 1) ==
                        expected);
            }
        }

        SECTION("Negative Interval over tuning length")
        {
            auto const tuning_base = 40.75f;

            auto const table = std::map<int, midi::MicrotonalNote>{
                {-13, {.note = 27, .pitch_bend = 13'260}},
                {-17, {.note = 23, .pitch_bend = 13'977}},
                {-25, {.note = 15, .pitch_bend = 13'260}},
            };

            for (auto const &[pitch, expected] : table)
            {
                REQUIRE(midi::create_midi_note(pitch, tuning, tuning_base, 1) ==
                        expected);
            }
        }

        SECTION("Zero is the lowest note")
        {
            auto const tuning_base = 10.5f;

            REQUIRE(midi::create_midi_note(-11, tuning, tuning_base, 1) ==
                    midi::MicrotonalNote{.note = 0, .pitch_bend = 8'192});

            REQUIRE(midi::create_midi_note(-100, tuning, tuning_base, 1) ==
                    midi::MicrotonalNote{.note = 0, .pitch_bend = 8'192});
        }

        SECTION("127 is the highest note")
        {
            auto const tuning_base = 100.f;

            REQUIRE(midi::create_midi_note(28, tuning, tuning_base, 1) ==
                    midi::MicrotonalNote{.note = 127, .pitch_bend = 8'192});

            REQUIRE(midi::create_midi_note(156, tuning, tuning_base, 1) ==
                    midi::MicrotonalNote{.note = 127, .pitch_bend = 8'192});
        }
    }

    SECTION("Single Note Tuning")
    {
        auto const tuning = Tuning{{0.f}, 1200.f, "description"};
        auto const tuning_base = 63.f;

        auto const table = std::map<int, midi::MicrotonalNote>{
            {0, {.note = 63, .pitch_bend = 8'192}},
            {1, {.note = 75, .pitch_bend = 8'192}},
            {-1, {.note = 51, .pitch_bend = 8'192}},
            {-2, {.note = 39, .pitch_bend = 8'192}},
        };

        for (auto const &[pitch, expected] : table)
        {
            REQUIRE(midi::create_midi_note(pitch, tuning, tuning_base, 1) == expected);
        }
    }

    SECTION("anharmonic tunings (e.g. non-octave based)")
    {
        // 13 Equal Divisions of the Tritave
        auto const tuning = Tuning{
            {0.f, 146.3f, 292.6f, 438.9f, 585.2f, 731.5f, 877.8f, 1024.1f, 1170.4f,
             1316.7f, 1463.0f, 1609.3f, 1755.7f},
            1902.f,
            "description",
        };
        auto const tuning_base = 60.f;

        auto const table = std::map<int, midi::MicrotonalNote>{
            {0, {.note = 60, .pitch_bend = 8'192}},
            {1, {.note = 61, .pitch_bend = 11'984}},
            {12, {.note = 77, .pitch_bend = 12'754}},
            {13, {.note = 79, .pitch_bend = 8'355}},
            {14, {.note = 80, .pitch_bend = 12'148}},
            {-1, {.note = 58, .pitch_bend = 12'591}},
            {-12, {.note = 42, .pitch_bend = 11'821}},
            {-13, {.note = 40, .pitch_bend = 16'220}},
            {-14, {.note = 39, .pitch_bend = 12'427}},
        };

        for (auto const &[pitch, expected] : table)
        {
            REQUIRE(midi::create_midi_note(pitch, tuning, tuning_base, 1) == expected);
        }
    }
}

TEST_CASE("flatten_and_translate_to_midi_notes", "[midi]")
{
    auto const tuning = Tuning{
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
        1200,
        "description",
    };

    auto const seq = Sequence{{
        {Sequence{{
            {Note{0, 0.75f, 0.f, 1.f}},
            {Rest{}},
            {Note{0 + 2, 0.75f, 0.f, 1.f}},
        }}},
        {Note{1, 0.75f, 0.f, 1.f}},
        {Sequence{{
            {Note{2, 0.75f, 0.f, 1.f}},
            {Rest{}},
            {Note{2 + 2, 0.75f, 0.f, 1.f}},
        }}},
        {Note{3, 0.75f, 0.f, 1.f}},
        {Sequence{{
            {Note{4, 0.75f, 0.f, 1.f}},
            {Rest{}},
            {Note{4 + 2, 0.75f, 0.f, 1.f}},
        }}},
        {Note{5, 0.75f, 0.f, 1.f}},
        {Sequence{{
            {Note{6, 0.75f, 0.f, 1.f}},
            {Rest{}},
            {Note{6 + 2, 0.75f, 0.f, 1.f}},
        }}},
        {Note{7, 0.75f, 0.f, 1.f}},
    }};

    auto const notes = midi::flatten_and_translate_to_midi_notes(Measure{seq}, tuning);

    auto const expected = std::vector<midi::MicrotonalNote>{
        {.note = 69, .pitch_bend = 8'192}, {.note = 71, .pitch_bend = 8'192},
        {.note = 70, .pitch_bend = 8'192}, {.note = 71, .pitch_bend = 8'192},
        {.note = 73, .pitch_bend = 8'192}, {.note = 72, .pitch_bend = 8'192},
        {.note = 73, .pitch_bend = 8'192}, {.note = 75, .pitch_bend = 8'192},
        {.note = 74, .pitch_bend = 8'192}, {.note = 75, .pitch_bend = 8'192},
        {.note = 77, .pitch_bend = 8'192}, {.note = 76, .pitch_bend = 8'192},
    };

    REQUIRE(notes == expected);
}

TEST_CASE("flatten_notes", "[midi]")
{
    auto const seq = Sequence{{
        {Rest{}},
        {Note{0, 0.5f, 0.f, 1.f}},
        {Sequence{{
            {Note{1, 0.5f, 0.f, 1.f}},
            {Rest{}},
            {Note{2, 0.5f, 0.f, 1.f}},
        }}},
        {Rest{}},
        {Note{3, 0.5f, 0.f, 1.f}},
    }};

    auto const flat = midi::flatten_notes(seq);

    auto const expected = std::vector<Note>{
        {0, 0.5f, 0.f, 1.f},
        {1, 0.5f, 0.f, 1.f},
        {2, 0.5f, 0.f, 1.f},
        {3, 0.5f, 0.f, 1.f},
    };

    REQUIRE(flat == expected);
}

TEST_CASE("flatten_and_translate_to_sample_infos", "[midi]")
{

    auto const tuning = Tuning{
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
        1200,
        "description",
    };

    SECTION("Straight")
    {
        auto const seq = Sequence{{
            {Sequence{{
                {Note{}},
                {Rest{}},
                {Note{}},
            }}},
            {Note{}},
            {Sequence{{
                {Note{}},
                {Rest{}},
                {Note{}},
            }}},
            {Note{}},
        }};

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 240);

        auto const expected = std::vector<midi::SampleRange>{
            {.begin = 0, .end = 3'675},       {.begin = 7'350, .end = 11'025},
            {.begin = 11'025, .end = 22'050}, {.begin = 22'050, .end = 25'725},
            {.begin = 29'400, .end = 33'075}, {.begin = 33'075, .end = 44'100},
        };

        REQUIRE(infos == expected);
    }

    SECTION("With gate")
    {
        auto const seq = Sequence{{
            {Sequence{{
                {Note{.gate = 0.5f}},
                {Rest{}},
                {Note{.gate = 0.25f}},
            }}},
            {Note{.gate = 0.f}}, // 0 gate is allowed but not recommended
            {Sequence{{
                {Note{.gate = 0.5f}},
                {Rest{}},
                {Note{.gate = 0.25f}},
            }}},
            {Note{.gate = 0.f}},
        }};

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 240);

        auto const expected = std::vector<midi::SampleRange>{
            {.begin = 0, .end = 1'837},       {.begin = 7'350, .end = 8'268},
            {.begin = 11'025, .end = 11'025}, {.begin = 22'050, .end = 23'887},
            {.begin = 29'400, .end = 30'318}, {.begin = 33'075, .end = 33'075},
        };

        REQUIRE(infos == expected);
    }

    SECTION("With delay")
    {
        auto const seq = Sequence{{
            {Sequence{{
                {Note{.delay = 0.5f}},
                {Rest{}},
                {Note{.delay = 0.25f}},
            }}},
            {Note{.delay = 1.f}},
            {Sequence{{
                {Note{.delay = 0.5f}},
                {Rest{}},
                {Note{.delay = 0.25f}},
            }}},
            {Note{.delay = 1.f}},
        }};

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 240);

        auto const expected = std::vector<midi::SampleRange>{
            {.begin = 1'837, .end = 3'675},   {.begin = 8'268, .end = 11'025},
            {.begin = 22'050, .end = 22'050}, {.begin = 23'887, .end = 25'725},
            {.begin = 30'318, .end = 33'075}, {.begin = 44'100, .end = 44'100},
        };

        REQUIRE(infos == expected);
    }

    SECTION("With delay and gate")
    {
        auto const seq = Sequence{{
            {Sequence{{
                {Note{.delay = 0.5f, .gate = 0.8f}},
                {Rest{}},
                {Note{.delay = 0.25f, .gate = 0.1f}},
            }}},
            {Note{.delay = 1.f, .gate = 0.1f}},
            {Sequence{{
                {Note{.delay = 0.5f, .gate = 0.8f}},
                {Rest{}},
                {Note{.delay = 0.25f, .gate = 0.1f}},
            }}},
            {Note{.delay = 1.f, .gate = 0.1f}},
            {Sequence{{
                {Note{.delay = 0.5f, .gate = 0.8f}},
                {Rest{}},
                {Note{.delay = 0.25f, .gate = 0.1f}},
            }}},
            {Note{.delay = 1.f, .gate = 0.1f}},
            {Sequence{{
                {Note{.delay = 0.5f, .gate = 0.8f}},
                {Rest{}},
                {Note{.delay = 0.25f, .gate = 0.1f}},
            }}},
            {Note{.delay = 1.f, .gate = 0.1f}},
        }};

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 120);

        auto const expected = std::vector<midi::SampleRange>{
            {.begin = 1'837, .end = 3'307},   {.begin = 8'268, .end = 8'543},
            {.begin = 22'050, .end = 22'050}, {.begin = 23'887, .end = 25'357},
            {.begin = 30'318, .end = 30'593}, {.begin = 44'100, .end = 44'100},
            {.begin = 45'937, .end = 47'407}, {.begin = 52'368, .end = 52'643},
            {.begin = 66'150, .end = 66'150}, {.begin = 67'987, .end = 69'457},
            {.begin = 74'418, .end = 74'693}, {.begin = 88'200, .end = 88'200},
        };

        REQUIRE(infos == expected);
    }

    SECTION("With Weights")
    {
        {
            auto const seq = Sequence{{
                {.element = Note{1}, .weight = 1.f},
                {.element = Note{1}, .weight = 1.f},
                {.element = Note{1}, .weight = 2.f},
            }};

            auto const infos =
                midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 120);

            auto const expected = std::vector<midi::SampleRange>{
                {.begin = 0, .end = 22'050},
                {.begin = 22'050, .end = 44'100},
                {.begin = 44'100, .end = 88'200},
            };

            REQUIRE(infos == expected);
        }

        {
            auto const seq = Sequence{{
                {.element = Note{}, .weight = 1.f},
                {.element = Sequence{{
                     // 7.65 total weight
                     {.element = Note{}, .weight = 0.5f},
                     {.element = Note{}, .weight = 1.4f},
                     {.element = Note{}, .weight = 1.f},
                     {.element = Rest{}, .weight = 3.f},
                     {.element = Note{}, .weight = 1.75f},
                 }},
                 .weight = 3.f},
                {.element = Note{}, .weight = 2.f},
            }};

            auto const infos =
                midi::flatten_and_translate_to_sample_infos(Measure{seq}, 44100, 120);

            auto const expected = std::vector<midi::SampleRange>{
                {.begin = 0, .end = 14'700},
                {.begin = 14'700, .end = 17'582},
                {.begin = 17'582, .end = 25'653},
                {.begin = 25'653, .end = 31'418},
                // Rest [31'418, 48'712]
                {.begin = 48'712, .end = 58'800},
                {.begin = 58'800, .end = 88'200},
            };

            REQUIRE(infos == expected);
        }
    }
}

TEST_CASE("translate_to_midi_timeline", "[midi]")
{
    auto const tuning = Tuning{
        {0, 50, 100, 150, 200},
        1200,
        "description",
    };

    auto const seq = Sequence{{
        {.element = Note{.pitch = 0, .velocity = 0.44}, .weight = 1.f},
        {.element = Sequence{{
             {
                 .element =
                     Note{.pitch = 4, .velocity = 0.43, .delay = 0.f, .gate = 1.f},
                 .weight = 0.5f,
             },
             {
                 .element =
                     Note{.pitch = 40, .velocity = 1.f, .delay = 0.4f, .gate = 1.f},
                 .weight = 1.4f,
             },
             {
                 .element =
                     Note{.pitch = 2, .velocity = 0.75f, .delay = 0.f, .gate = 0.33f},
                 .weight = 1.f,
             },
             {
                 .element = Rest{},
                 .weight = 3.f,
             },
             {
                 .element =
                     Note{.pitch = -14, .velocity = 0.5f, .delay = 0.f, .gate = 1.f},
                 .weight = 1.75f,
             },
         }},
         .weight = 3.f},
        {.element = Note{.pitch = -24, .delay = 0.3}, .weight = 2.f},
    }};

    auto const timeline =
        midi::translate_to_midi_timeline(Measure{seq}, 44100, 120, tuning, 440.f, 1);

    auto const expected = midi::EventTimeline{
        {midi::PitchBend{8'192}, 0},
        {midi::NoteOn{.note = 69, .velocity = 55}, 0},
        {midi::NoteOff{.note = 69}, 14'700},
        {midi::PitchBend{8'192}, 14'700},
        {midi::NoteOn{.note = 71, .velocity = 54}, 14'700},
        {midi::NoteOff{.note = 71}, 17'582},
        {midi::PitchBend{8'192}, 20'810},
        {midi::NoteOn{.note = 127, .velocity = 127}, 20'810},
        {midi::NoteOff{.note = 127}, 25'653},
        {midi::PitchBend{8'192}, 25'653},
        {midi::NoteOn{.note = 70, .velocity = 95}, 25'653},
        {midi::NoteOff{.note = 70}, 27'555},
        {midi::PitchBend{12'288}, 48'712},
        {midi::NoteOn{.note = 33, .velocity = 63}, 48'712},
        {midi::NoteOff{.note = 33}, 58'800},
        {midi::PitchBend{12'288}, 67'620},
        {midi::NoteOn{.note = 9, .velocity = 88}, 67'620},
        {midi::NoteOff{.note = 9}, 88'200},
    };

    REQUIRE(timeline == expected);
}