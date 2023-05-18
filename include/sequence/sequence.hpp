#ifndef SEQUENCE_SEQUENCE_HPP
#define SEQUENCE_SEQUENCE_HPP
#include <variant>
#include <vector>

namespace sequence
{

using Cell = std::variant<struct NoteOn, struct NoteOff, struct Rest, struct Sequence>;

struct NoteOn
{
    int interval;   // 0 is scale's base note, 1 is scale's second note, etc.
    float velocity; // 0.0 to 1.0, percentage of max velocity
    float delay;    // 0.0 to 1.0, percentage of cell length to wait before triggering
    float gate;     // 0.0 to 1.0, percentage of note length to play
};

struct NoteOff
{
};

struct Rest
{
};

struct Sequence
{
    std::vector<Cell> cells;
};

/**
 * @brief Compares two NoteOns for equality.
 */
[[nodiscard]] constexpr auto operator==(NoteOn const &lhs, NoteOn const &rhs) -> bool
{
    return lhs.interval == rhs.interval && lhs.velocity == rhs.velocity &&
           lhs.delay == rhs.delay && lhs.gate == rhs.gate;
}

/**
 * @brief Compares two NoteOns for inequality.
 */
[[nodiscard]] constexpr auto operator!=(NoteOn const &lhs, NoteOn const &rhs) -> bool
{
    return !(lhs == rhs);
}

// /**
//  * @brief Compares two Sequences for equality.
//  */
// [[nodiscard]] constexpr auto operator==(Sequence const &lhs, Sequence const &rhs)
//     -> bool
// {
//     return lhs.cells == rhs.cells;
// }

// /**
//  * @brief Compares two Sequences for inequality.
//  */
// [[nodiscard]] constexpr auto operator!=(Sequence const &lhs, Sequence const &rhs)
//     -> bool
// {
//     return !(lhs == rhs);
// }

} // namespace sequence
#endif // SEQUENCE_SEQUENCE_HPP