#ifndef SEQUENCE_MEASURE_HPP
#define SEQUENCE_MEASURE_HPP
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
                                         int cell_resolution = 1) -> Measure
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

} // namespace sequence
#endif // SEQUENCE_MEASURE_HPP