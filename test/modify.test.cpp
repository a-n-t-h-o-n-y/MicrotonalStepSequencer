#include "catch.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>

#include "helper.hpp"
#include <sequence/generate.hpp>
#include <sequence/modify.hpp>
#include <sequence/sequence.hpp>

using namespace sequence;
using namespace sequence::test::helper;

TEST_CASE("randomize_pitch", "[modify]")
{
    auto const min = 10;
    auto const max = 20;

    SECTION("All Note")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 0.8f, 0.1f, 0.4f},
            Note{2, 0.6f, 0.2f, 0.3f},
            Note{3, 0.4f, 0.3f, 0.2f},
            Note{4, 0.2f, 0.4f, 0.1f},
            Note{5, 0.0f, 0.5f, 0.0f},
        }};

        // Randomize the intervals in the sequence
        auto const randomized_seq = modify::randomize_pitch(seq, {0, {1}}, min, max);

        // Check that the randomized sequence has the same length
        REQUIRE(std::holds_alternative<Sequence>(randomized_seq));
        REQUIRE(std::get<Sequence>(randomized_seq).cells.size() == seq.cells.size());

        // Check that each pitch is within the expected range
        for (auto const &cell : std::get<Sequence>(randomized_seq).cells)
        {
            auto const pitch = std::get<Note>(cell).pitch;
            REQUIRE(pitch >= min);
            REQUIRE(pitch <= max);
        }
    }

    SECTION("All Rests")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Rest{},
            Rest{},
            Rest{},
            Rest{},
            Rest{},
            Rest{},
        }};

        // Randomize the intervals in the sequence
        auto const randomized_seq = modify::randomize_pitch(seq, {0, {1}}, min, max);

        // Check that the randomized sequence has the same length
        REQUIRE(std::holds_alternative<Sequence>(randomized_seq));
        REQUIRE(std::get<Sequence>(randomized_seq).cells.size() == seq.cells.size());

        // Check that each pitch is within the expected range
        for (auto const &cell : std::get<Sequence>(randomized_seq).cells)
        {
            REQUIRE(holds<Rest>(cell));
        }
    }

    SECTION("Throws std::invalid_argument")
    {
        // Create a test sequence
        auto const seq =
            Sequence{{Note{0, 1.0f, 0.0f, 0.5f}, Rest{}, Note{2, 0.6f, 0.2f, 0.3f}}};

        // min > max
        REQUIRE_THROWS_AS(modify::randomize_pitch(seq, {0, {1}}, 10, -10),
                          std::invalid_argument);
    }

    SECTION("SubSequences")
    {
        // Create a test sequence
        auto const seq = Sequence{
            {
                Note{0, 1.0f, 0.0f, 0.5f},
                Sequence{{
                    Note{1, 0.8f, 0.1f, 0.4f},
                    Note{2, 0.6f, 0.2f, 0.3f},
                }},
                Note{3, 0.4f, 0.3f, 0.2f},
                Sequence{{
                    Note{4, 0.2f, 0.4f, 0.1f},
                    Note{5, 0.0f, 0.5f, 0.0f},
                }},
            },
        };

        // Randomize the intervals in the sequence
        auto const randomized_seq = modify::randomize_pitch(seq, {0, {1}}, min, max);

        // Check that the randomized sequence has the same length
        REQUIRE(std::holds_alternative<Sequence>(randomized_seq));
        REQUIRE(std::get<Sequence>(randomized_seq).cells.size() == seq.cells.size());

        // Check that each pitch is within the expected range
        test::helper::check_sequence(randomized_seq, [&](Note const &note) {
            REQUIRE(note.pitch >= min);
            REQUIRE(note.pitch <= max);
        });
    }
}

TEST_CASE("randomize_velocity", "[modify]")
{
    auto const min = 0.2f;
    auto const max = 0.7f;

    SECTION("All Note")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 0.8f, 0.1f, 0.4f},
            Note{2, 0.6f, 0.2f, 0.3f},
            Note{3, 0.4f, 0.3f, 0.2f},
            Note{4, 0.2f, 0.4f, 0.1f},
            Note{5, 0.0f, 0.5f, 0.0f},
        }};

        // Randomize the velocity in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_velocity(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each velocity is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            auto const velocity = std::get<Note>(cell).velocity;
            REQUIRE(velocity >= min);
            REQUIRE(velocity <= max);
        }
    }

    SECTION("All Rests")
    {
        // Create a test sequence
        auto const seq = Sequence{{Rest{}, Rest{}, Rest{}, Rest{}, Rest{}, Rest{}}};

        // Randomize the velocity in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_velocity(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each velocity is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            REQUIRE(holds<Rest>(cell));
        }
    }

    SECTION("Throws std::invalid_argument")
    {
        // Create a test sequence
        auto const seq =
            Sequence{{Note{0, 1.0f, 0.0f, 0.5f}, Rest{}, Note{2, 0.6f, 0.2f, 0.3f}}};

        // min > max
        REQUIRE_THROWS_AS(modify::randomize_velocity(seq, {0, {1}}, 0.7f, 0.2f),
                          std::invalid_argument);

        // min < 0
        REQUIRE_THROWS_AS(modify::randomize_velocity(seq, {0, {1}}, -0.5f, 0.4f),
                          std::invalid_argument);

        // min > 1
        REQUIRE_THROWS_AS(modify::randomize_velocity(seq, {0, {1}}, 1.5f, 0.4f),
                          std::invalid_argument);

        // max > 1
        REQUIRE_THROWS_AS(modify::randomize_velocity(seq, {0, {1}}, 0.2f, 1.5f),
                          std::invalid_argument);

        // max < 0
        REQUIRE_THROWS_AS(modify::randomize_velocity(seq, {0, {1}}, 0.2f, -0.5f),
                          std::invalid_argument);
    }
    SECTION("SubSequences")
    {
        // Create a test sequence
        auto const seq = Sequence{
            {
                Note{0, 1.0f, 0.0f, 0.5f},
                Sequence{{
                    Note{1, 0.8f, 0.1f, 0.4f},
                    Note{2, 0.6f, 0.2f, 0.3f},
                }},
                Note{3, 0.4f, 0.3f, 0.2f},
                Sequence{{
                    Note{4, 0.2f, 0.4f, 0.1f},
                    Note{5, 0.0f, 0.5f, 0.0f},
                }},
            },
        };

        // Randomize the velocity in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_velocity(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each velocity is within the expected range
        test::helper::check_sequence(randomized_seq, [&](Note const &note) {
            REQUIRE(note.velocity >= min);
            REQUIRE(note.velocity <= max);
        });
    }
}

