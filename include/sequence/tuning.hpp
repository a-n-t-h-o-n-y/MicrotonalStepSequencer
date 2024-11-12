#ifndef SEQUENCE_TUNING_HPP
#define SEQUENCE_TUNING_HPP

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace sequence
{

/**
 * @brief A musical tuning defined by intervals (cents) from the base note and an octave
 * interval.
 *
 * @example
 * 12-tone equal temperament:
 * {
 *   intervals: {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
 *   octave: 1200
 * }
 */
struct Tuning
{
    using Interval_t = float;

    std::vector<Interval_t> intervals;
    Interval_t octave;
    std::string description;

    [[nodiscard]] auto operator==(Tuning const &other) const -> bool
    {
        // std::ranges::equal_to used to avoid float comparison warning.
        return intervals == other.intervals &&
               std::ranges::equal_to{}(octave, other.octave);
    }
    [[nodiscard]] auto operator!=(Tuning const &) const -> bool = default;
};

/**
 * @brief Generates a Tuning from a Scala file.
 *
 * @details The Scala file format is described here:
 * http://www.huygens-fokker.org/scala/scl_format.html
 *
 * @param scala_file The Scala file to generate the Tuning from.
 * @return Tuning - The generated tuning.
 * @throws std::runtime_error
 */
[[nodiscard]] auto from_scala(std::filesystem::path const &scala_file) -> Tuning;

/**
 * @brief Generates a Scala file from a Tuning.
 *
 * @details The Scala file format is described here:
 * http://www.huygens-fokker.org/scala/scl_format.html
 *
 * @param tuning The Tuning to generate the Scala file from.
 * @param file The file to write the Scala file to.
 * @throws std::runtime_error if the file could not be opened.
 */
void to_scala(Tuning const &tuning, std::filesystem::path const &file);

} // namespace sequence
#endif // SEQUENCE_TUNING_HPP