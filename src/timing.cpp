#include <sequence/timing.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include <sequence/time_signature.hpp>

namespace sequence
{

auto samples_count(TimeSignature const &time_signature,
                   std::uint32_t sample_rate,
                   float bpm) -> std::uint32_t
{
    if (time_signature.denominator == 0)
    {
        throw std::invalid_argument(
            "time_signature denominator must be greater than 0");
    }
    if (sample_rate == 0)
    {
        throw std::invalid_argument("sample_rate must be greater than 0");
    }
    if (!std::isfinite(bpm) || bpm <= 0.f)
    {
        throw std::invalid_argument("bpm must be greater than 0");
    }

    auto const samples_per_beat = static_cast<double>(sample_rate) * 60. / bpm;
    auto const beats_per_bar = (static_cast<double>(time_signature.numerator) /
                                static_cast<double>(time_signature.denominator)) *
                               4.;
    auto const result = samples_per_beat * beats_per_bar;
    if (!std::isfinite(result) ||
        result > static_cast<double>(std::numeric_limits<std::uint32_t>::max()))
    {
        throw std::overflow_error("sample count exceeds uint32_t range");
    }
    return static_cast<std::uint32_t>(result);
}

} // namespace sequence