TEST_CASE("randomize_delay", "[modify]")
{
    auto const min = 0.2f;
    auto const max = 0.7f;

    SECTION("All Note")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 0.8f, 0.1f, 0.4f},
            Note{2, 0.6f, 0.2f, 0.3f},
            Note{3, 0.4f, 0.3f, 0.2f},
            Note{4, 0.2f, 0.4f, 0.1f},
            Note{5, 0.0f, 0.5f, 0.0f},
        }};

        // Randomize the delay in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_delay(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each delay is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            auto const delay = std::get<Note>(cell).delay;
            REQUIRE(delay >= min);
            REQUIRE(delay <= max);
        }
    }

    SECTION("All Rests")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Rest{},
            Rest{},
            Rest{},
            Rest{},
            Rest{},
            Rest{},
        }};

        // Randomize the delay in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_delay(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each delay is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            REQUIRE(holds<Rest>(cell));
        }
    }

    SECTION("Throws std::invalid_argument")
    {
        // Create a test sequence
        auto const seq =
            Sequence{{Note{0, 1.0f, 0.0f, 0.5f}, Rest{}, Note{2, 0.6f, 0.2f, 0.3f}}};

        // min > max
        REQUIRE_THROWS_AS(modify::randomize_delay(seq, {0, {1}}, 0.7f, 0.2f),
                          std::invalid_argument);

        // min < 0
        REQUIRE_THROWS_AS(modify::randomize_delay(seq, {0, {1}}, -0.5f, 0.4f),
                          std::invalid_argument);

        // min > 1
        REQUIRE_THROWS_AS(modify::randomize_delay(seq, {0, {1}}, 1.5f, 0.4f),
                          std::invalid_argument);

        // max > 1
        REQUIRE_THROWS_AS(modify::randomize_delay(seq, {0, {1}}, 0.2f, 1.5f),
                          std::invalid_argument);

        // max < 0
        REQUIRE_THROWS_AS(modify::randomize_delay(seq, {0, {1}}, 0.2f, -0.5f),
                          std::invalid_argument);
    }

    SECTION("SubSequences")
    {
        // Create a test sequence
        auto const seq = Sequence{
            {
                Note{0, 1.0f, 0.0f, 0.5f},
                Sequence{{
                    Note{1, 0.8f, 0.1f, 0.4f},
                    Note{2, 0.6f, 0.2f, 0.3f},
                }},
                Note{3, 0.4f, 0.3f, 0.2f},
                Sequence{{
                    Note{4, 0.2f, 0.4f, 0.1f},
                    Note{5, 0.0f, 0.5f, 0.0f},
                }},
            },
        };

        // Randomize the delay in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_delay(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each delay is within the expected range
        test::helper::check_sequence(randomized_seq, [&](Note const &note) {
            REQUIRE(note.delay >= min);
            REQUIRE(note.delay <= max);
        });
    }
}

