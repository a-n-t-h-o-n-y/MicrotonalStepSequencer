#ifndef SEQUENCE_SEQUENCE_HPP
#define SEQUENCE_SEQUENCE_HPP
#include <variant>
#include <vector>

namespace sequence
{

using Cell = std::variant<struct Note, struct Rest, struct Sequence>;

struct Note
{
    int interval = 0;      // 0 is tunings's base note, 1 is tunings's second note, etc.
    float velocity = 0.7f; // 0.0 to 1.0, percentage of max velocity
    float delay = 0.f;     // 0.0 to 1.0, percentage of cell length to wait
    float gate = 1.f;      // 0.0 to 1.0, percentage of note length to play
};

struct Rest
{
};

struct Sequence
{
    std::vector<Cell> cells;
};

/**
 * @brief Compares two Notes for equality.
 */
[[nodiscard]] constexpr auto operator==(Note const &lhs, Note const &rhs) -> bool
{
    return lhs.interval == rhs.interval && lhs.velocity == rhs.velocity &&
           lhs.delay == rhs.delay && lhs.gate == rhs.gate;
}

/**
 * @brief Compares two Notes for inequality.
 */
[[nodiscard]] constexpr auto operator!=(Note const &lhs, Note const &rhs) -> bool
{
    return !(lhs == rhs);
}

[[nodiscard]] constexpr auto operator==(Rest const &, Rest const &) -> bool
{
    return true;
}

[[nodiscard]] constexpr auto operator!=(Rest const &lhs, Rest const &rhs) -> bool
{
    return !(lhs == rhs);
}

[[nodiscard]] constexpr auto operator==(Sequence const &lhs, Sequence const &rhs)
    -> bool
{
    return lhs.cells == rhs.cells;
}

[[nodiscard]] constexpr auto operator!=(Sequence const &lhs, Sequence const &rhs)
    -> bool
{
    return !(lhs == rhs);
}

[[nodiscard]] constexpr auto operator==(Cell const &lhs, Cell const &rhs) -> bool
{
    return std::visit(
        [](auto const &lhs, auto const &rhs) -> bool {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<T, U>)
            {
                return lhs == rhs;
            }
            else
            {
                return false;
            }
        },
        lhs, rhs);
}

} // namespace sequence
#endif // SEQUENCE_SEQUENCE_HPP