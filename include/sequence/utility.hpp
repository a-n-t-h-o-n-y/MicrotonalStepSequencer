#ifndef SEQUENCE_UTILITY_HPP
#define SEQUENCE_UTILITY_HPP

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

} // namespace sequence::utility
#endif // SEQUENCE_UTILITY_HPP