TEST_CASE("randomize_gate", "[modify]")
{
    auto const min = 0.2f;
    auto const max = 0.7f;

    SECTION("All Note")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 0.8f, 0.1f, 0.4f},
            Note{2, 0.6f, 0.2f, 0.3f},
            Note{3, 0.4f, 0.3f, 0.2f},
            Note{4, 0.2f, 0.4f, 0.1f},
            Note{5, 0.0f, 0.5f, 0.0f},
        }};

        // Randomize the gate in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_gate(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each gate is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            auto const gate = std::get<Note>(cell).gate;
            REQUIRE(gate >= min);
            REQUIRE(gate <= max);
        }
    }

    SECTION("All Rests")
    {
        // Create a test sequence
        auto const seq = Sequence{{Rest{}, Rest{}, Rest{}, Rest{}, Rest{}, Rest{}}};

        // Randomize the gate in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_gate(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each gate is within the expected range
        for (auto const &cell : randomized_seq.cells)
        {
            REQUIRE(holds<Rest>(cell));
        }
    }

    SECTION("Throws std::invalid_argument")
    {
        // Create a test sequence
        auto const seq =
            Sequence{{Note{0, 1.0f, 0.0f, 0.5f}, Rest{}, Note{2, 0.6f, 0.2f, 0.3f}}};

        // min > max
        REQUIRE_THROWS_AS(modify::randomize_gate(seq, {0, {1}}, 0.7f, 0.2f),
                          std::invalid_argument);

        // min < 0
        REQUIRE_THROWS_AS(modify::randomize_gate(seq, {0, {1}}, -0.5f, 0.4f),
                          std::invalid_argument);

        // min > 1
        REQUIRE_THROWS_AS(modify::randomize_gate(seq, {0, {1}}, 1.5f, 0.4f),
                          std::invalid_argument);

        // max > 1
        REQUIRE_THROWS_AS(modify::randomize_gate(seq, {0, {1}}, 0.2f, 1.5f),
                          std::invalid_argument);

        // max < 0
        REQUIRE_THROWS_AS(modify::randomize_gate(seq, {0, {1}}, 0.2f, -0.5f),
                          std::invalid_argument);
    }

    SECTION("SubSequences")
    {
        // Create a test sequence
        auto const seq = Sequence{
            {
                Note{0, 1.0f, 0.0f, 0.5f},
                Sequence{{
                    Note{1, 0.8f, 0.1f, 0.4f},
                    Note{2, 0.6f, 0.2f, 0.3f},
                }},
                Note{3, 0.4f, 0.3f, 0.2f},
                Sequence{{
                    Note{4, 0.2f, 0.4f, 0.1f},
                    Note{5, 0.0f, 0.5f, 0.0f},
                }},
            },
        };

        // Randomize the gate in the sequence
        auto const randomized_seq =
            std::get<Sequence>(modify::randomize_gate(seq, {0, {1}}, min, max));

        // Check that the randomized sequence has the same length
        REQUIRE(randomized_seq.cells.size() == seq.cells.size());

        // Check that each gate is within the expected range
        test::helper::check_sequence(randomized_seq, [&](Note const &note) {
            REQUIRE(note.gate >= min);
            REQUIRE(note.gate <= max);
        });
    }
}

TEST_CASE("shift_pitch", "[modify]")
{
    SECTION("Zero Shift")
    {
        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{2, 0.6f, 0.2f, 0.3f},
        }};

        // Shift the pitch in the sequence
        auto const shifted_seq =
            std::get<Sequence>(modify::shift_pitch(seq, {0, {1}}, 0));

        // Check that the shifted sequence has the same length
        REQUIRE(shifted_seq.cells.size() == seq.cells.size());

        // Check that the shifted sequence has the same pitch
        for (std::size_t i = 0; i < shifted_seq.cells.size(); ++i)
        {
            auto const &original_cell = seq.cells[i];
            auto const &shifted_cell = shifted_seq.cells[i];
            if (holds<Note>(shifted_cell))
            {
                auto const pitch = std::get<Note>(shifted_cell).pitch;
                REQUIRE(pitch == std::get<Note>(original_cell).pitch);
            }
        }
    }

    SECTION("Shift Positive")
    {
        auto const shift = 2;

        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{2, 0.6f, 0.2f, 0.3f},
        }};

        // Shift the pitch in the sequence
        auto const shifted_seq =
            std::get<Sequence>(modify::shift_pitch(seq, {0, {1}}, shift));

        // Check that the shifted sequence has the same length
        REQUIRE(shifted_seq.cells.size() == seq.cells.size());

        // Check that the shifted sequence has the same pitch
        for (std::size_t i = 0; i < shifted_seq.cells.size(); ++i)
        {
            auto const &original_cell = seq.cells[i];
            auto const &shifted_cell = shifted_seq.cells[i];
            if (holds<Note>(shifted_cell))
            {
                auto const pitch = std::get<Note>(shifted_cell).pitch;
                REQUIRE(pitch == std::get<Note>(original_cell).pitch + shift);
            }
        }
    }

    SECTION("Shift Negative")
    {
        auto const shift = -2;

        // Create a test sequence
        auto const seq = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{2, 0.6f, 0.2f, 0.3f},
        }};

        // Shift the pitch in the sequence
        auto const shifted_seq =
            std::get<Sequence>(modify::shift_pitch(seq, {0, {1}}, shift));

        // Check that the shifted sequence has the same length
        REQUIRE(shifted_seq.cells.size() == seq.cells.size());

        // Check that the shifted sequence has the same pitch
        for (std::size_t i = 0; i < shifted_seq.cells.size(); ++i)
        {
            auto const &original_cell = seq.cells[i];
            auto const &shifted_cell = shifted_seq.cells[i];
            if (holds<Note>(shifted_cell))
            {
                auto const pitch = std::get<Note>(shifted_cell).pitch;
                REQUIRE(pitch == std::get<Note>(original_cell).pitch + shift);
            }
        }
    }

    SECTION("SubSequences")
    {
        // Create a test sequence
        auto const seq = Sequence{
            {
                Note{5, 1.0f, 0.0f, 0.5f},
                Sequence{{
                    Note{5, 0.8f, 0.1f, 0.4f},
                    Note{5, 0.6f, 0.2f, 0.3f},
                }},
                Note{5, 0.4f, 0.3f, 0.2f},
                Sequence{{
                    Note{5, 0.2f, 0.4f, 0.1f},
                    Note{5, 0.0f, 0.5f, 0.0f},
                }},
            },
        };

        // Shift the pitch in the sequence
        auto const shifted_seq =
            std::get<Sequence>(modify::shift_pitch(seq, {0, {1}}, 2));

        // Check that the shifted sequence has the same length
        REQUIRE(shifted_seq.cells.size() == seq.cells.size());

        // Check that each gate is within the expected range
        test::helper::check_sequence(
            shifted_seq, [&](Note const &note) { REQUIRE(note.pitch == (5 + 2)); });
    }
}

