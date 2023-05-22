#include "catch.hpp"

#include <iostream>

#include <sequence/midi.hpp>

using namespace sequence;

TEST_CASE("create_midi_note", "[midi]")
{
    SECTION("throw if tuning is empty")
    {
        REQUIRE_THROWS_AS(midi::create_midi_note(0, {}, 0.f), std::invalid_argument);
    }

    SECTION("12edo (12 equal divisions of the octave)")
    {
        auto const tuning = Tuning{
            {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
            1200,
        };
        auto const tuning_base = 60.f;

        for (auto i = 0; i < (int)(tuning.intervals.size() + 1); ++i)
        {
            auto const note = midi::create_midi_note(i, tuning, tuning_base);
            REQUIRE((int)note.note == 60 + i);
            REQUIRE(note.pitch_bend == 8192);
        }
    }

    SECTION("grail (microtonal)")
    {
        auto const tuning =
            Tuning{{0.f, 86.869027f, 195.623009f, 304.376991f, 391.246018f, 504.376991f,
                    578.080960f, 695.623009f, 795.623009f, 895.623009f, 1013.165056f,
                    1086.869026f},
                   1200.f};

        SECTION("tuning_base = 60")
        {
            auto const tuning_base = 60.f;

            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 15308);
            }
            {
                auto const note = midi::create_midi_note(2, tuning, tuning_base);
                REQUIRE(note.note == 61);
                REQUIRE(note.pitch_bend == 16025);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base);
                REQUIRE(note.note == 72);
                REQUIRE(note.pitch_bend == 8192);
            }
        }
        SECTION("tuning_base = 10.5")
        {
            auto const tuning_base = 10.5f;

            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base);
                REQUIRE(note.note == 10);
                REQUIRE(note.pitch_bend == 12288);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base);
                REQUIRE(note.note == 11);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(2, tuning, tuning_base);
                REQUIRE(note.note == 12);
                REQUIRE(note.pitch_bend == 11929);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base);
                REQUIRE(note.note == 22);
                REQUIRE(note.pitch_bend == 12288);
            }
        }
        SECTION("Interval over tuning length")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(13, tuning, tuning_base);
                REQUIRE(note.note == 23);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(14, tuning, tuning_base);
                REQUIRE(note.note == 24);
                REQUIRE(note.pitch_bend == 11929);
            }
            {
                auto const note = midi::create_midi_note(15, tuning, tuning_base);
                REQUIRE(note.note == 25);
                REQUIRE(note.pitch_bend == 12646);
            }
        }

        SECTION("Negative Interval")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(-1, tuning, tuning_base);
                REQUIRE(note.note == 9);
                REQUIRE(note.pitch_bend == 11212);
            }
            {
                auto const note = midi::create_midi_note(-2, tuning, tuning_base);
                REQUIRE(note.note == 8);
                REQUIRE(note.pitch_bend == 13366);
            }
            {
                auto const note = midi::create_midi_note(-3, tuning, tuning_base);
                REQUIRE(note.note == 7);
                REQUIRE(note.pitch_bend == 11929);
            }
        }

        SECTION("Negative Interval over tuning length")
        {
            auto const tuning_base = 40.75f;
            {
                auto const note = midi::create_midi_note(-13, tuning, tuning_base);
                REQUIRE(note.note == 27);
                REQUIRE(note.pitch_bend == 13260);
            }
            {
                auto const note = midi::create_midi_note(-17, tuning, tuning_base);
                REQUIRE(note.note == 23);
                REQUIRE(note.pitch_bend == 13977);
            }
            {
                auto const note = midi::create_midi_note(-25, tuning, tuning_base);
                REQUIRE(note.note == 15);
                REQUIRE(note.pitch_bend == 13260);
            }
        }

        SECTION("Zero is the lowest note")
        {
            auto const tuning_base = 10.5f;
            {
                auto const note = midi::create_midi_note(-11, tuning, tuning_base);
                REQUIRE(note.note == 0);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(-100, tuning, tuning_base);
                REQUIRE(note.note == 0);
                REQUIRE(note.pitch_bend == 8192);
            }
        }

        SECTION("127 is the highest note")
        {
            auto const tuning_base = 100.f;
            {
                auto const note = midi::create_midi_note(28, tuning, tuning_base);
                REQUIRE(note.note == 127);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(156, tuning, tuning_base);
                REQUIRE(note.note == 127);
                REQUIRE(note.pitch_bend == 8192);
            }
        }
    }
    SECTION("Single Note Tuning")
    {
        auto const tuning = Tuning{{0.f}, 1200.f};
        auto const tuning_base = 63.f;
        {
            auto const note = midi::create_midi_note(0, tuning, tuning_base);
            REQUIRE(note.note == 63);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(1, tuning, tuning_base);
            REQUIRE(note.note == 75);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(-1, tuning, tuning_base);
            REQUIRE(note.note == 51);
            REQUIRE(note.pitch_bend == 8192);
        }
        {
            auto const note = midi::create_midi_note(-2, tuning, tuning_base);
            REQUIRE(note.note == 39);
            REQUIRE(note.pitch_bend == 8192);
        }
    }
    SECTION("anharmonic tunings (e.g. non-octave based)")
    {
        // 13 Equal Divisions of the Tritave
        auto const tuning =
            Tuning{{0.f, 146.3f, 292.6f, 438.9f, 585.2f, 731.5f, 877.8f, 1024.1f,
                    1170.4f, 1316.7f, 1463.0f, 1609.3f, 1755.7f},
                   1902.f};
        auto const tuning_base = 60.f;
        SECTION("Positive Intervals")
        {
            {
                auto const note = midi::create_midi_note(0, tuning, tuning_base);
                REQUIRE(note.note == 60);
                REQUIRE(note.pitch_bend == 8192);
            }
            {
                auto const note = midi::create_midi_note(1, tuning, tuning_base);
                REQUIRE(note.note == 61);
                REQUIRE(note.pitch_bend == 11984);
            }
            {
                auto const note = midi::create_midi_note(12, tuning, tuning_base);
                REQUIRE(note.note == 77);
                REQUIRE(note.pitch_bend == 12754);
            }
            {
                auto const note = midi::create_midi_note(13, tuning, tuning_base);
                REQUIRE(note.note == 79);
                REQUIRE(note.pitch_bend == 8355);
            }
            {
                auto const note = midi::create_midi_note(14, tuning, tuning_base);
                REQUIRE(note.note == 80);
                REQUIRE(note.pitch_bend == 12148);
            }
        }
        SECTION("Negative Intervals")
        {
            {
                auto const note = midi::create_midi_note(-1, tuning, tuning_base);
                REQUIRE(note.note == 58);
                REQUIRE(note.pitch_bend == 12591);
            }
            {
                auto const note = midi::create_midi_note(-12, tuning, tuning_base);
                REQUIRE(note.note == 42);
                REQUIRE(note.pitch_bend == 11821);
            }
            {
                auto const note = midi::create_midi_note(-13, tuning, tuning_base);
                REQUIRE(note.note == 40);
                REQUIRE(note.pitch_bend == 16220);
            }
            {
                auto const note = midi::create_midi_note(-14, tuning, tuning_base);
                REQUIRE(note.note == 39);
                REQUIRE(note.pitch_bend == 12427);
            }
        }
    }
}