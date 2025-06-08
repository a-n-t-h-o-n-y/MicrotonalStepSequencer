#ifndef SEQUENCY_MIDI_HPP
#define SEQUENCY_MIDI_HPP
#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

#include <sequence/measure.hpp>
#include <sequence/sequence.hpp>
#include <sequence/tuning.hpp>

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
 * @param pitch The pitch value to use, this is the value from Note.pitch, not the midi
 * note number.
 * @param tuning The tuning to use for the note.
 * @param tuning_base The base note of the tuning, as a floating point value. This is a
 * midi note value but allows for fractional notes that coorespond to any value
 * inbetween midi notes.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return MicrotonalNote
 * @throws std::invalid_argument if the tuning is empty.
 */
[[nodiscard]] auto create_midi_note(int pitch, Tuning const &tuning, float tuning_base,
                                    float pb_range) -> MicrotonalNote;

[[nodiscard]] auto create_midi_note_visitor(Cell const &cell, Tuning const &tuning,
                                            float tuning_base, float pb_range)
    -> std::vector<MicrotonalNote>;

/**
 * @brief Calculates the MIDI notes for a measure.
 *
 * @param measure The measure to calculate the MIDI notes for.
 * @param base_frequency The base frequency of the tuning. Defaults to 440 Hz. This is
 * what note.pitch 0 will be.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return std::vector<MicrotonalNote>
 */
[[nodiscard]] auto flatten_and_translate_to_midi_notes(Measure const &measure,
                                                       Tuning const &tuning,
                                                       float base_frequency = 440.f,
                                                       float pb_range = 48.f)
    -> std::vector<MicrotonalNote>;

[[nodiscard]] auto flatten_and_translate_to_midi_notes(Phrase const &phrase,
                                                       Tuning const &tuning,
                                                       float base_frequency = 440.f,
                                                       float pb_range = 48.f)
    -> std::vector<MicrotonalNote>;

/**
 * @brief Flattens any Cell type into a vector of notes.
 *
 * Only the Notes are returned, including Notes in subsequences, rests are ignored.
 *
 * @param cell The Cell to flatten.
 * @return std::vector<Note>
 */
[[nodiscard]] auto flatten_notes(Cell const &cell) -> std::vector<Note>;

[[nodiscard]] auto flatten_notes(Phrase const &phrase) -> std::vector<Note>;

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
[[nodiscard]] auto note_sample_infos(Cell const &cell, std::uint32_t total_samples,
                                     float offset = 0.f) -> std::vector<SampleRange>;

/**
 * @brief Flattens a measure into a vector of SampleRange objects.
 *
 * Only the Notes are returned, including Notes in subsequences, rests do not have an
 * entry. The sample counts are calculated from the measure's sample rate and bpm, and
 * note delay and gate are used, as well as the Measure's time signature and resolution.
 *
 * @param measure The measure to flatten.
 * @param sample_rate
 * @param bpm
 * @return std::vector<SampleRange>
 */
[[nodiscard]] auto flatten_and_translate_to_sample_infos(Measure const &measure,
                                                         std::uint32_t sample_rate,
                                                         float bpm)
    -> std::vector<SampleRange>;

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
 * Flattens a measure into a vector of MIDI events.
 *
 * @details Returns a sequence of MIDI events of type NoteOn, NoteOff, or PitchBend,
 * paired with the sample offset of the event.
 *
 * @param measure The measure to flatten.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @param tuning The tuning to use for the MIDI notes.
 * @param base_frequency The base frequency of the tuning.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return EventTimeline
 */
[[nodiscard]] auto translate_to_midi_timeline(Measure const &measure,
                                              std::uint32_t sample_rate, float bpm,
                                              Tuning const &tuning,
                                              float base_frequency, float pb_range)
    -> EventTimeline;

} // namespace sequence::midi
#endif // SEQUENCY_MIDI_HPP