#include <sequence/midi.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include <sequence/sequence.hpp>
#include <sequence/timing.hpp>
#include <sequence/tuning.hpp>
#include <sequence/utility.hpp>

namespace sequence::midi
{
auto create_midi_note(int pitch, Tuning const &tuning, float tuning_base,
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

namespace
{

auto create_midi_note_visitor(MusicElement const &element, Tuning const &tuning,
                              float tuning_base, float pb_range)
    -> std::vector<MicrotonalNote>
{
    return std::visit(utility::overload{
                          [&](Note const &note) -> std::vector<MicrotonalNote> {
                              return std::vector{create_midi_note(
                                  note.pitch, tuning, tuning_base, pb_range)};
                          },
                          [&](Rest const &) -> std::vector<MicrotonalNote> {
                              return std::vector<MicrotonalNote>{};
                          },
                          [&](Sequence const &seq) -> std::vector<MicrotonalNote> {
                              auto notes = std::vector<MicrotonalNote>{};
                              for (auto const &subcell : seq.cells)
                              {
                                  auto const results = create_midi_note_visitor(
                                      subcell.element, tuning, tuning_base, pb_range);
                                  std::copy(std::cbegin(results), std::cend(results),
                                            std::back_inserter(notes));
                              }
                              return notes;
                          },
                      },
                      element);
}

[[nodiscard]] auto weighted_sample_count(float total_weight, std::uint32_t total_samples,
                                         float weight) -> std::uint32_t
{
    if (total_weight <= 0.f)
    {
        throw std::invalid_argument("sequence total weight must be greater than 0");
    }

    return static_cast<std::uint32_t>(
        std::round((weight / total_weight) * total_samples));
}

} // namespace

auto flatten_and_translate_to_midi_notes(Cell const &cell, Tuning const &tuning,
                                         float base_frequency, float pb_range)
    -> std::vector<MicrotonalNote>
{
    if (base_frequency <= 0.f)
    {
        throw std::invalid_argument("base_frequency must be greater than 0");
    }

    constexpr auto a4 = 69;       // MIDI note number for A4
    constexpr auto a4_hz = 440.f; // Frequency of A4

    auto const base_midi_note =
        12.f * std::log2(base_frequency / a4_hz) + static_cast<float>(a4);

    return create_midi_note_visitor(cell.element, tuning, base_midi_note, pb_range);
}

auto flatten_notes(MusicElement const &element) -> std::vector<Note>
{
    auto notes = std::vector<Note>{};
    std::visit(utility::overload{
                   [&](Note const &note) { notes.push_back(note); },
                   [](Rest const &) {},
                   [&](Sequence const &seq) {
                       for (auto const &child : seq.cells)
                       {
                           auto const child_notes = flatten_notes(child.element);
                           std::copy(std::cbegin(child_notes), std::cend(child_notes),
                                     std::back_inserter(notes));
                       }
                   },
               },
               element);
    return notes;
}

auto flatten_notes(Cell const &cell) -> std::vector<Note>
{
    return flatten_notes(cell.element);
}

auto note_sample_infos(Cell const &cell, std::uint32_t total_samples, float offset)
    -> std::vector<SampleRange>
{
    return std::visit(
        utility::overload{
            [&](Note const &note) {
                auto const delay =
                    static_cast<std::uint32_t>((float)total_samples * note.delay);
                auto const note_samples = static_cast<std::uint32_t>(
                    ((float)total_samples - (float)delay) * note.gate);
                return std::vector{SampleRange{
                    static_cast<std::uint32_t>(offset + delay),
                    static_cast<std::uint32_t>(offset + delay + note_samples),
                }};
            },
            [](Rest const &) { return std::vector<SampleRange>{}; },
            [&](Sequence const &seq) {
                auto const total_weight =
                    std::accumulate(std::cbegin(seq.cells), std::cend(seq.cells), 0.f,
                                    [](float sum, Cell const &c) {
                                        return sum + c.weight;
                                    });

                auto infos = std::vector<SampleRange>{};
                for (auto const &c : seq.cells)
                {
                    auto const sample_count =
                        weighted_sample_count(total_weight, total_samples, c.weight);
                    auto const result = note_sample_infos(c, sample_count, offset);
                    std::copy(std::cbegin(result), std::cend(result),
                              std::back_inserter(infos));
                    offset += sample_count;
                }
                return infos;
            },
        },
        cell.element);
}

auto flatten_and_translate_to_sample_infos(Cell const &cell,
                                           TimeSignature const &time_signature,
                                           std::uint32_t sample_rate, float bpm)
    -> std::vector<SampleRange>
{
    auto const total_samples = samples_count(cell, time_signature, sample_rate, bpm);
    return note_sample_infos(cell, total_samples, 0);
}

auto translate_to_midi_timeline(Cell const &cell,
                                TimeSignature const &time_signature,
                                std::uint32_t sample_rate, float bpm,
                                Tuning const &tuning, float base_frequency,
                                float pb_range) -> std::vector<TimedMidiNote>
{
    auto timed_midi_notes = std::vector<TimedMidiNote>{};

    auto const ranges =
        flatten_and_translate_to_sample_infos(cell, time_signature, sample_rate, bpm);

    auto const midi_notes =
        flatten_and_translate_to_midi_notes(cell, tuning, base_frequency, pb_range);

    auto const notes = flatten_notes(cell);

    assert(ranges.size() == midi_notes.size());
    assert(ranges.size() == notes.size());

    timed_midi_notes.reserve(ranges.size());
    for (auto i = 0u; i < ranges.size(); ++i)
    {
        auto const [begin, end] = ranges[i];
        auto const [note, pitch_bend] = midi_notes[i];
        std::uint8_t const velocity =
            static_cast<std::uint8_t>(notes[i].velocity * 127);

        timed_midi_notes.emplace_back(TimedMidiNote{
            .begin = begin,
            .end = end,
            .note = note,
            .velocity = velocity,
            .pitch_bend = pitch_bend,
        });
    }

    return timed_midi_notes;
}

} // namespace sequence::midi
