#include <sequence/pattern.hpp>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sequence
{

auto parse_pattern(std::string const &input) -> std::optional<Pattern>
{
    auto const pattern_str = extract_pattern_str(input);
    if (pattern_str.empty() || pattern_str.back() != ' ')
    {
        return std::nullopt;
    }

    auto iss = std::istringstream{pattern_str};
    auto token = std::string{};
    auto pattern = Pattern{};

    bool is_offset_checked = false;
    while (iss >> token)
    {
        try
        {
            auto const value = std::stoull(token);
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
        catch (std::invalid_argument const &e)
        {
            throw std::invalid_argument("Invalid pattern string: '" + pattern_str +
                                        "'");
        }
    }
    if (pattern.intervals.empty())
    {
        pattern.intervals = {1};
    }
    if (std::ranges::find(pattern.intervals, 0) != std::cend(pattern.intervals))
    {
        throw std::invalid_argument("Invalid pattern string: '" + pattern_str +
                                    "'. Zero interval not allowed.");
    }
    return pattern;
}

auto strip_pattern_chars(std::string const &x) -> std::string
{

    if (auto const pattern_str = extract_pattern_str(x); pattern_str.empty())
    {
        return x;
    }
    auto const it = std::find_if_not(std::cbegin(x), std::cend(x), [](char c) {
        return std::isdigit(c) || c == ' ' || c == '+';
    });
    return std::string{it, std::cend(x)};
};

auto extract_pattern_str(std::string const &x) -> std::string
{
    auto const it = std::find_if_not(std::cbegin(x), std::cend(x), [](char c) {
        return std::isdigit(c) || c == ' ' || c == '+';
    });
    auto const result = std::string{std::cbegin(x), it};
    if (!result.empty() && result.back() == ' ')
    {
        return result;
    }
    else
    {
        return "";
    }
}

} // namespace sequence