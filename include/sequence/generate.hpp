#ifndef SEQUENCE_GENERATE_HPP
#define SEQUENCE_GENERATE_HPP
#include <cstddef>

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
[[nodiscard]] auto empty(std::size_t size) -> Sequence;

/**
 * @brief Generates a sequence of duplicated Note cells.
 *
 * @param size The number of cells to generate.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0.
 */
[[nodiscard]] auto full(std::size_t size, Note note = {0, 0.8f, 0.f, 1.f}) -> Sequence;

/**
 * @brief Generates a sequence of cells, each of which is either a Notee or Rest in an
 * evenly spaced pattern.
 *
 * @param size The number of cells to generate.
 * @param interval The number of steps taken before placing the next Note. A value of
 * 1 will place a Note in every cell, a value of 2 will place a Note in every other
 * cell, etc.
 * @param offset The offset in cells of the first Note.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0, interval < 1, or offset < 0.
 */
[[nodiscard]] auto interval(std::size_t size, std::size_t interval,
                            std::size_t offset = 0, Note note = {0, 0.8f, 0.f, 1.f})
    -> Sequence;

/**
 * @brief Generates a sequence of Note and Rest cells in a random pattern.
 *
 * @param size The number of cells to generate.
 * @param density The percentage of cells that will be Note cells.
 * @param note The note to duplicate.
 * @return Sequence
 *
 * @throws std::out_of_range if size < 0, density < 0, or density > 1.
 */
[[nodiscard]] auto random(std::size_t size, float density = 0.6f,
                          Note note = {0, 0.8f, 0.f, 1.f}) -> Sequence;

} // namespace sequence::generate
#endif // SEQUENCE_GENERATE_HPP