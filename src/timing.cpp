#include <sequence/timing.hpp>

#include <cstdint>
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
    if (bpm <= 0.f)
    {
        throw std::invalid_argument("bpm must be greater than 0");
    }

    auto const samples_per_beat = static_cast<float>(sample_rate) * 60.f / bpm;
    auto const beats_per_bar = (static_cast<float>(time_signature.numerator) /
                                static_cast<float>(time_signature.denominator)) *
                               4.f;
    return static_cast<std::uint32_t>(samples_per_beat * beats_per_bar);
}

} // namespace sequence
