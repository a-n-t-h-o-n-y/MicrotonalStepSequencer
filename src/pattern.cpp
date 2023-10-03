#include <sequence/pattern.hpp>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

/**
 * @brief Get the pattern string from a given command string.
 *
 * This will not check if the pattern is valid and does not extract digits if they are
 * attached to a command name. This will not work properly if there is no input after
 * the pattern.
 *
 * @param x The command string to extract the pattern from.
 * @return std::string The extracted pattern.
 */
[[nodiscard]] auto get_pattern_str(std::string const &x) -> std::string
{
    auto const it = std::find_if_not(std::cbegin(x), std::cend(x), [](char c) {
        return std::isdigit(c) || c == ' ' || c == '+';
    });

    auto result = std::string{std::cbegin(x), it};

    auto const r_space_it = std::find(std::crbegin(result), std::crend(result), ' ');
    if (r_space_it != std::crend(result))
    {
        result.erase((r_space_it + 1).base(), std::cend(result));
    }

    return result;
}

} // namespace

namespace sequence
{

auto contains_valid_pattern(std::string const &input) -> bool
{
    auto const pattern_str = get_pattern_str(input);
    auto stream = std::istringstream{pattern_str};
    auto token = std::string{};

    try
    {
        // Check for Offset
        if (!pattern_str.empty() && pattern_str[0] == '+')
        {
            stream >> token;
            auto processed = std::size_t{};
            auto const num = std::stoi(token, &processed);
            if (processed != token.size() || num < 0)
            {
                return false;
            }
        }

        // Body of Pattern
        while (stream >> token)
        {
            auto processed = std::size_t{};
            auto const num = std::stoi(token, &processed);
            if (processed != token.size() || num <= 0)
            {
                return false;
            }
        }
    }
    catch (std::invalid_argument const &)
    {
        return false;
    }
    catch (std::out_of_range const &)
    {
        return false;
    }

    return true;
}

auto parse_pattern(std::string const &input) -> Pattern
{
    if (!contains_valid_pattern(input))
    {
        throw std::invalid_argument{"Does not contain a valid Pattern: " + input};
    }

    auto const pattern_str = get_pattern_str(input);
    auto iss = std::istringstream{pattern_str};
    auto token = std::string{};
    auto pattern = Pattern{0, {}};

    bool is_offset_checked = false;
    while (iss >> token)
    {
        auto const value = std::stoi(token);

        if (!is_offset_checked && token[0] == '+')
        {
            pattern.offset = value;
            is_offset_checked = true;
        }
        else
        {
            is_offset_checked = true;
            pattern.intervals.push_back(value);
        }
    }
    if (pattern.intervals.empty())
    {
        pattern.intervals = {1};
    }
    return pattern;
}

auto pop_pattern_chars(std::string const &x) -> std::string
{
    auto const pattern_str = get_pattern_str(x);
    return std::string{std::cbegin(x) + pattern_str.size(), std::cend(x)};
};

} // namespace sequence
