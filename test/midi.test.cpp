#include "catch.hpp"

#include <iostream>

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

        for (auto i = 0; i < (int)(tuning.intervals.size() + 1); ++i)
        {
            auto const note = midi::create_midi_note(i, tuning, tuning_base, 1);
            REQUIRE((int)note.note == 60 + i);
            REQUIRE(note.pitch_bend == 8192);
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

            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base, 1);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base, 1);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 15308);
            }
            {
                auto const note = midi::create_midi_note(2, tuning, tuning_base, 1);
                REQUIRE(note.note == 61);
                REQUIRE(note.pitch_bend == 16025);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base, 1);
                REQUIRE(note.note == 72);
                REQUIRE(note.pitch_bend == 8192);
            }
        }
        SECTION("tuning_base = 10.5")
        {
            auto const tuning_base = 10.5f;

            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base, 1);
                REQUIRE(note.note == 10);
                REQUIRE(note.pitch_bend == 12288);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base, 1);
                REQUIRE(note.note == 11);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(2, tuning, tuning_base, 1);
                REQUIRE(note.note == 12);
                REQUIRE(note.pitch_bend == 11929);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base, 1);
                REQUIRE(note.note == 22);
                REQUIRE(note.pitch_bend == 12288);
            }
        }
        SECTION("Interval over tuning length")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(13, tuning, tuning_base, 1);
                REQUIRE(note.note == 23);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(14, tuning, tuning_base, 1);
                REQUIRE(note.note == 24);
                REQUIRE(note.pitch_bend == 11929);
            }
            {
                auto const note = midi::create_midi_note(15, tuning, tuning_base, 1);
                REQUIRE(note.note == 25);
                REQUIRE(note.pitch_bend == 12646);
            }
        }

        SECTION("Negative Interval")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(-1, tuning, tuning_base, 1);
                REQUIRE((int)note.note == 9);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(-2, tuning, tuning_base, 1);
                REQUIRE(note.note == 8);
                REQUIRE(note.pitch_bend == 13366);
            }
            {
                auto const note = midi::create_midi_note(-3, tuning, tuning_base, 1);
                REQUIRE(note.note == 7);
                REQUIRE(note.pitch_bend == 11929);
            }
        }

        SECTION("Negative Interval over tuning length")
        {
            auto const tuning_base = 40.75f;
            {
                auto const note = midi::create_midi_note(-13, tuning, tuning_base, 1);
                REQUIRE(note.note == 27);
                REQUIRE(note.pitch_bend == 13260);
            }
            {
                auto const note = midi::create_midi_note(-17, tuning, tuning_base, 1);
                REQUIRE(note.note == 23);
                REQUIRE(note.pitch_bend == 13977);
            }
            {
                auto const note = midi::create_midi_note(-25, tuning, tuning_base, 1);
                REQUIRE(note.note == 15);
                REQUIRE(note.pitch_bend == 13260);
            }
        }

        SECTION("Zero is the lowest note")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(-11, tuning, tuning_base, 1);
                REQUIRE(note.note == 0);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(-100, tuning, tuning_base, 1);
                REQUIRE(note.note == 0);
                REQUIRE(note.pitch_bend == 8192);
            }
        }

        SECTION("127 is the highest note")
        {
            auto const tuning_base = 100.f;
            {
                auto const note = midi::create_midi_note(28, tuning, tuning_base, 1);
                REQUIRE(note.note == 127);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(156, tuning, tuning_base, 1);
                REQUIRE(note.note == 127);
                REQUIRE(note.pitch_bend == 8192);
            }
        }
    }
    SECTION("Single Note Tuning")
    {
        auto const tuning = Tuning{{0.f}, 1200.f, "description"};
        auto const tuning_base = 63.f;
        {
            auto const note = midi::create_midi_note(0, tuning, tuning_base, 1);
            REQUIRE(note.note == 63);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(1, tuning, tuning_base, 1);
            REQUIRE(note.note == 75);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(-1, tuning, tuning_base, 1);
            REQUIRE(note.note == 51);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(-2, tuning, tuning_base, 1);
            REQUIRE(note.note == 39);
            REQUIRE(note.pitch_bend == 8192);
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
        SECTION("Positive Intervals")
        {
            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base, 1);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base, 1);
                REQUIRE(note.note == 61);
                REQUIRE(note.pitch_bend == 11984);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base, 1);
                REQUIRE(note.note == 77);
                REQUIRE(note.pitch_bend == 12754);
            }
            {
                auto const note = midi::create_midi_note(13, tuning, tuning_base, 1);
                REQUIRE(note.note == 79);
                REQUIRE(note.pitch_bend == 8355);
            }
            {
                auto const note = midi::create_midi_note(14, tuning, tuning_base, 1);
                REQUIRE(note.note == 80);
                REQUIRE(note.pitch_bend == 12148);
            }
        }
        SECTION("Negative Intervals")
        {
            {
                auto const note = midi::create_midi_note(-1, tuning, tuning_base, 1);
                REQUIRE(note.note == 58);
                REQUIRE(note.pitch_bend == 12591);
            }
            {
                auto const note = midi::create_midi_note(-12, tuning, tuning_base, 1);
                REQUIRE(note.note == 42);
                REQUIRE(note.pitch_bend == 11821);
            }
            {
                auto const note = midi::create_midi_note(-13, tuning, tuning_base, 1);
                REQUIRE(note.note == 40);
                REQUIRE(note.pitch_bend == 16220);
            }
            {
                auto const note = midi::create_midi_note(-14, tuning, tuning_base, 1);
                REQUIRE(note.note == 39);
                REQUIRE(note.pitch_bend == 12427);
            }
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

    auto const measure = [] {
        auto m = create_measure({4, 4}, 2);
        for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
        {
            // if even create a subsequence else create a note
            if (i % 2 == 0)
            {
                std::get<Sequence>(m.cell).cells[i] = Sequence{{
                    Note{i, 0.75f, 0.f, 1.f},
                    Rest{},
                    Note{i + 2, 0.75f, 0.f, 1.f},
                }};
            }
            else
            {
                std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 0.f, 1.f};
            }
        }
        return m;
    }();

    auto const notes = midi::flatten_and_translate_to_midi_notes(measure, tuning);

    REQUIRE(notes.size() == 12);

    for (auto const &note : notes)
    {
        REQUIRE(note.pitch_bend == 8192);
    }

    REQUIRE(notes[0].note == 69);
    REQUIRE(notes[1].note == 71);
    REQUIRE(notes[2].note == 70);
    REQUIRE(notes[3].note == 71);
    REQUIRE(notes[4].note == 73);
    REQUIRE(notes[5].note == 72);
    REQUIRE(notes[6].note == 73);
    REQUIRE(notes[7].note == 75);
    REQUIRE(notes[8].note == 74);
    REQUIRE(notes[9].note == 75);
    REQUIRE(notes[10].note == 77);
    REQUIRE(notes[11].note == 76);
}

