#ifndef SEQUENCE_TUNING_HPP
#define SEQUENCE_TUNING_HPP
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
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
};

/**
 * @brief Generates a Tuning from a Scala file.
 *
 * @details The Scala file format is described here:
 * http://www.huygens-fokker.org/scala/scl_format.html
 *
 * @param scala_file The Scala file to generate the Tuning from.
 * @return Tuning - The generated tuning.
 *
 * @throws std::runtime_error
 */
[[nodiscard]] inline auto from_scala(std::filesystem::path const &scala_file) -> Tuning
{
    auto file = std::ifstream{scala_file};

    if (!file)
    {
        throw std::runtime_error("Could not open file: " + scala_file.string());
    }

    auto tuning = Tuning{};
    auto &intervals = tuning.intervals;
    intervals.push_back(0.f); // The first interval is always 0

    auto line = std::string{};
    bool description_line_skipped = false;
    bool notes_line_read = false;
    int note_count = 0;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '!') // Ignore comments and empty lines
        {
            continue;
        }

        if (!description_line_skipped)
        {
            description_line_skipped = true; // Skip the first non-comment line
            continue;
        }

        if (!notes_line_read)
        {
            auto ss = std::stringstream{line};
            ss >> note_count;
            if (!ss || note_count < 0)
            {
                throw std::runtime_error("Invalid number of notes.");
            }
            notes_line_read = true;
            continue;
        }

        if (note_count == 0) // All notes have been read
        {
            break;
        }

        // Parse the pitch value
        if (line.find('/') != std::string::npos) // Ratio
        {
            auto pos = line.find('/');
            try
            {
                auto numerator = std::stof(line.substr(0, pos));
                auto denominator = std::stof(line.substr(pos + 1));
                if (numerator <= 0.f || denominator <= 0.f)
                {
                    throw std::runtime_error(
                        "Negative ratio or zero denominator is not allowed.");
                }
                // Ratio to Cents
                intervals.push_back(1200.f * std::log2(numerator / denominator));
            }
            catch (...)
            {
                throw std::runtime_error("Invalid ratio: " + line);
            }
        }
        else if (line.find('.') != std::string::npos) // Cents
        {
            auto ss = std::stringstream{line};
            auto cents = 0.f;
            ss >> cents;
            if (!ss)
            {
                throw std::runtime_error("Invalid cents value.");
            }
            intervals.push_back(cents);
        }
        else // Integer ratio
        {
            try
            {
                auto numerator = std::stoll(line);
                if (numerator <= 0)
                {
                    throw std::runtime_error("Negative ratio is not allowed.");
                }
                intervals.push_back(1200.f * std::log2(static_cast<float>(numerator)));
            }
            catch (...)
            {
                throw std::runtime_error("Invalid ratio.");
            }
        }

        --note_count;
    }

    if (note_count > 0)
    {
        throw std::runtime_error("Not enough pitch values in the file.");
    }

    tuning.octave = intervals.back();
    intervals.pop_back();

    return tuning;
}

} // namespace sequence
#endif // SEQUENCE_TUNING_HPP