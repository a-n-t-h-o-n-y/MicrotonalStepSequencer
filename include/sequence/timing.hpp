#pragma once

#include <cstddef>
#include <cstdint>

#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{

/**
 * @brief Calculates the number of samples in the given top-level cell.
 *
 * @param cell The cell whose top-level duration is being calculated.
 * @param time_signature The top-level time signature for the cell.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @return std::uint32_t - The number of samples in the cell.
 *
 * @throws std::invalid_argument if \p time_signature.denominator is zero, if
 * \p sample_rate is zero, or if \p bpm is not greater than zero.
 */
[[nodiscard]] auto samples_count(Cell const &cell, TimeSignature const &time_signature,
                                 std::uint32_t sample_rate, float bpm) -> std::uint32_t;

} // namespace sequence
