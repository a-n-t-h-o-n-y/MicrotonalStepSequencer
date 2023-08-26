#include <sequence/midi.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
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

auto create_midi_note(int interval, Tuning const &tuning, float tuning_base)
    -> MicrotonalNote
{
    if (tuning.intervals.empty())
    {
        throw std::invalid_argument("Tuning must not be empty");
    }

    auto const fractional_note = tuning_base + [&] {
        constexpr auto semitone_cents = 100.f;
        auto const length = (int)tuning.intervals.size();

        auto const octave_offset = (float)(interval / length) * tuning.octave;
        auto const interval_offset = [&] {
            auto const interval_index = interval % length;
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
    return MicrotonalNote{static_cast<std::uint8_t>(integral),
                          static_cast<std::uint16_t>((1 + fractional) * 8192)};
}

auto create_midi_note_visitor(Cell const &cell, Tuning const &tuning, float tuning_base)
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

auto flatten_and_translate_to_midi_notes(Measure const &measure, Tuning const &tuning,
                                         float base_frequency)
    -> std::vector<MicrotonalNote>
{
    constexpr auto a4 = 69;       // MIDI note number for A4
    constexpr auto a4_hz = 440.f; // Frequency of A4

    auto const base_midi_note =
        12.f * std::log2(base_frequency / a4_hz) + static_cast<float>(a4);

    return create_midi_note_visitor(measure.cell, tuning, base_midi_note);
}

auto flatten_and_translate_to_midi_notes(Phrase const &phrase, Tuning const &tuning,
                                         float base_frequency)
    -> std::vector<MicrotonalNote>
{
    auto notes = std::vector<MicrotonalNote>{};

    for (auto const &measure : phrase)
    {
        auto const results =
            flatten_and_translate_to_midi_notes(measure, tuning, base_frequency);
        std::copy(std::cbegin(results), std::cend(results), std::back_inserter(notes));
    }

    return notes;
}

auto flatten_notes(Cell const &cell) -> std::vector<Note>
{
    auto notes = std::vector<Note>{};
    std::visit(utility::overload{
                   [&](Note const &note) { notes.push_back(note); },
                   [](Rest const &) {},
                   [&](Sequence const &seq) {
                       for (auto const &child : seq.cells)
                       {
                           auto const child_notes = flatten_notes(child);
                           std::copy(std::cbegin(child_notes), std::cend(child_notes),
                                     std::back_inserter(notes));
                       }
                   },
               },
               cell);
    return notes;
}

auto flatten_notes(Phrase const &phrase) -> std::vector<Note>
{
    auto notes = std::vector<Note>{};

    for (auto const &measure : phrase)
    {
        auto const measure_notes = flatten_notes(measure.cell);
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
                float const samples_per_cell =
                    (float)(total_samples) / (float)seq.cells.size();
                auto infos = std::vector<SampleRange>{};
                for (auto const &c : seq.cells)
                {
                    auto const result = note_sample_infos(
                        c, static_cast<std::uint32_t>(samples_per_cell), offset);
                    std::copy(std::cbegin(result), std::cend(result),
                              std::back_inserter(infos));
                    offset += samples_per_cell;
                }
                return infos;
            },
        },
        cell);
}

auto flatten_and_translate_to_sample_infos(Phrase const &phrase,
                                           std::uint32_t sample_rate, float bpm)
    -> std::vector<SampleRange>
{
    auto infos = std::vector<SampleRange>{};

    auto sample_offset = 0.f;

    for (auto const &measure : phrase)
    {
        auto const samples_per_measure = samples_count(measure, sample_rate, bpm);
        auto const result =
            note_sample_infos(measure.cell, samples_per_measure, sample_offset);
        std::copy(std::cbegin(result), std::cend(result), std::back_inserter(infos));
        sample_offset += samples_per_measure;
    }
    return infos;
}

auto translate_to_midi_timeline(Phrase const &phrase, std::uint32_t sample_rate,
                                float bpm, Tuning const &tuning, float base_frequency)
    -> EventTimeline
{
    auto midi_events = EventTimeline{};

    auto const ranges = flatten_and_translate_to_sample_infos(phrase, sample_rate, bpm);

    auto const midi_notes =
        flatten_and_translate_to_midi_notes(phrase, tuning, base_frequency);

    auto const notes = flatten_notes(phrase);

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