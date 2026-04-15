#pragma once

#include <cmath>
#include <variant>
#include <vector>

namespace sequence
{

struct Note
{
    int pitch = 0;         // 0 is tunings's base note, 1 is tunings's second note, etc.
    float velocity = 0.7f; // 0.0 to 1.0, percentage of max velocity
    float delay = 0.f;     // 0.0 to 1.0, percentage of cell length to wait
    float gate = 1.f;      // 0.0 to 1.0, percentage of note length to play
};

struct Cell;

struct Sequence
{
    std::vector<Cell> cells;

    bool operator==(Sequence const &) const = default;
    bool operator!=(Sequence const &) const = default;
};

using MusicElement = std::variant<Note, Sequence>;

struct Cell
{
    std::vector<MusicElement> elements;
    float weight = 1.f; // Defines length, in relation to sibling Cells
};

#include <cmath>

/**
 * @brief Compares two Notes for equality.
 */
[[nodiscard]]
constexpr auto operator==(Note const &lhs, Note const &rhs) -> bool
{
    constexpr float tolerance = 0.0001f; // set a small tolerance value
    return lhs.pitch == rhs.pitch &&
           std::fabs(lhs.velocity - rhs.velocity) < tolerance &&
           std::fabs(lhs.delay - rhs.delay) < tolerance &&
           std::fabs(lhs.gate - rhs.gate) < tolerance;
}

/**
 * @brief Compares two Notes for inequality.
 */
[[nodiscard]]
constexpr auto operator!=(Note const &lhs, Note const &rhs) -> bool
{
    return !(lhs == rhs);
}

[[nodiscard]]
constexpr auto operator==(Cell const &lhs, Cell const &rhs) -> bool
{
    return lhs.elements == rhs.elements && std::fabs(lhs.weight - rhs.weight) < 0.0001f;
}

[[nodiscard]]
constexpr auto operator!=(Cell const &lhs, Cell const &rhs) -> bool
{
    return !(lhs == rhs);
}

} // namespace sequence
