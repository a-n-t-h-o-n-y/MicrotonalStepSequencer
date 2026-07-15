#pragma once

#include <cmath>
#include <map>
#include <variant>
#include <vector>

namespace sequence
{

using MidiControllerNumber = int;
using MidiCcValues = std::map<MidiControllerNumber, float>;

inline constexpr auto MAX_MIDI_CONTROLLER_NUMBER = 127;
inline constexpr auto MIDI_CC_NEUTRAL_VALUE = 0.5f;

struct Note
{
    int pitch = 0;         // 0 is tunings's base note, 1 is tunings's second note, etc.
    float velocity = 0.7f; // 0.0 to 1.0, percentage of max velocity
    float delay = 0.f;     // 0.0 to 1.0, percentage of cell length to wait
    float gate = 1.f;      // 0.0 to 1.0, percentage of note length to play
    MidiCcValues midi_cc{}; // Per-note normalized MIDI controller values
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
inline auto operator==(Note const &lhs, Note const &rhs) -> bool
{
    constexpr float tolerance = 0.0001f; // set a small tolerance value
    if (lhs.pitch != rhs.pitch || std::fabs(lhs.velocity - rhs.velocity) >= tolerance ||
        std::fabs(lhs.delay - rhs.delay) >= tolerance ||
        std::fabs(lhs.gate - rhs.gate) >= tolerance ||
        lhs.midi_cc.size() != rhs.midi_cc.size())
    {
        return false;
    }

    auto lhs_cc = lhs.midi_cc.begin();
    auto rhs_cc = rhs.midi_cc.begin();
    for (; lhs_cc != lhs.midi_cc.end(); ++lhs_cc, ++rhs_cc)
    {
        if (lhs_cc->first != rhs_cc->first ||
            std::fabs(lhs_cc->second - rhs_cc->second) >= tolerance)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compares two Notes for inequality.
 */
[[nodiscard]]
inline auto operator!=(Note const &lhs, Note const &rhs) -> bool
{
    return !(lhs == rhs);
}

[[nodiscard]]
inline auto operator==(Cell const &lhs, Cell const &rhs) -> bool
{
    return lhs.elements == rhs.elements && std::fabs(lhs.weight - rhs.weight) < 0.0001f;
}

[[nodiscard]]
inline auto operator!=(Cell const &lhs, Cell const &rhs) -> bool
{
    return !(lhs == rhs);
}

} // namespace sequence