TEST_CASE("rotate", "[modify]")
{
    // Create a test sequence
    auto const seq = Sequence{
        {
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{1, 0.6f, 0.2f, 0.3f},
            Note{2, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{3, 0.6f, 0.2f, 0.3f},
            Note{4, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{5, 0.6f, 0.2f, 0.3f},
            Note{6, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{7, 0.6f, 0.2f, 0.3f},
            Note{8, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{9, 0.6f, 0.2f, 0.3f},
            Note{10, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{11, 0.6f, 0.2f, 0.3f},
        },
    };

    SECTION("Zero Rotate")
    {
        // Rotate the sequence
        auto const rotated_seq = std::get<Sequence>(modify::rotate(seq, 0));

        // Check that the rotated sequence has the same length
        REQUIRE(rotated_seq.cells.size() == seq.cells.size());

        // Check that the rotated sequence is the same as the original
        REQUIRE(rotated_seq == seq);
    }

    SECTION("+4 Shift")
    {
        // Rotate the sequence
        auto const rotated_seq = std::get<Sequence>(modify::rotate(seq, 4));

        // Check that the rotated sequence has the same length
        REQUIRE(rotated_seq.cells.size() == seq.cells.size());

        // Check that the rotated sequence is shifted.
        auto local_rotated = seq;
        auto const shift = seq.cells.size() - (4 % seq.cells.size());
        std::rotate(local_rotated.cells.begin(), local_rotated.cells.begin() + shift,
                    local_rotated.cells.end());
        REQUIRE(rotated_seq == local_rotated);
    }

    SECTION("-5 Shift")
    {
        // Rotate the sequence
        auto const rotated_seq = std::get<Sequence>(modify::rotate(seq, -5));

        // Check that the rotated sequence has the same length
        REQUIRE(rotated_seq.cells.size() == seq.cells.size());

        // Check that the rotated sequence is shifted.
        auto local_rotated = seq;
        auto const shift = (seq.cells.size() + 5) % seq.cells.size();
        std::rotate(local_rotated.cells.begin(), local_rotated.cells.begin() + shift,
                    local_rotated.cells.end());
        REQUIRE(rotated_seq == local_rotated);
    }

    SECTION("OverShift")
    {
        // Rotate the sequence
        auto const amount = 100;
        auto const rotated_seq = std::get<Sequence>(modify::rotate(seq, amount));

        // Check that the rotated sequence has the same length
        REQUIRE(rotated_seq.cells.size() == seq.cells.size());

        // Check that the rotated sequence is shifted to the left by amount % seq.size()
        auto local_rotated = seq;
        auto const shift = seq.cells.size() - (amount % seq.cells.size());
        std::rotate(local_rotated.cells.begin(), local_rotated.cells.begin() + shift,
                    local_rotated.cells.end());
        REQUIRE(rotated_seq == local_rotated);
    }

    SECTION("Shift on Empty Sequence")
    {
        // Create an empty sequence
        auto const empty_seq = Sequence{};

        // Rotate the sequence
        auto const rotated_seq = std::get<Sequence>(modify::rotate(empty_seq, 3));

        // Check that the rotated sequence has the same length
        REQUIRE(rotated_seq.cells.size() == empty_seq.cells.size());

        // Check that the rotated sequence is the same as the original
        REQUIRE(rotated_seq == empty_seq);
    }
}

auto check_swing(Sequence const &seq, float expected) -> void
{
    using namespace utility;
    for (std::size_t i = 0; i < seq.cells.size(); ++i)
    {
        auto const is_odd = (i % 2) == 1;
        std::visit(overload{
                       [&](Note const &note) {
                           REQUIRE(note.delay == (is_odd ? expected : 0.f));
                       },
                       [](Rest) {},
                       [&](Sequence const &sub_seq) { check_swing(sub_seq, expected); },
                   },
                   seq.cells[i]);
    }
}

TEST_CASE("swing", "[modify]")
{
    // Create a test sequence
    auto const seq = Sequence{
        {
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{1, 0.6f, 0.2f, 0.3f},
            Note{2, 1.0f, 0.5f, 0.5f},
            Rest{},
            Note{3, 0.6f, 0.2f, 0.3f},
            Note{4, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{5, 0.6f, 0.2f, 0.3f},
            Note{6, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{7, 0.6f, 0.2f, 0.3f},
            Note{8, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{9, 0.6f, 0.2f, 0.3f},
            Note{10, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{11, 0.6f, 0.4f, 0.3f},
        },
    };

    SECTION("Zero Swing")
    {
        // Swing the sequence
        auto const swung_seq = std::get<Sequence>(modify::swing(seq, 0.f));

        // Check that the swung sequence has the same length
        REQUIRE(swung_seq.cells.size() == seq.cells.size());

        // Check that the swung sequence delays are all zero
        test::helper::check_sequence(
            swung_seq, [&](Note const &note) { REQUIRE(note.delay == 0.f); });
    }

    SECTION("0.5 Swing")
    {
        // Swing the sequence
        auto const swung_seq = std::get<Sequence>(modify::swing(seq, 0.5f));

        // Check that the swung sequence has the same length
        REQUIRE(swung_seq.cells.size() == seq.cells.size());

        // Check that all odd indexed cells are delayed by 0.5f and even by 0.f
        check_swing(swung_seq, 0.5f);
    }

    SECTION("1.0 Swing")
    {
        // Swing the sequence
        auto const swung_seq = std::get<Sequence>(modify::swing(seq, 1.0f));

        // Check that the swung sequence has the same length
        REQUIRE(swung_seq.cells.size() == seq.cells.size());

        check_swing(swung_seq, 1.0f);
    }

    SECTION("Throws")
    {
        REQUIRE_THROWS_AS(modify::swing(seq, 100.f), std::invalid_argument);
        REQUIRE_THROWS_AS(modify::swing(seq, -100.f), std::invalid_argument);
    }

    SECTION("Swing on Empty Sequence")
    {
        // Create an empty sequence
        auto const empty_seq = Sequence{};

        // Swing the sequence
        auto const swung_seq = std::get<Sequence>(modify::swing(empty_seq, 0.5f));

        // Check that the swung sequence has the same length
        REQUIRE(swung_seq.cells.size() == empty_seq.cells.size());

        // Check that the swung sequence is the same as the original
        REQUIRE(swung_seq == empty_seq);
    }

    SECTION("SubSequences")
    {
        // Create a test sequence with sub sequences
        auto const seqs = Sequence{
            {
                Note{0, 1.0f, 0.0f, 0.5f},
                Rest{},
                Sequence{
                    {
                        Note{1, 0.6f, 0.2f, 0.3f},
                        Note{2, 1.0f, 0.5f, 0.5f},
                        Rest{},
                        Note{3, 0.6f, 0.2f, 0.3f},
                    },
                },
                Note{4, 1.0f, 0.0f, 0.5f},
                Rest{},
                Sequence{
                    {
                        Note{5, 0.6f, 0.2f, 0.3f},
                        Note{6, 1.0f, 0.0f, 0.5f},
                        Rest{},
                        Note{7, 0.6f, 0.2f, 0.3f},
                    },
                },
                Note{8, 1.0f, 0.0f, 0.5f},
                Rest{},
                Sequence{
                    {
                        Note{9, 0.6f, 0.2f, 0.3f},
                        Note{10, 1.0f, 0.0f, 0.5f},
                        Rest{},
                        Note{11, 0.6f, 0.4f, 0.3f},
                    },
                },
            },
        };

        auto const swung_seq = std::get<Sequence>(modify::swing(seqs, 0.25f));
        REQUIRE(swung_seq.cells.size() == seqs.cells.size());

        check_swing(swung_seq, 0.25f);
    }
}

TEST_CASE("quantize", "[modify]")
{
    // Create a test sequence with subsequences
    auto const seq = Sequence{
        {
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{1, 0.6f, 0.2f, 0.3f},
                    Note{2, 1.0f, 0.5f, 0.5f},
                    Rest{},
                    Note{3, 0.6f, 0.2f, 0.3f},
                },
            },
            Note{4, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{5, 0.6f, 0.2f, 0.3f},
                    Note{6, 1.0f, 0.0f, 0.5f},
                    Rest{},
                    Note{7, 0.6f, 0.2f, 0.3f},
                },
            },
            Note{8, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{9, 0.6f, 0.2f, 0.3f},
                    Note{10, 1.0f, 0.0f, 0.5f},
                    Rest{},
                    Note{11, 0.6f, 0.4f, 0.3f},
                },
            },
        },
    };

    SECTION("Quantization")
    {
        // Quantize the sequence
        auto const quantized_seq = std::get<Sequence>(modify::quantize(seq, {0, {1}}));

        // Check that the quantized sequence has the same length
        REQUIRE(quantized_seq.cells.size() == seq.cells.size());

        // Check that the quantized sequence delays are all 0.f and gate is 1.f
        test::helper::check_sequence(quantized_seq, [&](Note const &note) {
            REQUIRE(note.delay == 0.f);
            REQUIRE(note.gate == 1.f);
        });
    }
}

TEST_CASE("mirror", "[modify]")
{
    // Create a test sequence with subsequences
    auto const seq = Sequence{
        {
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{1, 0.6f, 0.2f, 0.3f},
                    Note{2, 1.0f, 0.5f, 0.5f},
                    Rest{},
                    Note{3, 0.6f, 0.2f, 0.3f},
                },
            },
            Note{4, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{5, 0.6f, 0.2f, 0.3f},
                    Note{6, 1.0f, 0.0f, 0.5f},
                    Rest{},
                    Note{7, 0.6f, 0.2f, 0.3f},
                },
            },
            Note{8, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{9, 0.6f, 0.2f, 0.3f},
                    Note{10, 1.0f, 0.0f, 0.5f},
                    Rest{},
                    Note{11, 0.6f, 0.4f, 0.3f},
                },
            },
        },
    };

    SECTION("Mirror around zero")
    {
        // Mirror the sequence
        auto const mirrored_seq = std::get<Sequence>(modify::mirror(seq, {0, {1}}, 0));

        // Check that the mirrored sequence has the same length
        REQUIRE(mirrored_seq.cells.size() == seq.cells.size());

        // Create expected sequence
        Cell expected = seq;
        test::helper::modify_notes(expected,
                                   [](Note &note) { note.pitch = -note.pitch; });
        REQUIRE(mirrored_seq == expected);
    }

    SECTION("Mirror around +5")
    {
        // Mirror the sequence
        auto const mirrored_seq = std::get<Sequence>(modify::mirror(seq, {0, {1}}, 5));

        // Check that the mirrored sequence has the same length
        REQUIRE(mirrored_seq.cells.size() == seq.cells.size());

        // Create expected sequence
        Cell expected = seq;
        test::helper::modify_notes(
            expected, [](Note &note) { note.pitch = 5 + (5 - note.pitch); });
        REQUIRE(mirrored_seq == expected);
    }

    SECTION("Mirror around -10")
    {
        // Mirror the sequence
        auto const mirrored_seq =
            std::get<Sequence>(modify::mirror(seq, {0, {1}}, -10));

        // Check that the mirrored sequence has the same length
        REQUIRE(mirrored_seq.cells.size() == seq.cells.size());

        // Create expected sequence
        Cell expected = seq;
        test::helper::modify_notes(
            expected, [](Note &note) { note.pitch = -10 + (-10 - note.pitch); });
        REQUIRE(mirrored_seq == expected);
    }
}

TEST_CASE("reverse", "[modify]")
{
    // Create a test sequence with subsequences
    auto const seq = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Rest{},
        Sequence{
            {
                Note{1, 0.6f, 0.2f, 0.3f},
                Note{2, 1.0f, 0.5f, 0.5f},
                Rest{},
                Note{3, 0.6f, 0.2f, 0.3f},
            },
        },
        Note{4, 1.0f, 0.0f, 0.5f},
    }};

    SECTION("Reverse")
    {
        // Reverse the sequence
        auto const reversed_seq = std::get<Sequence>(modify::reverse(seq));

        // Check that the reversed sequence has the same length
        REQUIRE(reversed_seq.cells.size() == seq.cells.size());

        // Manually check each note
        REQUIRE(reversed_seq.cells[0] == seq.cells[3]);
        REQUIRE(reversed_seq.cells[2] == Cell{Rest{}});
        REQUIRE(reversed_seq.cells[3] == seq.cells[0]);

        auto const subseq = std::get<Sequence>(reversed_seq.cells[1]);
        REQUIRE(subseq.cells[0] == std::get<Sequence>(seq.cells[2]).cells[3]);
        REQUIRE(subseq.cells[1] == std::get<Sequence>(seq.cells[2]).cells[2]);
        REQUIRE(subseq.cells[2] == std::get<Sequence>(seq.cells[2]).cells[1]);
        REQUIRE(subseq.cells[3] == std::get<Sequence>(seq.cells[2]).cells[0]);
    }

    SECTION("Reverse Empty Sequence")
    {
        // Reverse the sequence
        auto const reversed_seq = std::get<Sequence>(modify::reverse(Sequence{}));

        // Check that the reversed sequence is empty
        REQUIRE(reversed_seq.cells.empty());
    }
}

