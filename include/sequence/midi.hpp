#ifndef SEQUENCY_MIDI_HPP
#define SEQUENCY_MIDI_HPP
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
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

[[nodiscard]] inline auto create_midi_note_visitor(Cell const &cell,
                                                   Tuning const &tuning,
                                                   float tuning_base)
    -> std::vector<MicrotonalNote>
{
    return std::visit(utility::overload{
                          [&](Note const &note) -> std::vector<MicrotonalNote> {
                              return std::vector{
                                  create_midi_note(note.interval, tuning, tuning_base)};
                          },
                          [&](Rest const &) -> std::vector<MicrotonalNote> {
                              return std::vector<MicrotonalNote>{};
                          },
                          [&](Sequence const &seq) -> std::vector<MicrotonalNote> {
                              auto notes = std::vector<MicrotonalNote>{};
                              for (auto const &subcell : seq.cells)
                              {
                                  auto const results = create_midi_note_visitor(
                                      subcell, tuning, tuning_base);
                                  std::copy(std::cbegin(results), std::cend(results),
                                            std::back_inserter(notes));
                              }
                              return notes;
                          },
                      },
                      cell);
}

/**
 * @brief Calculates the MIDI notes for a measure.
 *
 * @param measure The measure to calculate the MIDI notes for.
 * @param base_frequency The base frequency of the tuning. Defaults to 440 Hz. This is
 * what note.interval 0 will be.
 * @return std::vector<MicrotonalNote>
 */
[[nodiscard]] inline auto flatten_and_translate_to_midi_notes(
    Measure const &measure, Tuning const &tuning, float base_frequency = 440.f)
    -> std::vector<MicrotonalNote>
{
    constexpr auto a4 = 69;       // MIDI note number for A4
    constexpr auto a4_hz = 440.f; // Frequency of A4

    auto const base_midi_note =
        12.f * std::log2(base_frequency / a4_hz) + static_cast<float>(a4);

    auto notes = std::vector<MicrotonalNote>{};

    for (auto const &cell : measure.sequence.cells)
    {
        auto const results = create_midi_note_visitor(cell, tuning, base_midi_note);
        std::copy(std::cbegin(results), std::cend(results), std::back_inserter(notes));
    }

    return notes;
}

[[nodiscard]] inline auto flatten_and_translate_to_midi_notes(
    std::vector<Measure> const &measures, Tuning const &tuning,
    float base_frequency = 440.f) -> std::vector<MicrotonalNote>
{
    auto notes = std::vector<MicrotonalNote>{};

    for (auto const &measure : measures)
    {
        auto const results =
            flatten_and_translate_to_midi_notes(measure, tuning, base_frequency);
        std::copy(std::cbegin(results), std::cend(results), std::back_inserter(notes));
    }

    return notes;
}

/**
 * @brief Flattens a sequence into a vector of notes.
 *
 * Only the Notes are returned, including Notes in subsequences, rests are ignored.
 *
 * @param sequence The sequence to flatten.
 * @return std::vector<Note>
 */
[[nodiscard]] inline auto flatten_notes(Sequence const &seq) -> std::vector<Note>
{
    auto notes = std::vector<Note>{};
    for (auto const &cell : seq.cells)
    {
        std::visit(utility::overload{
                       [&](Note const &note) { notes.push_back(note); },
                       [&](Rest const &) {},
                       [&](Sequence const &subseq) {
                           auto const subseq_notes = flatten_notes(subseq);
                           std::copy(std::cbegin(subseq_notes), std::cend(subseq_notes),
                                     std::back_inserter(notes));
                       },
                   },
                   cell);
    }
    return notes;
}

[[nodiscard]] inline auto flatten_notes(std::vector<Measure> const &measures)
    -> std::vector<Note>
{
    auto notes = std::vector<Note>{};

    for (auto const &measure : measures)
    {
        auto const measure_notes = flatten_notes(measure.sequence);
        std::copy(std::cbegin(measure_notes), std::cend(measure_notes),
                  std::back_inserter(notes));
    }

    return notes;
}

/// Stores the starting and ending audio sample for a note.
struct SampleRange
{
    std::uint32_t begin;
    std::uint32_t end;
};

/**
 * @brief Calculates the sample count and offset for each note in a sequence.
 *
 * This will recurse into subsequences. Only returns sample counts for Notes, Rests are
 * ignored.
 *
 * @param seq The sequence to calculate the sample counts for.
 * @param total_samples The total number of samples for the sequence.
 * @param offset The offset of the sequence in samples. Float to avoid rounding errors.
 * @return std::vector<SampleRange>
 */
