#ifndef SEQUENCE_TIME_SIGNATURE_HPP
#define SEQUENCE_TIME_SIGNATURE_HPP

#include <ostream>

namespace sequence
{

/**
 * @brief A time signature.
 *
 * The time signature is a fraction that represents the number of beats per measure and
 * the note value that represents one beat.
 *
 * For example, 4/4 time has 4 beats per measure and a quarter note represents one beat.
 * 3/8 time has 3 beats per measure and an eighth note represents one beat.
 */
struct TimeSignature
{
    unsigned numerator;   // Number of beats per measure
    unsigned denominator; // Beat value
};

inline auto operator<<(std::ostream &os, TimeSignature const &ts) -> std::ostream &
{
    return os << ts.numerator << "/" << ts.denominator;
}

} // namespace sequence
#endif // SEQUENCE_TIME_SIGNATURE_HPP