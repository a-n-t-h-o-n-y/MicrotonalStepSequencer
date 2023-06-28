#ifndef SEQUENCE_MEASURE_HPP
#define SEQUENCE_MEASURE_HPP
#include <cstdint>

#include <sequence/generate.hpp>
#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{

/**
 * @brief A measure.
 *
 * A measure is a sequence of notes that are played in a specific time signature.
 */
struct Measure
{
    Sequence sequence;
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
[[nodiscard]] inline auto create_measure(TimeSignature const &time_signature,
                                         std::size_t cell_resolution = 1) -> Measure
{
    if (cell_resolution < 1)
    {
        throw std::invalid_argument("Invalid cell_resolution, must be >= 1");
    }

    return Measure{
        generate::empty(time_signature.numerator * cell_resolution),
        time_signature,
    };
}

/**
 * @brief Calculates the number of samples in the given measure.
 *
 * @param measure The measure to calculate the number of samples for.
 * @param sample_rate The sample rate of the audio.
 * @param bpm The beats per minute of the audio.
 * @return std::uint32_t - The number of samples in the measure.
 */
[[nodiscard]] inline auto samples_count(Measure const &measure,
                                        std::uint32_t sample_rate, float bpm)
    -> std::uint32_t
{
    auto const &time_sig = measure.time_signature;

    auto const samples_per_beat = static_cast<float>(sample_rate) * 60.f / bpm;
    auto const beats_per_measure = static_cast<float>(time_sig.numerator);
    return static_cast<std::uint32_t>(samples_per_beat * beats_per_measure);
}

/**
 * @brief Calculates the number of samples in the given phrase.
 *
 * @param phrase The phrase to calculate the number of samples for.
 * @return std::uint32_t - The number of samples in the phrase.
 */
[[nodiscard]] inline auto samples_count(Phrase const &phrase, std::uint32_t sample_rate,
                                        float bpm) -> std::uint32_t
{
    auto output = std::uint32_t{0};
    for (auto const &measure : phrase)
    {
        output += samples_count(measure, sample_rate, bpm);
    }
    return output;
}

} // namespace sequence
#endif // SEQUENCE_MEASURE_HPP