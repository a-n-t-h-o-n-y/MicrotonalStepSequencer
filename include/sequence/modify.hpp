#pragma once

#include <cstddef>
#include <vector>

#include <sequence/pattern.hpp>
#include <sequence/sequence.hpp>
#include <sequence/utility.hpp>

namespace sequence::modify
{

/// Randomizes note pitches in the selected target. For sequences, pattern matching is
/// evaluated independently at each sequence level. Throws if min > max.
[[nodiscard]]
auto randomize_pitch(MusicElement element, Pattern const &pattern, int min, int max)
    -> MusicElement;

[[nodiscard]]
auto randomize_pitch(Cell cell, Pattern const &pattern, int min, int max) -> Cell;

/// Randomizes note velocities in the selected target. Pattern matching is evaluated
/// independently at each sequence level. Throws if min > max or either bound is
/// outside [0, 1].
[[nodiscard]]
auto randomize_velocity(MusicElement element,
                        Pattern const &pattern,
                        float min,
                        float max) -> MusicElement;

[[nodiscard]]
auto randomize_velocity(Cell cell, Pattern const &pattern, float min, float max)
    -> Cell;

/// Randomizes note delays in the selected target. Pattern matching is evaluated
/// independently at each sequence level. Throws if min > max or either bound is
/// outside [0, 1].
[[nodiscard]]
auto randomize_delay(MusicElement element, Pattern const &pattern, float min, float max)
    -> MusicElement;

[[nodiscard]]
auto randomize_delay(Cell cell, Pattern const &pattern, float min, float max) -> Cell;

/// Randomizes note gates in the selected target. Pattern matching is evaluated
/// independently at each sequence level. Throws if min > max or either bound is
/// outside [0, 1].
[[nodiscard]]
auto randomize_gate(MusicElement element, Pattern const &pattern, float min, float max)
    -> MusicElement;

[[nodiscard]]
auto randomize_gate(Cell cell, Pattern const &pattern, float min, float max) -> Cell;

/// Shifts note pitch by a constant amount. Pattern matching is evaluated
/// independently at each sequence level.
[[nodiscard]]
auto shift_pitch(MusicElement element, Pattern const &pattern, int amount)
    -> MusicElement;

[[nodiscard]]
auto shift_pitch(Cell cell, Pattern const &pattern, int amount) -> Cell;

/// Shifts note velocities by a constant amount, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto shift_velocity(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement;

[[nodiscard]]
auto shift_velocity(Cell cell, Pattern const &pattern, float amount) -> Cell;

/// Shifts note delays by a constant amount, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto shift_delay(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement;

[[nodiscard]]
auto shift_delay(Cell cell, Pattern const &pattern, float amount) -> Cell;

/// Shifts note gates by a constant amount, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto shift_gate(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement;

[[nodiscard]]
auto shift_gate(Cell cell, Pattern const &pattern, float amount) -> Cell;

/// Sets note pitch to a constant value. Pattern matching is evaluated independently at
/// each sequence level.
[[nodiscard]]
auto set_pitch(MusicElement element, Pattern const &pattern, int pitch) -> MusicElement;

[[nodiscard]]
auto set_pitch(Cell cell, Pattern const &pattern, int pitch) -> Cell;

/// Sets note octave while preserving degree within the tuning length. Pattern matching
/// is evaluated independently at each sequence level. Throws if tuning_length is zero.
[[nodiscard]]
auto set_octave(MusicElement element,
                Pattern const &pattern,
                int octave,
                std::size_t tuning_length) -> MusicElement;

[[nodiscard]]
auto set_octave(Cell cell,
                Pattern const &pattern,
                int octave,
                std::size_t tuning_length) -> Cell;

/// Sets note velocity to a constant value, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto set_velocity(MusicElement element, Pattern const &pattern, float velocity)
    -> MusicElement;

[[nodiscard]]
auto set_velocity(Cell cell, Pattern const &pattern, float velocity) -> Cell;

/// Sets note delay to a constant value, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto set_delay(MusicElement element, Pattern const &pattern, float delay)
    -> MusicElement;

[[nodiscard]]
auto set_delay(Cell cell, Pattern const &pattern, float delay) -> Cell;

/// Sets note gate to a constant value, clamped to [0, 1]. Pattern matching is
/// evaluated independently at each sequence level.
[[nodiscard]]
auto set_gate(MusicElement element, Pattern const &pattern, float gate) -> MusicElement;

[[nodiscard]]
auto set_gate(Cell cell, Pattern const &pattern, float gate) -> Cell;

/// Rotates sequence cell order. Positive values shift right, negative values shift
/// left. Non-sequence cells are unchanged.
[[nodiscard]]
auto rotate(MusicElement element, int amount) -> MusicElement;

[[nodiscard]]
auto rotate(Cell cell, int amount) -> Cell;

/// Mirrors note pitch around center_note. Pattern matching is evaluated independently
/// at each sequence level.
[[nodiscard]]
auto mirror(MusicElement element, Pattern const &pattern, int center_note)
    -> MusicElement;

[[nodiscard]]
auto mirror(Cell cell, Pattern const &pattern, int center_note) -> Cell;

/// Reverses sequence cell order recursively through nested sequences.
[[nodiscard]]
auto reverse(MusicElement element) -> MusicElement;

[[nodiscard]]
auto reverse(Cell cell) -> Cell;

/// Returns a sequence containing count copies of the selected target. Does not recurse
/// into child sequences. Throws if count is zero.
[[nodiscard]]
auto repeat(MusicElement element, std::size_t count) -> MusicElement;

[[nodiscard]]
auto repeat(Cell cell, std::size_t count) -> Cell;

/// Repeats each selected cell amount times in a row. Returns a sequence and recurses
/// into child sequences. Throws if amount is zero.
[[nodiscard]]
auto stretch(MusicElement element, Pattern const &pattern, std::size_t amount)
    -> MusicElement;

[[nodiscard]]
auto stretch(Cell cell, Pattern const &pattern, std::size_t amount) -> Cell;

/// Keeps only cells selected by pattern in any selected sequence target. Does not
/// recurse into child sequences. Non-sequence targets are unchanged.
[[nodiscard]]
auto compress(MusicElement element, Pattern const &pattern) -> MusicElement;

[[nodiscard]]
auto compress(Cell cell, Pattern const &pattern) -> Cell;

/// Shuffles sequence cell order recursively. Simultaneous cell elements are not
/// reordered.
[[nodiscard]]
auto shuffle(MusicElement element) -> MusicElement;

[[nodiscard]]
auto shuffle(Cell cell) -> Cell;

/// Creates a note as a MusicElement. Throws if vel, delay, or gate is outside [0, 1].
[[nodiscard]]
auto note(int pitch, float velocity, float delay, float gate) -> MusicElement;

} // namespace sequence::modify
