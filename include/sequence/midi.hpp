#ifndef SEQUENCY_MIDI_HPP
#define SEQUENCY_MIDI_HPP
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

#include <sequence/measure.hpp>
#include <sequence/sequence.hpp>
#include <sequence/tuning.hpp>
#include <sequence/utility.hpp>

namespace sequence::midi
{

/**
 * @brief A MIDI note to represent any microtone using pitch bend.
 *
 * The note is a number from 0 to 127, where 60 is middle C. The pitch bend
 * is a number from 0 to 16383, where 8192 is the center position. This assumes the
 * pitch bend is set to cover 2 semitones, one in each direction.
 */
struct MicrotonalNote
{
    std::uint8_t note = 60;
    std::uint16_t pitch_bend = 8192;
};

/**
 * @brief Creates a MIDI note from a Note, Tuning and base fractional note.
 *
 * @param note The note to use.
 * @param tuning The tuning to use for the note.
 * @param tuning_base The base note of the tuning, as a floating point value. This is a
 * midi note value but allows for fractional notes that coorespond to any value
 * inbetween midi notes.
 * @return MicrotonalNote
 *
 * @throws std::invalid_argument if the tuning is empty.
 */
[[nodiscard]] inline auto create_midi_note(int interval, Tuning const &tuning,
                                           float tuning_base) -> MicrotonalNote
{
    if (tuning.intervals.empty())
    {
        throw std::invalid_argument("Tuning must not be empty");
    }

    auto const fractional_note = tuning_base + [&] {
        constexpr auto semitone_cents = 100.f;
        auto const length = static_cast<int>(tuning.intervals.size());

        auto const octave_offset = static_cast<int>(interval / length) * tuning.octave;
        auto const interval_offset = [&] {
            auto const interval_index = interval % length;
            if (interval_index < 0)
            {
                return tuning.intervals[interval_index + length] - tuning.octave;
            }
            else
            {
                return tuning.intervals[interval_index];
            }
        }();

        return (octave_offset + interval_offset) / semitone_cents;
    }();

    auto integral = 0.f;
    auto const fractional =
        std::modf(std::clamp(fractional_note, 0.f, 127.f), &integral);
    return MicrotonalNote{static_cast<std::uint8_t>(integral),
                          static_cast<std::uint16_t>((1 + fractional) * 8192)};
}

// [[nodiscard]] inline auto create_midi_note_visitor(Tuning const &tuning, float
// tuning_base)
// {
//     return utility::overload{
//         [&](Note const &note) -> std::vector<MicrotonalNote> {
//             return std::vector{create_midi_note(note, tuning, tuning_base)};
//         },
//         [&](Rest const &) -> std::vector<MicrotonalNote> {
//             return std::vector<MicrotonalNote>{};
//         },
//         [&](Sequence const &seq) -> std::vector<MicrotonalNote> {
//             return create_midi_note(seq.cells[0], seq.cells[0].tuning, tuning_base);
//         },
//     };
// }

/**
 * @brief Calculates the MIDI notes for a measure.
 *
 * @param measure The measure to calculate the MIDI notes for.
 * @param base_frequency The base frequency of the tuning. Defaults to 440 Hz.
 * @return std::vector<MicrotonalNote>
 */
// [[nodiscard]] inline auto calculate_midi_notes(Measure const &measure,
//                                                float base_frequency = 440.f)
//     -> std::vector<MicrotonalNote>
// {
//     constexpr auto a4 = 69;       // MIDI note number for A4
//     constexpr auto a4_hz = 440.f; // Frequency of A4

//     auto const base_note = 12 * std::log2(base_frequency / a4_hz) + a4;

//     auto notes = std::vector<MicrotonalNote>{};

//     // TODO recursive visit
//     for (auto const &cell : measure.sequence)
//     {
//         /// TODO visit cell instead of assuming its a Note.
//         // offset from base in fractional semitones
//         auto const offset = (measure.tuning[cell.interval % measure.tuning.size()] *
//                              (1 + (cell.interval / measure.tuning.size()))) /
//                             100.f;
//         auto integral = 0.f;
//         auto const fractional = std::modf(offset, &integral);
//         notes.push_back(std::pair{static_cast<std::uint8_t>(base_note + integral),
//                                   static_cast<std::uint16_t>((1 + fractional) *
//                                   8192)});
//     }

//     return notes;
// }

// [[nodiscard]] inline auto transpose(MicrotonalNote, int semitones) -> MicrotonalNote
// {
//     return {};
// }

} // namespace sequence::midi
#endif // SEQUENCY_MIDI_HPP