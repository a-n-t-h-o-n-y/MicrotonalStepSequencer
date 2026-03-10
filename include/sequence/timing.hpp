#ifndef SEQUENCE_TIMING_HPP
#define SEQUENCE_TIMING_HPP

#include <cstddef>
#include <cstdint>

#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{
/**
 * @brief Generates a top-level cell containing a sequence of rest cells.
 *
 * @param time_signature The time signature used to derive the number of top-level cells.
 * @param cell_resolution The number of cells per beat, where beat is determined by the
 * time_signature denominator. Must be >= 1.
 * @return Cell
 *
 * @throws std::invalid_argument if resolution < 1.
 */
[[nodiscard]] auto create_cell(TimeSignature const &time_signature,
                               std::size_t cell_resolution = 1) -> Cell;

/**
 * @brief Calculates the number of samples in the given top-level cell.
 *
 * @param cell The cell whose top-level duration is being calculated.
 * @param time_signature The top-level time signature for the cell.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @return std::uint32_t - The number of samples in the cell.
 */
[[nodiscard]] auto samples_count(Cell const &cell,
                                 TimeSignature const &time_signature,
                                 std::uint32_t sample_rate, float bpm)
    -> std::uint32_t;

} // namespace sequence

#endif // SEQUENCE_TIMING_HPP