TEST_CASE("repeat", "[modify]")
{
    auto const seq = Sequence{
        {
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Sequence{
                {
                    Note{1, 0.6f, 0.2f, 0.3f},
                    Note{2, 1.0f, 0.5f, 0.5f},
                    Rest{},
                    Note{3, 0.6f, 0.2f, 0.3f},
                },
            },
            Note{4, 1.0f, 0.0f, 0.5f},
        },
    };

    SECTION("Repeat 0 times throws")
    {
        REQUIRE_THROWS_AS(modify::repeat(seq, 0), std::invalid_argument);
    }

    SECTION("Repeat 1 time")
    {
        auto const repeated_seq = modify::repeat(seq, 1);
        REQUIRE(Sequence{{seq}} == repeated_seq);
    }

    SECTION("Repeat 5 times")
    {
        auto const repeated_seq = std::get<Sequence>(modify::repeat(seq, 5));
        REQUIRE(repeated_seq == Sequence{{seq, seq, seq, seq, seq}});
    }
}

TEST_CASE("stretch", "[modify]")
{
    auto const seq = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Rest{},
        Note{1, 1.0f, 0.0f, 0.5f},
    }};

    SECTION("Stretch by 0 throws")
    {
        REQUIRE_THROWS_AS(modify::stretch(seq, {0, {1}}, 0), std::invalid_argument);
    }

    SECTION("Stretch by 1")
    {
        auto const stretched_seq = modify::stretch(seq, {0, {1}}, 1);
        REQUIRE(stretched_seq == Sequence{{
                                     Sequence{{Note{0, 1.0f, 0.0f, 0.5f}}},
                                     Sequence{{Rest{}}},
                                     Sequence{{Note{1, 1.0f, 0.0f, 0.5f}}},
                                 }});
    }

    SECTION("Stretch by 3")
    {
        auto const stretched_seq =
            std::get<Sequence>(modify::stretch(seq, {0, {1}}, 3));

        REQUIRE(stretched_seq == Sequence{{
                                     Sequence{{
                                         Note{0, 1.0f, 0.0f, 0.5f},
                                         Note{0, 1.0f, 0.0f, 0.5f},
                                         Note{0, 1.0f, 0.0f, 0.5f},
                                     }},
                                     Sequence{{
                                         Rest{},
                                         Rest{},
                                         Rest{},
                                     }},
                                     Sequence{{
                                         Note{1, 1.0f, 0.0f, 0.5f},
                                         Note{1, 1.0f, 0.0f, 0.5f},
                                         Note{1, 1.0f, 0.0f, 0.5f},
                                     }},
                                 }});
    }
}