TEST_CASE("flatten_notes", "[midi]")
{
    auto const seq = Sequence{{
        Rest{},
        Note{0, 0.5f, 0.f, 1.f},
        Sequence{{
            Note{1, 0.5f, 0.f, 1.f},
            Rest{},
            Note{2, 0.5f, 0.f, 1.f},
        }},
        Rest{},
        Note{3, 0.5f, 0.f, 1.f},
    }};

    // FIXME This recurses into infinity
    auto const flat = midi::flatten_notes(seq);

    REQUIRE(flat.size() == 4);
    REQUIRE(flat[0] == Note{0, 0.5f, 0.f, 1.f});
    REQUIRE(flat[1] == Note{1, 0.5f, 0.f, 1.f});
    REQUIRE(flat[2] == Note{2, 0.5f, 0.f, 1.f});
    REQUIRE(flat[3] == Note{3, 0.5f, 0.f, 1.f});
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
        auto const measure = [] {
            auto m = create_measure({4, 4}, 2);
            for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
            {
                // if even create a subsequence else create a note
                if (i % 2 == 0)
                {
                    std::get<Sequence>(m.cell).cells[i] = Sequence{{
                        Note{i, 0.75f, 0.f, 1.f},
                        Rest{},
                        Note{i + 2, 0.75f, 0.f, 1.f},
                    }};
                }
                else
                {
                    std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 0.f, 1.f};
                }
            }
            return m;
        }();

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(measure, 44100, 120);

        REQUIRE(infos.size() == 12);

        REQUIRE(infos[0].begin == 0);
        REQUIRE(infos[0].end == 3'675);

        REQUIRE(infos[1].begin == 7'350);
        REQUIRE(infos[1].end == 11'025);

        REQUIRE(infos[2].begin == 11'025);
        REQUIRE(infos[2].end == 22'050);

        REQUIRE(infos[3].begin == 22'050);
        REQUIRE(infos[3].end == 25'725);

        REQUIRE(infos[4].begin == 29'400);
        REQUIRE(infos[4].end == 33'075);

        REQUIRE(infos[5].begin == 33'075);
        REQUIRE(infos[5].end == 44'100);
    }

    SECTION("With gate")
    {
        auto const measure = [] {
            auto m = create_measure({4, 4}, 2);
            for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
            {
                // if even create a subsequence else create a note
                if (i % 2 == 0)
                {
                    std::get<Sequence>(m.cell).cells[i] = Sequence{{
                        Note{i, 0.75f, 0.f, 0.5f},
                        Rest{},
                        Note{i + 2, 0.75f, 0.f, 0.25f},
                    }};
                }
                else
                {
                    std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 0.f, 0.f};
                }
            }
            return m;
        }();

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(measure, 44100, 120);

        REQUIRE(infos[0].begin == 0);
        REQUIRE(infos[0].end == 1'837);

        REQUIRE(infos[1].begin == 7'350);
        REQUIRE(infos[1].end == 8'268);

        REQUIRE(infos[2].begin == 11'025);
        REQUIRE(infos[2].end == 11'025); // zero gate is allowed but not recommended.

        REQUIRE(infos[3].begin == 22'050);
        REQUIRE(infos[3].end == 23'887);

        REQUIRE(infos[4].begin == 29'400);
        REQUIRE(infos[4].end == 30'318);

        REQUIRE(infos[5].begin == 33'075);
        REQUIRE(infos[5].end == 33'075);
    }

    SECTION("With delay")
    {
        auto const measure = [] {
            auto m = create_measure({4, 4}, 2);
            for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
            {
                // if even create a subsequence else create a note
                if (i % 2 == 0)
                {
                    std::get<Sequence>(m.cell).cells[i] = Sequence{{
                        Note{i, 0.75f, 0.5f, 1.f},
                        Rest{},
                        Note{i + 2, 0.75f, 0.25f, 1.f},
                    }};
                }
                else
                {
                    std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 1.f, 1.f};
                }
            }
            return m;
        }();

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(measure, 44100, 120);

        REQUIRE(infos[0].begin == 1'837);
        REQUIRE(infos[0].end == 3'675);

        REQUIRE(infos[1].begin == 8'268);
        REQUIRE(infos[1].end == 11'025);

        REQUIRE(infos[2].begin == 22'050);
        REQUIRE(infos[2].begin == 22'050);

        REQUIRE(infos[3].begin == 23'887);
        REQUIRE(infos[3].end == 25'725);

        REQUIRE(infos[4].begin == 30'318);
        REQUIRE(infos[4].end == 33'075);

        REQUIRE(infos[5].begin == 44'100);
        REQUIRE(infos[5].end == 44'100);
    }

    SECTION("With delay and gate")
    {
        auto const measure = [] {
            auto m = create_measure({4, 4}, 2);
            for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
            {
                // if even create a subsequence else create a note
                if (i % 2 == 0)
                {
                    std::get<Sequence>(m.cell).cells[i] = Sequence{{
                        Note{i, 0.75f, 0.5f, 0.8f},
                        Rest{},
                        Note{i + 2, 0.75f, 0.25f, 0.1f},
                    }};
                }
                else
                {
                    std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 1.f, 0.1f};
                }
            }
            return m;
        }();

        auto const infos =
            midi::flatten_and_translate_to_sample_infos(measure, 44100, 120);

        REQUIRE(infos[0].begin == 1'837);
        REQUIRE(infos[0].end == 3'307);

        REQUIRE(infos[1].begin == 8'268);
        REQUIRE(infos[1].end == 8'543);

        REQUIRE(infos[2].begin == 22'050);
        REQUIRE(infos[2].end == 22'050);

        REQUIRE(infos[3].begin == 23'887);
        REQUIRE(infos[3].end == 25'357);

        REQUIRE(infos[4].begin == 30'318);
        REQUIRE(infos[4].end == 30'593);

        REQUIRE(infos[5].begin == 44'100);
        REQUIRE(infos[5].end == 44'100);
    }
}

TEST_CASE("translate_to_midi_timeline", "[midi]")
{
    auto const measure1 = [] {
        auto m = create_measure({4, 4}, 2);
        for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
        {
            // if even create a subsequence else create a note
            if (i % 2 == 0)
            {
                std::get<Sequence>(m.cell).cells[i] = Sequence{{
                    Note{i, 0.75f, 0.5f, 0.8f},
                    Rest{},
                    Note{i + 2, 0.75f, 0.25f, 0.1f},
                }};
            }
            else
            {
                std::get<Sequence>(m.cell).cells[i] = Note{i, 0.75f, 1.f, 0.1f};
            }
        }
        return m;
    }();

    auto const measure2 = [] {
        auto m = create_measure({3, 4}, 3);
        for (auto i = 0; i < (int)std::get<Sequence>(m.cell).cells.size(); ++i)
        {
            std::get<Sequence>(m.cell).cells[i] = Sequence{{
                Note{i, 0.75f, 0.5f, 0.8f},
                Rest{},
                Note{i + 2, 0.75f, 0.25f, 0.1f},
            }};
        }
        return m;
    }();

    auto const tuning = Tuning{
        {0, 50, 100, 150, 200},
        1200,
        "description",
    };

    auto const timeline =
        midi::translate_to_midi_timeline(measure1, 44100, 120, tuning, 440.f, 1);

    // test::helper::print_midi_event_timeline(timeline);

    auto const timeline2 =
        midi::translate_to_midi_timeline(measure2, 44100, 120, tuning, 440.f, 1);

    // test::helper::print_midi_event_timeline(timeline2);
}