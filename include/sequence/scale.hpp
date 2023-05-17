#ifndef SEQUENCE_SCALE_HPP
#define SEQUENCE_SCALE_HPP
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace sequence
{

/**
 * @brief A musical scale defined by intervals from the base note in cents.
 *
 * @details The first entry is always the base note, 1/1
 *
 * @example
 * 12-tone equal temperament:
 * {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100}
 */
using Scale = std::vector<float>;

/**
 * @brief Generates a scale from a Scala file.
 *
 * @details The Scala file format is described here:
 * http://www.huygens-fokker.org/scala/scl_format.html
 *
 * @param scala_file The Scala file to generate the scale from.
 * @return Scale - The generated scale.
 *
 * @throws std::runtime_error
 */
[[nodiscard]] inline auto from_scala(std::string_view scala_file) -> Scale
{
    auto file = std::ifstream{scala_file.data()};

    if (!file)
    {
        throw std::runtime_error("Could not open file: " + std::string{scala_file});
    }

    auto scale = Scale{0.f}; // The first note of 1/1 or 0.0 cents is implicit in file.

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
                scale.push_back(1200.f * std::log2(numerator / denominator));
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
            scale.push_back(cents);
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
                scale.push_back(1200.f * std::log2(static_cast<float>(numerator)));
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

    return scale;
}

} // namespace sequence
#endif // SEQUENCE_SCALE_HPP