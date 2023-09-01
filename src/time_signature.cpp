#include <sequence/time_signature.hpp>

#include <iostream>
#include <stdexcept>

namespace sequence
{

auto operator<<(std::ostream &os, TimeSignature const &ts) -> std::ostream &
{
    return os << ts.numerator << "/" << ts.denominator;
}

auto operator>>(std::istream &is, TimeSignature &ts) -> std::istream &
{
    auto numerator = unsigned{};
    auto denominator = unsigned{1};

    if (!(is >> numerator))
    {
        throw std::invalid_argument(
            "Invalid time signature format: Couldn't parse numerator.");
    }

    if (is.peek() == '/') // denominator is optional, defaults to one
    {
        is.ignore(); // Skip the '/' character
        if (!(is >> denominator))
        {
            throw std::invalid_argument(
                "Invalid time signature format: Couldn't parse denominator.");
        }
    }

    ts = TimeSignature{numerator, denominator};

    return is;
}
} // namespace sequence