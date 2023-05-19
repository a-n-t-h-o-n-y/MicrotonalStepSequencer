#ifndef SEQUENCE_UTILITY_HPP
#define SEQUENCE_UTILITY_HPP
#include <algorithm>

namespace sequence::utility
{

/**
 * @brief A helper class for creating overloaded lambdas.
 *
 * @tparam Ts The types of the lambdas to overload.
 */
template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

/**
 * @brief A helper function for visiting cells in a sequence.
 *
 * @tparam Sequence The type of the sequence.
 * @tparam Visitor The type of the visitor.
 * @param seq The sequence to visit.
 * @param visitor The visitor to use.
 */
auto visit_cells(Sequence &seq, auto &&visitor) -> void
{
    std::ranges::for_each(seq.cells,
                          [&visitor](Cell &cell) { std::visit(visitor, cell); });
}

auto visit_cells(Sequence const &seq, auto &&visitor) -> void
{
    std::ranges::for_each(seq.cells,
                          [&visitor](Cell const &cell) { std::visit(visitor, cell); });
}

} // namespace sequence::utility
#endif // SEQUENCE_UTILITY_HPP