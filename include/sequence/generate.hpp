#ifndef SEQUENCE_GENERATE_HPP
#define SEQUENCE_GENERATE_HPP
#include <algorithm>
#include <cstdint>
#include <sequence/sequence.hpp>

namespace sequence
{
/**
 * @brief Generates a sequence of cells, each of which is a rest.
 *
 * @param cell_count The number of cells to generate.
 * @return Sequence The generated sequence.
 */
[[nodiscard]] inline auto generate_empty_sequence(std::uint8_t cell_count) -> Sequence
{
    return Sequence{std::vector(cell_count, Cell{Rest{}})};
}

// note gen
// cell type gen
// combine the above two to gen any seq
// each has a random impl that are combined into a randomize generator
// only issue then is you only ask for notes when note on is generated
// and for actual random impl it'll only generate rests and note ons

// generate_full_sequence
// generate_skip_sequence
// randomize (existing)

// auto generate_random_sequence(std::uint8_t cell_count) -> Sequence
// {
//     auto s = generate_empty_sequence(cell_count);
//     std::generate(s.cells.begin(), s.cells.end(), []()
//                   { return Cell{Rest{}}; });
//     return s;
// }
} // namespace sequence

#endif // SEQUENCE_GENERATE_HPP