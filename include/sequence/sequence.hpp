#ifndef SEQUENCE_SEQUENCE_HPP
#define SEQUENCE_SEQUENCE_HPP
#include <cmath>
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
#include <cmath>

[[nodiscard]] constexpr auto operator==(Note const &lhs, Note const &rhs) -> bool
{
    constexpr float tolerance = 0.0001f; // set a small tolerance value
    return lhs.interval == rhs.interval &&
           std::fabs(lhs.velocity - rhs.velocity) < tolerance &&
           std::fabs(lhs.delay - rhs.delay) < tolerance &&
           std::fabs(lhs.gate - rhs.gate) < tolerance;
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
        [](auto const &l, auto const &r) -> bool {
            using T = std::decay_t<decltype(l)>;
            using U = std::decay_t<decltype(r)>;
            if constexpr (std::is_same_v<T, U>)
            {
                return l == r;
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