TEST_CASE("compress", "[modify]")
{
    auto const seq = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{0, 1.0f, 0.0f, 0.5f},
        Rest{},
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
    }};

    SECTION("Throws")
    {
        REQUIRE_THROWS_AS(modify::compress(seq, {0, {}}), std::invalid_argument);
    }

    SECTION("Compress by 1")
    {
        auto const compressed_seq = modify::compress(seq, {0, {1}});
        REQUIRE(compressed_seq == seq);
    }

    SECTION("Compress by 2")
    {
        auto const compressed_seq = std::get<Sequence>(modify::compress(seq, {0, {2}}));
        REQUIRE(compressed_seq.cells.size() == std::ceil(seq.cells.size() / 2.f));

        REQUIRE(compressed_seq.cells[0] == seq.cells[0]);
        REQUIRE(compressed_seq.cells[1] == seq.cells[2]);
        REQUIRE(compressed_seq.cells[2] == seq.cells[4]);
    }

    SECTION("Compress by 4")
    {
        auto const compressed_seq = std::get<Sequence>(modify::compress(seq, {0, {4}}));
        REQUIRE(compressed_seq.cells.size() == std::ceil(seq.cells.size() / 4.f));

        REQUIRE(compressed_seq.cells[0] == seq.cells[0]);
        REQUIRE(compressed_seq.cells[1] == seq.cells[4]);
    }

    SECTION("Subsequence")
    {
        auto const seq2 = Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{0, 1.0f, 0.0f, 0.5f},
            Sequence{{
                Note{0, 1.0f, 0.0f, 0.5f},
                Note{1, 1.0f, 0.0f, 0.5f},
                Note{1, 1.0f, 0.0f, 0.5f},
            }},
            Rest{},
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 1.0f, 0.0f, 0.5f},
            Note{1, 1.0f, 0.0f, 0.5f},
        }};

        auto const compressed_seq =
            std::get<Sequence>(modify::compress(seq2, {0, {2}}));
        REQUIRE(compressed_seq.cells.size() == std::ceil(seq2.cells.size() / 2.f));

        REQUIRE(compressed_seq.cells[0] == seq2.cells[0]);
        REQUIRE(compressed_seq.cells[1] == seq2.cells[2]);
        REQUIRE(compressed_seq.cells[2] == seq2.cells[4]);
        REQUIRE(compressed_seq.cells[3] == seq2.cells[6]);
    }
}

