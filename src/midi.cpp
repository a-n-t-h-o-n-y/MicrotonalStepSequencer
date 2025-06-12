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

#include <sequence/measure.hpp>
#include <sequence/sequence.hpp>
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

auto flatten_and_translate_to_midi_notes(Measure const &measure, Tuning const &tuning,
                                         float base_frequency, float pb_range)
    -> std::vector<MicrotonalNote>
{
    constexpr auto a4 = 69;       // MIDI note number for A4
    constexpr auto a4_hz = 440.f; // Frequency of A4

    auto const base_midi_note =
        12.f * std::log2(base_frequency / a4_hz) + static_cast<float>(a4);

    return create_midi_note_visitor(measure.cell.element, tuning, base_midi_note,
                                    pb_range);
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

auto flatten_notes(Phrase const &phrase) -> std::vector<Note>
{
    auto notes = std::vector<Note>{};

    for (auto const &measure : phrase)
    {
        auto const measure_notes = flatten_notes(measure.cell.element);
        std::copy(std::cbegin(measure_notes), std::cend(measure_notes),
                  std::back_inserter(notes));
    }

    return notes;
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
                auto const sample_count_from_weight = [&seq, total_samples] {
                    auto const total_weight = std::accumulate(
                        std::cbegin(seq.cells), std::cend(seq.cells), 0.f,
                        [](float sum, Cell const &c) { return sum + c.weight; });
                    return [total_weight, total_samples](float weight) {
                        return static_cast<std::uint32_t>(
                            std::round((weight / total_weight) * total_samples));
                    };
                }();

                auto infos = std::vector<SampleRange>{};
                for (auto const &c : seq.cells)
                {
                    auto const sample_count = sample_count_from_weight(c.weight);
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

auto flatten_and_translate_to_sample_infos(Measure const &measure,
                                           std::uint32_t sample_rate, float bpm)
    -> std::vector<SampleRange>
{

    auto const samples_per_measure = samples_count(measure, sample_rate, bpm);
    return note_sample_infos({measure.cell.element}, samples_per_measure, 0);
}

auto translate_to_midi_timeline(Measure const &measure, std::uint32_t sample_rate,
                                float bpm, Tuning const &tuning, float base_frequency,
                                float pb_range) -> EventTimeline
{
    auto midi_events = EventTimeline{};

    auto const ranges =
        flatten_and_translate_to_sample_infos(measure, sample_rate, bpm);

    auto const midi_notes =
        flatten_and_translate_to_midi_notes(measure, tuning, base_frequency, pb_range);

    auto const notes = flatten_notes(measure.cell.element);

    assert(ranges.size() == midi_notes.size());
    assert(ranges.size() == notes.size());

    // create a midi event for each entry
    for (auto i = 0u; i < ranges.size(); ++i)
    {
        auto const [begin, end] = ranges[i];
        auto const [note, pitch_bend] = midi_notes[i];
        std::uint8_t const velocity =
            static_cast<std::uint8_t>(notes[i].velocity * 127);

        midi_events.emplace_back(PitchBend{pitch_bend}, begin);
        midi_events.emplace_back(NoteOn{note, velocity}, begin);
        midi_events.emplace_back(NoteOff{note}, end);
    }

    return midi_events;
}

} // namespace sequence::midi