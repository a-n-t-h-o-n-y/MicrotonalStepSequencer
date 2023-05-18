#ifndef SEQUENCE_GENERATE_HPP
#define SEQUENCE_GENERATE_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>

#include <sequence/sequence.hpp>

namespace sequence::generate
{

/**
 * @brief Generates a sequence of cells, each of which is a rest.
 *
 * @param size The number of cells to generate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0.
 */
[[nodiscard]] inline auto empty(int size) -> Sequence
{
    return Sequence{std::vector<Cell>(size, Rest{})};
}

/**
 * @brief Generates a sequence of duplicated NoteOn cells.
 *
 * @param size The number of cells to generate.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0.
 */
[[nodiscard]] inline auto full(int size, NoteOn note = {0, 0.8f, 0.f, 1.f}) -> Sequence
{
    return Sequence{std::vector<Cell>(size, note)};
}

/**
 * @brief Generates a sequence of cells, each of which is either a NoteOne or Rest in an
 * evenly spaced pattern.
 *
 * @param size The number of cells to generate.
 * @param interval The number of steps taken before placing the next NoteOn. A value of
 * 1 will place a NoteOn in every cell, a value of 2 will place a NoteOn in every other
 * cell, etc.
 * @param offset The offset in cells of the first NoteOn.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0, interval < 1, or offset < 0.
 */
[[nodiscard]] inline auto interval(int size, int interval, int offset = 0,
                                   NoteOn note = {0, 0.8f, 0.f, 1.f}) -> Sequence
{
    if (size < 0 || interval < 1 || offset < 0)
    {
        throw std::out_of_range("Invalid arguments");
    }

    auto seq = Sequence{std::vector<Cell>(size, Rest{})};

    for (auto i = offset; i < size; i += interval)
    {
        seq.cells[i] = note;
    }

    return seq;
}

/**
 * @brief Generates a sequence of NoteOn and Rest cells in a random pattern.
 *
 * @param size The number of cells to generate.
 * @param density The percentage of cells that will be NoteOn cells.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0, density < 0, or density > 1.
 */
[[nodiscard]] inline auto random(int size, float density = 0.6f,
                                 NoteOn note = {0, 0.8f, 0.f, 1.f}) -> Sequence
{
    if (size < 0 || density < 0.f || density > 1.f)
    {
        throw std::out_of_range("Invalid arguments");
    }

    auto seq = Sequence{std::vector<Cell>(size, Rest{})};

    auto rd = std::random_device{};
    auto gen = std::mt19937{rd()};
    auto dist = std::uniform_real_distribution<float>{0.f, 1.f};

    for (int i = 0; i < size; ++i)
    {
        if (dist(gen) < density)
        {
            seq.cells[i] = note;
        }
    }

    return seq;
}

} // namespace sequence::generate
#endif // SEQUENCE_GENERATE_HPP