TEST_CASE("shuffle", "[modify]")
{
    // medium sequence with subsequence
    auto const seq = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{0, 1.0f, 0.0f, 0.5f},
        Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 1.0f, 0.0f, 0.5f},
            Note{2, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
        }},
        Rest{},
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
        Note{2, 1.0f, 0.0f, 0.5f},
    }};

    SECTION("shuffle returns a sequence with the same number of cells")
    {
        auto shuffled_seq = std::get<Sequence>(modify::shuffle(seq));

        REQUIRE(shuffled_seq.cells.size() == seq.cells.size());
    }

    SECTION("shuffle returns a sequence with the same cells")
    {
        auto shuffled_seq = std::get<Sequence>(modify::shuffle(seq));

        // Checking for variant type counts instead of exact cell equality
        for (auto i = 0u; i < std::variant_size_v<Cell>; ++i)
        {
            auto const count_original = std::ranges::count_if(
                seq.cells, [i](auto const &c) { return c.index() == i; });
            auto const count_shuffled = std::ranges::count_if(
                shuffled_seq.cells, [i](auto const &c) { return c.index() == i; });

            REQUIRE(count_shuffled == count_original);
        }
    }

    SECTION("shuffle returns a sequence with different cell order")
    {
        auto shuffled_seq = std::get<Sequence>(modify::shuffle(seq));

        REQUIRE_FALSE(std::ranges::equal(seq.cells, shuffled_seq.cells));
    }

    SECTION("Subsequence")
    {
        // Warning: This SECTION depends on the order of seq elements.
        auto shuffled_seq = std::get<Sequence>(modify::shuffle(seq));

        for (auto const &cell : shuffled_seq.cells)
        {
            if (std::holds_alternative<Sequence>(cell))
            {
                auto const &subseq = std::get<Sequence>(cell);
                for (auto i = 0u; i < std::variant_size_v<Cell>; ++i)
                {
                    auto const count_original = std::ranges::count_if(
                        std::get<Sequence>(seq.cells[2]).cells,
                        [i](auto const &c) { return c.index() == i; });

                    auto const count_shuffled = std::ranges::count_if(
                        subseq.cells, [i](auto const &c) { return c.index() == i; });

                    REQUIRE(count_shuffled == count_original);
                }
            }
        }
    }
}

TEST_CASE("concat", "[modify]")
{
    auto const seq1 = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{0, 1.0f, 0.0f, 0.5f},
        Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Note{1, 1.0f, 0.0f, 0.5f},
            Note{2, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
        }},
        Rest{},
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
        Note{2, 1.0f, 0.0f, 0.5f},
    }};

    // different than seq1
    auto const seq2 = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
        Note{2, 1.0f, 0.0f, 0.5f},
        Rest{},
        Note{0, 1.0f, 0.0f, 0.5f},
        Rest{},
    }};

    SECTION("concat returns a sequence with the total number of cells from each")
    {
        auto const concated_seq = std::get<Sequence>(modify::concat(seq1, seq2));

        REQUIRE(concated_seq.cells.size() == (seq1.cells.size() + seq2.cells.size()));
    }

    SECTION("concat returns a sequence with the same cells in order seq1, seq2")
    {
        auto const concated_seq = std::get<Sequence>(modify::concat(seq1, seq2));

        auto const seq1_cells = seq1.cells;
        auto const seq2_cells = seq2.cells;

        auto const concated_seq_cells = concated_seq.cells;

        REQUIRE(std::ranges::equal(
            seq1_cells, concated_seq_cells | std::views::take(seq1_cells.size())));
        REQUIRE(std::ranges::equal(
            seq2_cells, concated_seq_cells | std::views::drop(seq1_cells.size())));
    }

    SECTION("concat with empty sequence returns non-empty sequence")
    {
        auto const empty_seq = Sequence{};

        auto const concated_seq = std::get<Sequence>(modify::concat(seq1, empty_seq));

        REQUIRE(std::ranges::equal(seq1.cells, concated_seq.cells));
    }

    SECTION("concat of two emtpy sequences returns empty sequence")
    {
        auto const empty_seq = Sequence{};

        auto const concated_seq =
            std::get<Sequence>(modify::concat(empty_seq, empty_seq));

        REQUIRE(concated_seq.cells.empty());
    }
}
/**
 * @brief Merge two Sequences into a new Sequence by interweaving their notes
 *
 * If one Sequence is shorter than the other, it is repeated to be the same length
 * as the longer Sequence. If either Sequence is empty, the other Sequence is
 * returned.
 *
 * @param seq_a The first sequence.
 * @param seq_b The second sequence.
 * @return Sequence The merged sequence.
 */
