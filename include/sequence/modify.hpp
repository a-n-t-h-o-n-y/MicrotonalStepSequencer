#ifndef SEQUENCE_MODIFY_HPP
#define SEQUENCE_MODIFY_HPP
#include <cstddef>
#include <vector>

#include <sequence/pattern.hpp>
#include <sequence/sequence.hpp>
#include <sequence/utility.hpp>

namespace sequence::modify
{

/**
 * @brief Randomize the note pitches in an existing Cell.
 *
 * This overwrites existings notes, it does not apply an offset to the existing
 * notes.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param pattern The Pattern to apply across Sequences.
 * @param min The minimum note value to use, inclusive.
 * @param max The maximum note value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max.
 */
[[nodiscard]] auto randomize_pitch(Cell cell, Pattern const &pattern, int min,
                                   int max) -> Cell;

/**
 * @brief Randomize the note velocities in a given Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param pattern The Pattern to apply across Sequences.
 * @param min The minimum velocity value to use, inclusive.
 * @param max The maximum velocity value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] auto randomize_velocity(Cell cell, Pattern const &pattern, float min,
                                      float max) -> Cell;

/**
 * @brief Randomize the note delays in a Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param pattern The Pattern to apply across Sequences.
 * @param min The minimum delay value to use, inclusive.
 * @param max The maximum delay value to use, inclusive.
 * @return Cell The randomized Cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] auto randomize_delay(Cell cell, Pattern const &pattern, float min,
                                   float max) -> Cell;

/**
 * @brief Randomize the note gates of a Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param pattern The Pattern to apply across Sequences.
 * @param min The minimum gate value to use, inclusive.
 * @param max The maximum gate value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] auto randomize_gate(Cell cell, Pattern const &pattern, float min,
                                  float max) -> Cell;

/**
 * @brief Shift note pitch by a constant amount.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to shift the notes of.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Cell The pitch shifted Cell.
 */
[[nodiscard]] auto shift_pitch(Cell cell, Pattern const &pattern, int amount) -> Cell;

/**
 * @brief Shift note velocities by a constant amount.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to shift the velocities of.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Cell The velocity shifted Cell.
 */
[[nodiscard]] auto shift_velocity(Cell cell, Pattern const &pattern,
                                  float amount) -> Cell;

/**
 * @brief Shift note delays by a constant amount.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to shift the delays of.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Cell The delay shifted Cell.
 */
[[nodiscard]] auto shift_delay(Cell cell, Pattern const &pattern, float amount) -> Cell;

/**
 * @brief Shift note gates by a constant amount.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to shift the gates of.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Cell The gate shifted Cell.
 */
[[nodiscard]] auto shift_gate(Cell cell, Pattern const &pattern, float amount) -> Cell;

/**
 * @brief Set the pitch of a note to a constant value.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to set the pitch of.
 * @param pattern The Pattern to apply across Sequences.
 * @param pitch The pitch to set the note(s) to.
 * @return Cell The pitch set Cell.
 */
[[nodiscard]] auto set_pitch(Cell cell, Pattern const &pattern, int pitch) -> Cell;

/**
 * @brief Set the octave of a note to a constant value.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to set the octave of.
 * @param pattern The Pattern to apply across Sequences.
 * @param octave The octave to set the note to.
 * @param tuning_length The length of the tuning.
 * @return Cell The octave set Cell.
 */
[[nodiscard]] auto set_octave(Cell cell, Pattern const &pattern, int octave,
                              std::size_t tuning_length) -> Cell;

/**
 * @brief Set the velocity of a note to a constant value.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to set the velocity of.
 * @param pattern The Pattern to apply across Sequences.
 * @param velocity The velocity to set the note to.
 * @return Cell The velocity set Cell.
 */
[[nodiscard]] auto set_velocity(Cell cell, Pattern const &pattern,
                                float velocity) -> Cell;

/**
 * @brief Set the delay of a note to a constant value.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to set the delay of.
 * @param pattern The Pattern to apply across Sequences.
 * @param delay The delay to set the note to.
 * @return Cell The delay set Cell.
 */
[[nodiscard]] auto set_delay(Cell cell, Pattern const &pattern, float delay) -> Cell;

/**
 * @brief Set the gate of a note to a constant value.
 *
 * If cell is a Sequence, this will recurse into child Sequences. Clamps the
 * result to the range [0, 1].
 *
 * @param cell The Cell to set the gate of.
 * @param pattern The Pattern to apply across Sequences.
 * @param gate The gate to set the note to.
 * @return Cell The gate set Cell.
 */
[[nodiscard]] auto set_gate(Cell cell, Pattern const &pattern, float gate) -> Cell;

/**
 * @brief Rotate the note order in an existing sequence so the `amount`th note is the
 * new first note. Negative amounts count from the end of the Sequence.
 *
 * If amount is greater than the size of the Sequence, it shifts by the remainder.
 *
 * No-op for Note and Rest.
 *
 * @param Cell The Cell to rotate.
 * @param amount The amount to rotate by, can be positive, negative or zero. Positive is
 * a right shift and negative a left shift, zero returns original Sequence.
 * @return Cell The rotated Cell.
 */
[[nodiscard]] auto rotate(Cell cell, int amount) -> Cell;

/**
 * @brief Set delay of even index notes to zero and odd index notes to `amount`.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to modify.
 * @param amount The amount to set the note delays to.
 * @param is_odd If true, odd index notes are set to `amount` and even index notes are
 * set to zero.
 * @return Cell The cell with the note delays set.
 *
 * @throws std::invalid_argument If amount is less than zero or greater than one.
 */
[[nodiscard]] auto swing(Cell cell, float amount, bool is_odd = false) -> Cell;

/**
 * @brief Quantize the notes in an existing sequence zero delay and full gate.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to quantize.
 * @param pattern The Pattern to apply across Sequences.
 * @return Cell The quantized Cell.
 */
[[nodiscard]] auto quantize(Cell cell, Pattern const &pattern) -> Cell;

/**
 * @brief Swap the notes in a Cell around a center note.

 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to mirror.
 * @param pattern The Pattern to apply across Sequences.
 * @param center_note The note to mirror around.
 * @return Cell The mirrored Cell.
 */
[[nodiscard]] auto mirror(Cell cell, Pattern const &pattern, int center_note) -> Cell;

/**
 * @brief Reverse the notes in a Cell
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to reverse.
 * @return Cell The reversed Cell.
 */
[[nodiscard]] auto reverse(Cell cell) -> Cell;

/**
 * @brief Create `count` copies of the provided Cell as a new Cell.
 *
 * The returned Cell is always a Sequence. This is also known as 'split'. This
 * does not recurse into child Sequences.
 *
 * @param cell The Cell to duplicate.
 * @param count The number of copies to make, if zero, returns an empty Sequence.
 * @return Cell The repeated sequence as a Cell.
 */
[[nodiscard]] auto repeat(Cell const &cell, std::size_t count) -> Cell;

/**
 * @brief Repeat each note in the sequence `amount` times in a row.
 *
 * The returned Cell is always a Sequence. This recurses into child Sequences.
 *
 * @param cell The Cell to repeat.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The number of times to repeat each Cell, if zero, returns an empty
 * Sequence.
 * @return Cell The stretched Sequence.
 */
[[nodiscard]] auto stretch(Cell const &cell, Pattern const &pattern,
                           std::size_t amount) -> Cell;

/**
 * @brief Compress a Cell by keeping every Cell in the Pattern and skipping the rest.
 *
 * This does not recurse into child Sequences. No-op for Notes and Rests.
 *
 * @param seq The Cell to compress.
 * @param pattern The Pattern the defines the cells to keep.
 * @return Cell The compressed Sequence.
 */
[[nodiscard]] auto compress(Cell const &cell, Pattern const &pattern) -> Cell;

/**
 * @brief Extract a single note from a Sequence Cell.
 *
 * No-op for Notes and Rests.
 *
 * @param cell The Cell to extract from.
 * @param index The index of the note to extract.
 * @return Cell The extracted note.
 *
 * @throws std::invalid_argument If index is out of bounds.
 */
[[nodiscard]] auto extract(Cell const &cell, std::size_t index) -> Cell;

/**
 * @brief Get the first Note or Rest in a Cell.
 *
 * If the Cell is an empty Sequence, this will return a Rest. No-op for Notes
 * and Rests.
 *
 * @param cell The Cell to get the first Note or Rest from.
 * @return Cell The first Note or Rest.
 *
 * @throws std::invalid_argument If the Cell is an empty Sequence.
 */
[[nodiscard]] auto first(Cell const &cell) -> Cell;

/**
 * @brief Get the last Note or Rest in a Cell.
 *
 * If the Cell is an empty Sequence, this will return a Rest. No-op for Notes
 * and Rests.
 *
 * @param cell The Cell to get the last Note or Rest from.
 * @return Cell The last Note or Rest.
 *
 * @throws std::invalid_argument If the Cell is an empty Sequence.
 */
[[nodiscard]] auto last(Cell const &cell) -> Cell;

/**
 * @brief Shuffle the notes in a Cell into a random order.
 *
 * Each note remains in the Sequence, but in a (potentially) new order. Sub-Sequences
 * are recursed into but notes are only shuffled within their own Sequence.
 *
 * @param cell The Cell to shuffle.
 * @return Cell The shuffled Cell.
 */
[[nodiscard]] auto shuffle(Cell cell) -> Cell;

/**
 * @brief Concatenate two Cells into a new Cell.
 *
 * @param cell_a The first Cell.
 * @param cell_b The second Cell.
 * @return Cell The concatenated Cell as a Sequence.
 */
[[nodiscard]] auto concat(Cell const &cell_a, Cell const &cell_b) -> Cell;

/**
 * @brief Merge two Cell into a new Cell by interleaving their notes
 *
 * If one Sequence is shorter than the other, it is repeated to be the same length as
 * the longer Sequence. If either Sequence is empty, the other Sequence is returned.
 * Does not recurse into subsequences.
 *
 * @param cell_a The first Cell.
 * @param cell_b The second Cell.
 * @return Cell The merged Cell as a Sequence.
 */
[[nodiscard]] auto merge(Cell const &cell_a, Cell const &cell_b) -> Cell;

/**
 * @brief Divide a Sequence into two Sequences at the provided index.
 *
 * If index is out of bounds, the second Sequence will be empty.
 *
 * @param seq The sequence to divide.
 * @param index The index to split at, this element is included in the second Sequence.
 * @return Cell The split Cell as a new Sequence.
 */
[[nodiscard]] auto divide(Cell const &cell, std::size_t index) -> Cell;

/**
 * @brief Create a Note.
 *
 * @param pitch The pitch of the note.
 * @param velocity The velocity of the note.
 * @param delay The delay of the note.
 * @param gate The gate of the note.
 * @return Cell The created Note.
 * @throws std::invalid_argument If velocity, delay or gate are not in the range [0, 1].
 */
[[nodiscard]] auto note(int pitch, float velocity, float delay, float gate) -> Cell;

/**
 * @brief Create a Rest.
 *
 * @return Cell The created Rest.
 */
[[nodiscard]] auto rest() -> Cell;

/**
 * @brief Create a Sequence from a vector of Cells.
 *
 * @param cells The Cells to create the Sequence from.
 * @return Cell The created Sequence.
 */
[[nodiscard]] auto sequence(std::vector<Cell> cells) -> Cell;

/**
 * @brief Flip a Rest to a Note and a Note to a Rest.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to flip.
 * @param n The Note to flip to.
 * @return Cell The flipped Cell.
 */
[[nodiscard]] auto flip(Cell cell, Pattern const &pattern, Note n = Note{}) -> Cell;

/**
 * @brief Humanize the note velocities in a Cell.
 *
 * @details If cell is a Sequence, this will recurse into child Sequences. This applies
 * a random offset between [0, amount] to each note's velocity.
 *
 * @param cell The Cell to humanize.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to humanize by.
 * @return Cell The humanized Cell.
 */
[[nodiscard]] auto humanize_velocity(Cell cell, Pattern const &pattern,
                                     float amount) -> Cell;

/**
 * @brief Humanize the note delays in a Cell.
 *
 * @details If cell is a Sequence, this will recurse into child Sequences. This applies
 * a random offset between [0, amount] to each note's delay.
 *
 * @param cell The Cell to humanize.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to humanize by.
 * @return Cell The humanized Cell.
 */
[[nodiscard]] auto humanize_delay(Cell cell, Pattern const &pattern,
                                  float amount) -> Cell;

/**
 * @brief Humanize the note gates in a Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences. This applies a
 * random offset between [0, amount] to each note's gate.
 *
 * @param cell The Cell to humanize.
 * @param pattern The Pattern to apply across Sequences.
 * @param amount The amount to humanize by.
 * @return Cell The humanized Cell.
 */
[[nodiscard]] auto humanize_gate(Cell cell, Pattern const &pattern,
                                 float amount) -> Cell;

/**
 * @brief Fill a Sequence with Notes.
 *
 * If cell is a Sequence, this will recurse into child Sequences. This fills the
 * Sequence with Notes, using the Pattern to define which Cells are turned into Notes.
 *
 * @param cell The Cell to fill.
 * @param pattern The Pattern to apply across Sequences.
 * @param note The Note to fill with.
 */
[[nodiscard]] auto notes_fill(Cell cell, Pattern const &pattern, Note note) -> Cell;

/**
 * @brief Fill a Sequence with Rests.
 *
 * If cell is a Sequence, this will recurse into child Sequences. This fills the
 * Sequence with Rests, using the Pattern to define which Cells are turned into Rests.
 *
 * @param cell The Cell to fill.
 * @param pattern The Pattern to apply across Sequences.
 * @return Cell The filled Cell.
 */
[[nodiscard]] auto rests_fill(Cell cell, Pattern const &pattern) -> Cell;

} // namespace sequence::modify
#endif // SEQUENCE_MODIFY_HPP