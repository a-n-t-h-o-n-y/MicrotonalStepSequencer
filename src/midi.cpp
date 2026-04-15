#include <sequence/midi.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <variant>
#include <vector>

#include <sequence/utility.hpp>

namespace
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

    auto operator==(MicrotonalNote const &) const -> bool = default;
    auto operator!=(MicrotonalNote const &) const -> bool = default;
};

/**
 * @brief Creates a MIDI note from a Note, Tuning and base fractional note.
 *
 * @param pitch The pitch value to use, this is the value from Note.pitch, not the midi
 * note number.
 * @param tuning The tuning to use for the note.
 * @param tuning_base The base note of the tuning, as a floating point value. This is a
 * MIDI note value but allows for fractional notes that correspond to any value
 * in between MIDI notes.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return MicrotonalNote
 * @throws std::invalid_argument if the tuning is empty or if \p pb_range is not
 * greater than zero.
 */
[[nodiscard]]
auto create_midi_note(int pitch,
                      sequence::Tuning const &tuning,
                      float tuning_base,
                      float pb_range) -> MicrotonalNote
{
    if (tuning.intervals.empty())
    {
        throw std::invalid_argument("Tuning must not be empty");
    }
    if (pb_range <= 0.f)
    {
        throw std::invalid_argument("pb_range must be greater than 0");
    }

    auto const fractional_note = tuning_base + [&] {
        constexpr auto semitone_cents = 100.f;
        auto const length = (int)tuning.intervals.size();

        auto const octave_offset = (float)(pitch / length) * tuning.octave;
        auto const interval_offset = [&] {
            auto const interval_index = pitch % length;
            if (interval_index < 0)
            {
                return tuning.intervals[(std::size_t)(interval_index + length)] -
                       tuning.octave;
            }
            else
            {
                return tuning.intervals[(std::size_t)interval_index];
            }
        }();

        return (octave_offset + interval_offset) / semitone_cents;
    }();

    auto integral = 0.f;
    auto const fractional =
        std::modf(std::clamp(fractional_note, 0.f, 127.f), &integral);
    return MicrotonalNote{
        static_cast<std::uint8_t>(integral),
        static_cast<std::uint16_t>(8'192 + (fractional * 8'192.f / pb_range))};
}

/**
 * @brief Creates a timed MIDI note from a Note and an allocated sample span.
 *
 * The returned note uses \p sample_offset and \p sample_count as the containing
 * timespan for the note, then applies Note.delay and Note.gate within that span to
 * calculate the final begin and end sample positions.
 *
 * @throws std::invalid_argument if \p tuning is empty, if \p base_frequency is not
 * greater than zero, or if \p pb_range is not greater than zero.
 */
[[nodiscard]]
auto create_timed_midi_note(sequence::Note const &note,
                            std::uint32_t sample_offset,
                            std::uint32_t sample_count,
                            sequence::Tuning const &tuning,
                            float base_frequency,
                            float pb_range) -> sequence::midi::TimedMidiNote
{
    if (base_frequency <= 0.f)
    {
        throw std::invalid_argument("base_frequency must be greater than 0");
    }

    constexpr auto a4 = 69;       // MIDI note number for A4
    constexpr auto a4_hz = 440.f; // Frequency of A4

    auto const base_midi_note =
        12.f * std::log2(base_frequency / a4_hz) + static_cast<float>(a4);
    auto const [midi_note, pitch_bend] =
        create_midi_note(note.pitch, tuning, base_midi_note, pb_range);

    auto const delay =
        static_cast<std::uint32_t>(static_cast<float>(sample_count) * note.delay);
    auto const note_samples = static_cast<std::uint32_t>(
        (static_cast<float>(sample_count) - static_cast<float>(delay)) * note.gate);

    return sequence::midi::TimedMidiNote{
        .begin = sample_offset + delay,
        .end = sample_offset + delay + note_samples,
        .note = midi_note,
        .velocity = static_cast<std::uint8_t>(note.velocity * 127),
        .pitch_bend = pitch_bend,
    };
}

} // namespace

namespace sequence::midi
{

auto flatten_to_midi(std::vector<MusicElement> const &elements,
                     std::uint32_t sample_offset,
                     std::uint32_t sample_count,
                     Tuning const &tuning,
                     float base_frequency,
                     float pb_range) -> std::vector<TimedMidiNote>
{
    if (tuning.intervals.empty())
    {
        throw std::invalid_argument("Tuning must not be empty");
    }
    if (base_frequency <= 0.f)
    {
        throw std::invalid_argument("base_frequency must be greater than 0");
    }
    if (pb_range <= 0.f)
    {
        throw std::invalid_argument("pb_range must be greater than 0");
    }

    auto results = std::vector<TimedMidiNote>{};

    for (auto const &element : elements)
    {
        std::visit(
            utility::overload{
                [&](Note const &note) {
                    results.push_back(create_timed_midi_note( // TOD-
                        note, sample_offset, sample_count, tuning, base_frequency,
                        pb_range));
                },
                [&](Sequence const &seq) {
                    auto const total_weight = std::accumulate(
                        std::cbegin(seq.cells), std::cend(seq.cells), 0.,
                        [](double sum, Cell const &cell) {
                            return sum + static_cast<double>(cell.weight);
                        });
                    if (total_weight <= 0.)
                    {
                        throw std::invalid_argument(
                            "sequence total weight must be greater than 0");
                    }

                    auto current_offset = static_cast<double>(sample_offset);
                    auto const sequence_end = sample_offset + sample_count;

                    for (auto i = 0u; i < seq.cells.size(); ++i)
                    {
                        auto const &cell = seq.cells[i];
                        auto const exact_count =
                            static_cast<double>(sample_count) *
                            (static_cast<double>(cell.weight) / total_weight);
                        auto const cell_sample_offset =
                            static_cast<std::uint32_t>(std::round(current_offset));
                        current_offset += exact_count;
                        auto const cell_end = i + 1 == seq.cells.size()
                                                  ? sequence_end
                                                  : static_cast<std::uint32_t>(
                                                        std::round(current_offset));
                        auto const cell_sample_count = cell_end - cell_sample_offset;
                        auto sub_results = flatten_to_midi(
                            cell.elements, cell_sample_offset, cell_sample_count,
                            tuning, base_frequency, pb_range);
                        std::move(std::begin(sub_results), std::end(sub_results),
                                  std::back_inserter(results));
                    }
                },
            },
            element);
    }

    return results;
}

} // namespace sequence::midi
