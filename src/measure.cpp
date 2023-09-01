#include <sequence/measure.hpp>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <sequence/generate.hpp>
#include <sequence/sequence.hpp>
#include <sequence/time_signature.hpp>

namespace sequence
{

auto create_measure(TimeSignature const &time_signature, std::size_t cell_resolution)
    -> Measure
{
    if (cell_resolution < 1)
    {
        throw std::invalid_argument("invalid cell_resolution, must be >= 1");
    }

    return Measure{
        generate::empty(time_signature.numerator * cell_resolution),
        time_signature,
    };
}

auto samples_count(Measure const &measure, std::uint32_t sample_rate, float bpm)
    -> std::uint32_t
{
    auto const &time_sig = measure.time_signature;

    auto const samples_per_beat = static_cast<float>(sample_rate) * 60.f / bpm;
    // 4 beats per whole note; num/den == whole note count;
    auto const beats_per_measure = (static_cast<float>(time_sig.numerator) /
                                    static_cast<float>(time_sig.denominator)) *
                                   4.f;
    return static_cast<std::uint32_t>(samples_per_beat * beats_per_measure);
}

auto samples_count(Phrase const &phrase, std::uint32_t sample_rate, float bpm)
    -> std::uint32_t
{
    auto output = std::uint32_t{0};
    for (auto const &measure : phrase)
    {
        output += samples_count(measure, sample_rate, bpm);
    }
    return output;
}

} // namespace sequence