[[nodiscard]] inline auto note_sample_infos(Sequence const &seq,
                                            std::uint32_t total_samples,
                                            float offset = 0.f)
    -> std::vector<SampleRange>
{
    // This is float to avoid accumulating rounding errors.
    float const samples_per_cell = static_cast<float>(total_samples) / seq.cells.size();

    auto infos = std::vector<SampleRange>{};

    for (auto const &cell : seq.cells)
    {
        auto const result = std::visit(
            utility::overload{
                [&](Note const &note) {
                    std::uint32_t const delay = samples_per_cell * note.delay;
                    std::uint32_t const note_samples =
                        (samples_per_cell - delay) * note.gate;
                    return std::vector{
                        SampleRange(offset + delay, offset + delay + note_samples)};
                },
                [](Rest const &) { return std::vector<SampleRange>{}; },
                [&](Sequence const &subseq) {
                    return note_sample_infos(subseq, samples_per_cell, offset);
                },
            },
            cell);
        std::copy(std::cbegin(result), std::cend(result), std::back_inserter(infos));
        offset += samples_per_cell;
    }
    return infos;
}

/**
 * @brief Flattens a measure into a vector of SampleRange objects.
 *
 * Only the Notes are returned, including Notes in subsequences, rests do not have an
 * entry. The sample counts are calculated from the measure's sample rate and bpm, and
 * note delay and gate are used, as well as the Measure's time signature and resolution.
 *
 * @param measure The measure to flatten.
 * @return std::vector<SampleRange>
 */
[[nodiscard]] inline auto flatten_and_translate_to_sample_infos(
    std::vector<Measure> const &measures, std::uint32_t sample_rate, std::uint16_t bpm)
    -> std::vector<SampleRange>
{
    auto infos = std::vector<SampleRange>{};

    auto sample_offset = 0.f;

    for (auto const &measure : measures)
    {
        auto const samples_per_measure = [&] {
            auto const &time_sig = measure.time_signature;

            auto const samples_per_beat = static_cast<float>(sample_rate) * 60.f / bpm;
            auto const beats_per_measure =
                (static_cast<float>(time_sig.numerator) / time_sig.denominator) * 2;
            return samples_per_beat * beats_per_measure;
        }();
        auto const result =
            note_sample_infos(measure.sequence, samples_per_measure, sample_offset);
        std::copy(std::cbegin(result), std::cend(result), std::back_inserter(infos));
        sample_offset += samples_per_measure;
    }
    return infos;
}

struct NoteOn
{
    std::uint8_t note;
    std::uint8_t velocity;
};

struct NoteOff
{
    std::uint8_t note;
};

struct PitchBend
{
    std::uint16_t value;
};

using Event = std::variant<NoteOn, NoteOff, PitchBend>;

/// A MIDI Event paired with a sample offset.
using EventTimeline = std::vector<std::pair<Event, std::uint32_t>>;

/**
 * @brief Flattens a measure into a vector of MIDI events.
 *
 * Returns a sequence of MIDI events of type NoteOn, NoteOff, or PitchBend, paired with
 * the sample offset of the event.
 *
 * @param measures The measures to create a timeline of.
 *
 * @return EventTimeline
 */
[[nodiscard]] inline auto translate_to_midi_timeline(
    std::vector<Measure> const &measures, std::uint32_t sample_rate, std::uint16_t bpm,
    Tuning const &tuning, float base_frequency) -> EventTimeline
{
    auto midi_events = EventTimeline{};

    auto const ranges =
        flatten_and_translate_to_sample_infos(measures, sample_rate, bpm);

    auto const midi_notes =
        flatten_and_translate_to_midi_notes(measures, tuning, base_frequency);

    auto const notes = flatten_notes(measures);

    assert(ranges.size() == midi_notes.size());
    assert(ranges.size() == notes.size());

    // create a midi event for each entry
    for (auto i = 0u; i < ranges.size(); ++i)
    {
        auto const [begin, end] = ranges[i];
        auto const [note, pitch_bend] = midi_notes[i];
        std::uint8_t const velocity = notes[i].velocity * 127;

        midi_events.emplace_back(NoteOn{note, velocity}, begin);
        midi_events.emplace_back(PitchBend{pitch_bend}, begin);
        midi_events.emplace_back(NoteOff{note}, end);
    }

    return midi_events;
}

} // namespace sequence::midi
#endif // SEQUENCY_MIDI_HPP