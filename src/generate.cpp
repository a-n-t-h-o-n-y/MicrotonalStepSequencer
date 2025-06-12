#include <sequence/generate.hpp>

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

#include <sequence/sequence.hpp>

namespace sequence::generate
{

auto empty(std::size_t size) -> Sequence
{
    return Sequence{std::vector<Cell>(size, {Rest{}})};
}

auto full(std::size_t size, Note note) -> Sequence
{
    return Sequence{std::vector<Cell>(size, {note})};
}

auto interval(std::size_t size, std::size_t interval, std::size_t offset, Note note)
    -> Sequence
{
    auto seq = Sequence{std::vector<Cell>(size, {Rest{}})};

    for (auto i = offset; i < size; i += interval)
    {
        seq.cells[i].element = note;
    }

    return seq;
}

auto random(std::size_t size, float density, Note note) -> Sequence
{
    if (density < 0.f || density > 1.f)
    {
        throw std::out_of_range("Invalid arguments");
    }

    auto seq = Sequence{std::vector<Cell>(size, {Rest{}})};

    auto rd = std::random_device{};
    auto gen = std::mt19937{rd()};
    auto dist = std::uniform_real_distribution<float>{0.f, 1.f};

    for (auto &cell : seq.cells)
    {
        if (dist(gen) < density)
        {
            cell.element = note;
        }
    }

    return seq;
}

} // namespace sequence::generate