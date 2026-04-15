#pragma once

#include <cstdint>
#include <vector>

#include <sequence/sequence.hpp>
#include <sequence/tuning.hpp>

namespace sequence::midi
{

/**
 * @brief A MIDI note event with absolute sample timing.
 *
 * begin and end are absolute sample positions in the rendered timeline. note,
 * velocity, and pitch_bend together describe the MIDI event to play over that span.
 */
struct TimedMidiNote
{
    std::uint32_t begin;
    std::uint32_t end;
    std::uint8_t note;
    std::uint8_t velocity;
    std::uint16_t pitch_bend;

    auto operator==(TimedMidiNote const &) const -> bool = default;
    auto operator!=(TimedMidiNote const &) const -> bool = default;
};

/**
 * @brief Flattens a set of recursive simultaneous music elements into timed MIDI notes.
 *
 * Notes in \p elements are treated as simultaneous within the provided sample span.
 * Any nested Sequence elements subdivide that same span across their child cells
 * according to child cell weight, preserving sequential timing within the subsequence.
 *
 * @param elements The simultaneous music elements to flatten.
 * @param sample_offset The absolute starting sample for these elements.
 * @param sample_count The total number of samples allocated to these elements.
 * @param tuning The tuning used to translate note pitches to MIDI note and pitch bend.
 * @param base_frequency The base frequency for note pitch 0.
 * @param pb_range The pitch bend range expected by the MIDI receiver.
 * @return std::vector<TimedMidiNote>
 *
 * @throws std::invalid_argument if \p tuning is empty, if \p base_frequency is not
 * greater than zero, if \p pb_range is not greater than zero, or if any visited
 * Sequence has a total child weight that is not greater than zero.
 */
[[nodiscard]]
auto flatten_to_midi(std::vector<MusicElement> const &elements,
                     std::uint32_t sample_offset,
                     std::uint32_t sample_count,
                     Tuning const &tuning,
                     float base_frequency,
                     float pb_range) -> std::vector<TimedMidiNote>;

} // namespace sequence::midi
