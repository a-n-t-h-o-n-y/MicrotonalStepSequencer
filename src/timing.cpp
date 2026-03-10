#include <sequence/timing.hpp>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <sequence/generate.hpp>
#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{

auto create_cell(TimeSignature const &time_signature, std::size_t cell_resolution) -> Cell
{
    if (cell_resolution < 1)
    {
        throw std::invalid_argument("invalid cell_resolution, must be >= 1");
    }

    return Cell{generate::empty(time_signature.numerator * cell_resolution)};
}

auto samples_count(Cell const &, TimeSignature const &time_signature,
                   std::uint32_t sample_rate, float bpm) -> std::uint32_t
{
    if (time_signature.denominator == 0)
    {
        throw std::invalid_argument("time_signature denominator must be greater than 0");
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
