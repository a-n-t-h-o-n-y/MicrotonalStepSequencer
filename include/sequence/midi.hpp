#ifndef SEQUENCE_MIDI_HPP
#define SEQUENCE_MIDI_HPP
#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

#include <sequence/sequence.hpp>
#include <sequence/timing.hpp>
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
[[nodiscard]] auto create_midi_note(int pitch, Tuning const &tuning, float tuning_base,
                                    float pb_range) -> MicrotonalNote;

/**
 * @brief Calculates the MIDI notes for a cell.
 *
 * @param cell The cell to calculate the MIDI notes for.
 * @param tuning The tuning to use for note translation.
 * @param base_frequency The base frequency of the tuning. Defaults to 440 Hz. This is
 * what note.pitch 0 will be.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return std::vector<MicrotonalNote>
 *
 * @throws std::invalid_argument if \p base_frequency is not greater than zero, if
 * \p pb_range is not greater than zero, or if \p tuning is empty.
 */
[[nodiscard]] auto flatten_and_translate_to_midi_notes(Cell const &cell,
                                                       Tuning const &tuning,
                                                       float base_frequency = 440.f,
                                                       float pb_range = 48.f)
    -> std::vector<MicrotonalNote>;

/**
 * @brief Flattens any Cell type into a vector of notes.
 *
 * Only the Notes are returned, including Notes in subsequences, rests are ignored.
 *
 * @param element The MusicElement to flatten.
 * @return std::vector<Note>
 */
[[nodiscard]] auto flatten_notes(MusicElement const &element) -> std::vector<Note>;
[[nodiscard]] auto flatten_notes(Cell const &cell) -> std::vector<Note>;

/// Stores the starting and ending audio sample for a note.
struct SampleRange
{
    std::uint32_t begin;
    std::uint32_t end;

    auto operator==(SampleRange const &) const -> bool = default;
    auto operator!=(SampleRange const &) const -> bool = default;
};

/**
 * @brief Calculates the sample count and offset for each note in a sequence.
 *
 * This will recurse into subsequences. Only returns sample counts for Notes, Rests are
 * ignored.
 *
 * @param cell The Cell to calculate sample ranges for.
 * @param total_samples The total number of samples for the sequence.
 * @param offset The offset of the sequence in samples. Float to avoid rounding errors.
 * @return std::vector<SampleRange>
 *
 * @throws std::invalid_argument if any visited Sequence has a total child weight that
 * is not greater than zero.
 */
[[nodiscard]] auto note_sample_infos(Cell const &cell, std::uint32_t total_samples,
                                     float offset = 0.f) -> std::vector<SampleRange>;

/**
 * @brief Flattens a timed cell into a vector of SampleRange objects.
 *
 * Only the Notes are returned, including Notes in subsequences, rests do not have an
 * entry. The sample counts are calculated from the cell's sample rate and bpm, and
 * note delay and gate are used, as well as the provided time signature.
 *
 * @param cell The cell to flatten.
 * @param time_signature The top-level time signature for the cell.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @return std::vector<SampleRange>
 *
 * @throws std::invalid_argument if the timing inputs are invalid or if any visited
 * Sequence has a total child weight that is not greater than zero.
 */
[[nodiscard]] auto flatten_and_translate_to_sample_infos(Cell const &cell,
                                                         TimeSignature const &time_signature,
                                                         std::uint32_t sample_rate,
                                                         float bpm)
    -> std::vector<SampleRange>;

struct NoteOn
{
    std::uint8_t note;
    std::uint8_t velocity;

    auto operator==(NoteOn const &) const -> bool = default;
    auto operator!=(NoteOn const &) const -> bool = default;
};

struct NoteOff
{
    std::uint8_t note;

    auto operator==(NoteOff const &) const -> bool = default;
    auto operator!=(NoteOff const &) const -> bool = default;
};

struct PitchBend
{
    std::uint16_t value;

    auto operator==(PitchBend const &) const -> bool = default;
    auto operator!=(PitchBend const &) const -> bool = default;
};

using Event = std::variant<NoteOn, NoteOff, PitchBend>;

/// A MIDI Event paired with a sample offset.
using EventTimeline = std::vector<std::pair<Event, std::uint32_t>>;

/**
 * Flattens a timed cell into a vector of MIDI events.
 *
 * @details Returns a sequence of MIDI events of type NoteOn, NoteOff, or PitchBend,
 * paired with the sample offset of the event.
 *
 * @param cell The cell to flatten.
 * @param time_signature The top-level time signature for the cell.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @param tuning The tuning to use for the MIDI notes.
 * @param base_frequency The base frequency of the tuning.
 * @param pb_range The amount of note pitch bend range expected by the midi receiver.
 * @return EventTimeline
 *
 * @throws std::invalid_argument if the timing inputs are invalid, if \p tuning is
 * empty, if \p base_frequency is not greater than zero, if \p pb_range is not greater
 * than zero, or if any visited Sequence has a total child weight that is not greater
 * than zero.
 */
[[nodiscard]] auto translate_to_midi_timeline(Cell const &cell,
                                              TimeSignature const &time_signature,
                                              std::uint32_t sample_rate, float bpm,
                                              Tuning const &tuning,
                                              float base_frequency, float pb_range)
    -> EventTimeline;

} // namespace sequence::midi
#endif // SEQUENCE_MIDI_HPP