// [[nodiscard]] inline auto merge(Sequence const &seq_a, Sequence const &seq_b)
//     -> Sequence

TEST_CASE("merge", "[modify]")
{
    auto const seq1 = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{0, 1.0f, 0.0f, 0.5f},
        Sequence{{
            Note{0, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{0, 1.0f, 0.0f, 0.5f},
        }},
        Rest{},
        Note{2, 1.0f, 0.0f, 0.5f},
    }};

    auto const seq2 = Sequence{{
        Note{1, 1.0f, 0.0f, 0.5f},
        Rest{},
        Note{3, 1.0f, 0.0f, 0.5f},
    }};

    SECTION("merge returns a sequence with 2 * max cell count cells")
    {
        auto const merged_seq = std::get<Sequence>(modify::merge(seq1, seq2));

        REQUIRE(merged_seq.cells.size() ==
                (std::max(seq1.cells.size(), seq2.cells.size()) * 2));
    }

    SECTION("merge returns a sequence with cells interleaved starting with seq1")
    {
        auto const merged_seq = std::get<Sequence>(modify::merge(seq1, seq2));

        for (auto i = 0u; i < merged_seq.cells.size(); ++i)
        {
            auto const &cell = merged_seq.cells[i];
            if (i % 2 == 0)
            {
                auto const &seq1_cell = seq1.cells[(i / 2) % seq1.cells.size()];
                REQUIRE(seq1_cell == cell);
            }
            else
            {
                auto const &seq2_cell = seq2.cells[(i / 2) % seq2.cells.size()];
                REQUIRE(seq2_cell == cell);
            }
        }
    }

    SECTION("merge with empty sequence returns the non-empty sequence")
    {
        auto const empty_seq = Sequence{};

        auto const merged_seq = modify::merge(seq1, empty_seq);

        REQUIRE(seq1 == merged_seq);
    }

    SECTION("merge two empty sequences returns empty sequence")
    {
        auto const empty_seq = Sequence{};

        auto const merged_seq = std::get<Sequence>(modify::merge(empty_seq, empty_seq));

        REQUIRE(merged_seq.cells.empty());
    }
}

TEST_CASE("divide", "[modify]")
{
    auto const seq = Sequence{{
        Note{0, 1.0f, 0.0f, 0.5f},
        Note{1, 1.0f, 0.0f, 0.5f},
        Sequence{{
            Note{2, 1.0f, 0.0f, 0.5f},
            Rest{},
            Note{0, 1.0f, 0.0f, 0.5f},
        }},
        Note{2, 1.0f, 0.0f, 0.5f},
        Rest{},
        Note{0, 1.0f, 0.0f, 0.5f},
        Rest{},
    }};

    SECTION("divide at index zero returns empty sequence and original sequence")
    {
        auto const x = std::get<Sequence>(modify::divide(seq, 0));
        auto const seq_a = std::get<Sequence>(x.cells[0]);
        auto const seq_b = std::get<Sequence>(x.cells[1]);

        REQUIRE(seq_a.cells.empty());
        REQUIRE(seq_b == seq);
    }

    SECTION("divide at end or beyond returns original sequence and empty sequence")
    {
        auto const x = std::get<Sequence>(modify::divide(seq, seq.cells.size()));
        auto const seq_a = std::get<Sequence>(x.cells[0]);
        auto const seq_b = std::get<Sequence>(x.cells[1]);

        REQUIRE(seq_a == seq);
        REQUIRE(seq_b.cells.empty());
    }

    SECTION("divide at index 3 returns first 3 cells and remaining cells")
    {
        auto const x = std::get<Sequence>(modify::divide(seq, 3));
        auto const seq_a = std::get<Sequence>(x.cells[0]);
        auto const seq_b = std::get<Sequence>(x.cells[1]);

        REQUIRE(std::ranges::equal(seq_a.cells, seq.cells | std::views::take(3)));
        REQUIRE(std::ranges::equal(seq_b.cells, seq.cells | std::views::drop(3)));
    }

    SECTION("splitting an empty sequence returns an pair of empty sequences")
    {
        auto const empty_seq = Sequence{};

        auto const x = std::get<Sequence>(modify::divide(empty_seq, 0));
        auto const seq_a = std::get<Sequence>(x.cells[0]);
        auto const seq_b = std::get<Sequence>(x.cells[1]);

        REQUIRE(seq_a.cells.empty());
        REQUIRE(seq_b.cells.empty());
    }
}