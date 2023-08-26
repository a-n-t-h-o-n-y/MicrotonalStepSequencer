#ifndef SEQUENCE_MEASURE_HPP
#define SEQUENCE_MEASURE_HPP
#include <cstdint>
#include <utility>
#include <vector>

#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{

/**
 * @brief A measure.
 *
 * A measure is a sequence of notes that are played in a specific time signature.
 * A Cell is used for uniformity and any Cell type can be used, not just a Sequence.
 */
struct Measure
{
    Cell cell;
    TimeSignature time_signature;
};

using Phrase = std::vector<sequence::Measure>;

/**
 * @brief Generates a measure of Rest cells.
 *
 * @param time_signature The time signature of the measure.
 * @param cell_resolution The number of cells per beat, where beat is determined by the
 * time_signature denominator. Must be >= 1.
 * @return Measure
 *
 * @throws std::invalid_argument if resolution < 1.
 */
[[nodiscard]] auto create_measure(TimeSignature const &time_signature,
                                  std::size_t cell_resolution = 1) -> Measure;

/**
 * @brief Calculates the number of samples in the given measure.
 *
 * @param measure The measure to calculate the number of samples for.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @return std::uint32_t - The number of samples in the measure.
 */
[[nodiscard]] auto samples_count(Measure const &measure, std::uint32_t sample_rate,
                                 float bpm) -> std::uint32_t;

/**
 * @brief Calculates the number of samples in the given phrase.
 *
 * @param phrase The phrase to calculate the number of samples for.
 * @return std::uint32_t - The number of samples in the phrase.
 */
[[nodiscard]] auto samples_count(Phrase const &phrase, std::uint32_t sample_rate,
                                 float bpm) -> std::uint32_t;

} // namespace sequence
#endif // SEQUENCE_MEASURE_HPP