#ifndef SEQUENCE_MODIFY_HPP
#define SEQUENCE_MODIFY_HPP
#include <algorithm>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <variant>

#include <sequence/sequence.hpp>
#include <sequence/utility.hpp>

namespace sequence::modify
{

/**
 * @brief Randomize the note intervals in an existing sequence.
 *
 * This recurses on child Sequences.
 *
 * @param seq The sequence to randomize.
 * @param min The minimum note value to use, inclusive.
 * @param max The maximum note value to use, inclusive.
 * @return Sequence The randomized sequence.
 *
 * @throws std::invalid_argument If min is greater than max.
 */
[[nodiscard]] inline auto randomize_intervals(Sequence const &seq, int min, int max)
    -> Sequence
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }

    auto randomized_seq = seq;

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_int_distribution{min, max};

    using namespace utility;

    visit_cells(randomized_seq,
                overload{
                    [&](Note &note) { note.interval = dis(gen); },
                    [](Rest) {},
                    [&](Sequence &seq) { seq = randomize_intervals(seq, min, max); },
                });

    return randomized_seq;
}

/**
 * @brief Randomize the note velocities in an existing sequence.
 *
 * This recurses on child Sequences.
 *
 * @param seq The sequence to randomize.
 * @param min The minimum velocity value to use, inclusive.
 * @param max The maximum velocity value to use, inclusive.
 * @return Sequence The randomized sequence.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_velocity(Sequence const &seq, float min, float max)
    -> Sequence
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto randomized_seq = seq;

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    using namespace utility;

    visit_cells(randomized_seq,
                overload{
                    [&](Note &note) { note.velocity = dis(gen); },
                    [](Rest) {},
                    [&](Sequence &seq) { seq = randomize_velocity(seq, min, max); },
                });

    return randomized_seq;
}

/**
 * @brief Randomize the note delays in an existing sequence.
 *
 * This recurses on child Sequences.
 *
 * @param seq The sequence to randomize.
 * @param min The minimum delay value to use, inclusive.
 * @param max The maximum delay value to use, inclusive.
 * @return Sequence The randomized sequence.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_delay(Sequence const &seq, float min, float max)
    -> Sequence
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto randomized_seq = seq;

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    using namespace utility;

    visit_cells(randomized_seq,
                overload{
                    [&](Note &note) { note.delay = dis(gen); },
                    [](Rest) {},
                    [&](Sequence &seq) { seq = randomize_delay(seq, min, max); },
                });

    return randomized_seq;
}

/**
 * @brief Randomize the note gates in an existing sequence.
 *
 * This recurses on child Sequences.
 *
 * @param seq The sequence to randomize.
 * @param min The minimum gate value to use, inclusive.
 * @param max The maximum gate value to use, inclusive.
 * @return Sequence The randomized sequence.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_gate(Sequence const &seq, float min, float max)
    -> Sequence
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto randomized_seq = seq;

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    using namespace utility;

    visit_cells(randomized_seq,
                overload{
                    [&](Note &note) { note.gate = dis(gen); },
                    [](Rest) {},
                    [&](Sequence &seq) { seq = randomize_gate(seq, min, max); },
                });

    return randomized_seq;
}

/**
 * @brief Shift the note pitches in an existing sequence by a constant amount.
 *
 * This recurses on child Sequences.
 *
 * @param seq The sequence to shift the notes of.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Sequence The shifted sequence.
 */
[[nodiscard]] inline auto shift_pitch(Sequence const &seq, int amount) -> Sequence
{
    auto shifted_seq = seq;

    using namespace utility;

    visit_cells(shifted_seq, overload{
                                 [&](Note &note) { note.interval += amount; },
                                 [](Rest) {},
                                 [&](Sequence &seq) { seq = shift_pitch(seq, amount); },
                             });

    return shifted_seq;
}

/**
 * @brief Rotate the note order in an existing sequence so the `amount`th note is the
 * new first note. Negative amounts count from the end of the Sequence.
 *
 * If amount is greater than the size of the Sequence, it shifts by the remainder.
 *
 * @param seq The sequence to rotate.
 * @param amount The amount to rotate by, can be positive, negative or zero. Positive is
 * a right shift and negative a left shift, zero returns original Sequence.
 * @return Sequence The rotated sequence.
 */
[[nodiscard]] inline auto rotate(Sequence const &seq, int amount) -> Sequence
{
    if (seq.cells.empty())
    {
        return seq;
    }
    auto rotated_seq = seq;
    amount = amount % static_cast<int>(rotated_seq.cells.size());
    if (amount < 0)
    {
        amount += static_cast<int>(rotated_seq.cells.size());
    }
    std::rotate(rotated_seq.cells.begin(), rotated_seq.cells.begin() + amount,
                rotated_seq.cells.end());
    return rotated_seq;
}

/**
 * @brief Set the note delay of pairs of notes so that the first note has zero delay and
 * the second note has a delay cooresponding to the provided amount.
 *
 * Pairs are determined by the Sequence they are a part of, child Sequences are
 * independent of parent Sequenes. This function does not recurse on child Sequences.
 *
 * @param seq The sequence to set the note delays of.
 * @param amount The amount to set the note delays to.
 * @return Sequence The sequence with the note delays set.
 *
 * @throws std::invalid_argument If amount is less than zero or greater than one.
 */
[[nodiscard]] inline auto swing(Sequence const &seq, float amount) -> Sequence
{
    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0, 1]");
    }

    auto swung_seq = seq;

    for (auto i = 0u; i < swung_seq.cells.size(); ++i)
    {
        using namespace utility;
        auto const is_odd = (i % 2) == 1;
        std::visit(overload{
                       [&](Note &note) { note.delay = is_odd ? amount : 0.f; },
                       [](Rest) {},
                       [&](Sequence &sub_seq) { sub_seq = swing(sub_seq, amount); },
                   },
                   swung_seq.cells[i]);
    }

    return swung_seq;
}

/**
 * @brief Quantize the notes in an existing sequence zero delay and full gate.
 *
 * @param seq The sequence to quantize.
 * @return Sequence The quantized sequence.
 */
[[nodiscard]] inline auto quantize(Sequence const &seq) -> Sequence
{
    using namespace utility;

    auto quantized_seq = seq;

    visit_cells(quantized_seq,
                overload{
                    [&](Note &note) {
                        note.delay = 0.f;
                        note.gate = 1.f;
                    },
                    [](Rest) {},
                    [&](Sequence &sub_seq) { sub_seq = quantize(sub_seq); },
                });

    return quantized_seq;
}

/**
 * @brief Swap the notes in an existing sequence around a center note.
 *
 * @param seq The sequence to mirror.
 * @param center_note The note to mirror around.
 * @return Sequence The mirrored sequence.
 */
[[nodiscard]] inline auto mirror(Sequence const &seq, int center_note) -> Sequence
{
    auto mirrored_seq = seq;

    using namespace utility;

    visit_cells(mirrored_seq,
                overload{
                    [&](Note &note) {
                        auto const diff = center_note - note.interval;
                        note.interval = center_note + diff;
                    },
                    [](Rest) {},
                    [&](Sequence &sub_seq) { sub_seq = mirror(sub_seq, center_note); },
                });

    return mirrored_seq;
}

/**
 * @brief Reverse the notes in an existing sequence.
 *
 * @param seq The sequence to reverse.
 * @return Sequence The reversed sequence.
 */
[[nodiscard]] inline auto reverse(Sequence const &seq) -> Sequence
{
    auto reversed_seq = seq;

    std::ranges::reverse(reversed_seq.cells);

    for (auto &cell : reversed_seq.cells)
    {
        using namespace utility;
        std::visit(overload{
                       [](Note &) {},
                       [](Rest &) {},
                       [&](Sequence &sub_seq) { sub_seq = reverse(sub_seq); },
                   },
                   cell);
    }

    return reversed_seq;
}

/**
 * @brief Create `count` copies of the provided Sequence as a new Sequence.
 *
 * @param seq The sequence to duplicate.
 * @param count The number of copies to make, if zero, returns an empty Sequence.
 * @return Sequence The repeated sequence.
 *
 * @throws std::invalid_argument If count is less than zero.
 */
[[nodiscard]] inline auto repeat(Sequence const &seq, int count) -> Sequence
{
    if (count < 0)
    {
        throw std::invalid_argument("count must be greater than or equal to zero");
    }

    auto repeated_seq = Sequence{};
    repeated_seq.cells.reserve(seq.cells.size() * count);

    for (auto i = 0; i < count; ++i)
    {
        std::copy(std::cbegin(seq.cells), std::cend(seq.cells),
                  std::back_inserter(repeated_seq.cells));
    }

    return repeated_seq;
}

/**
 * @brief Repeat each note in the sequence `amount` times in a row.
 *
 * This does not recurse into child Sequences.
 *
 * @param seq The sequence to repeat.
 * @param amount The number of times to repeat each note, if zero, returns an empty
 * Sequence.
 * @return Sequence The stretched sequence.
 *
 * @throws std::invalid_argument If amount is less than zero.
 */
[[nodiscard]] inline auto stretch(Sequence const &seq, int amount) -> Sequence
{
    if (amount < 0)
    {
        throw std::invalid_argument("count must be greater than or equal to zero");
    }

    auto stretched_seq = Sequence{};
    stretched_seq.cells.reserve(seq.cells.size() * amount);

    for (auto const &cell : seq.cells)
    {
        for (auto i = 0; i < amount; ++i)
        {
            stretched_seq.cells.push_back(cell);
        }
    }

    return stretched_seq;
}

/**
 * @brief Compress the sequence by keeping every `amount` note and removing the rest.
 *
 * This does not recurse into child Sequences.
 *
 * @param seq The sequence to compress.
 * @param amount The amount of compression, meaning the first note is kept, then
 * `amount` minus one notes are skipped, then the next note is kept, etc.
 * @return Sequence The compressed sequence.
 *
 * @throws std::invalid_argument If amount is less than one.
 */
[[nodiscard]] inline auto compress(Sequence const &seq, int amount) -> Sequence
{
    if (amount < 1)
    {
        throw std::invalid_argument("count must be greater than or equal to one");
    }

    auto compressed_seq = Sequence{};
    compressed_seq.cells.reserve(seq.cells.size() / amount);

    std::copy_if(
        std::cbegin(seq.cells), std::cend(seq.cells),
        std::back_inserter(compressed_seq.cells),
        [amount, i = 0u](auto const &) mutable { return (i++ % amount) == 0; });

    return compressed_seq;
}

/**
 * @brief Shuffle the notes in an existing sequence into a random order.
 *
 * Each note remains in the Sequence, but in a (potentially) new order. Sub-Sequences
 * are recursed into but notes are only shuffled within their own Sequence.
 *
 * @param seq The sequence to shuffle.
 * @return Sequence The shuffled sequence
 */
[[nodiscard]] inline auto shuffle(Sequence const &seq) -> Sequence
{
    // shuffle top level, then visit each and if is sequence, recurse
    auto shuffled_seq = seq;

    std::ranges::shuffle(shuffled_seq.cells, std::mt19937{std::random_device{}()});

    using namespace utility;

    visit_cells(shuffled_seq,
                overload{
                    [](Note &) {},
                    [](Rest &) {},
                    [&](Sequence &sub_seq) { sub_seq = shuffle(sub_seq); },
                });

    return shuffled_seq;
}

/**
 * @brief Concatenate two Sequences into a new Sequence.
 *
 * @param seq_a The first sequence.
 * @param seq_b The second sequence.
 * @return Sequence The concatenated sequence.
 */
[[nodiscard]] inline auto concat(Sequence const &seq_a, Sequence const &seq_b)
    -> Sequence
{
    auto concat_seq = seq_a;

    concat_seq.cells.insert(std::cend(concat_seq.cells), std::cbegin(seq_b.cells),
                            std::cend(seq_b.cells));

    return concat_seq;
}

/**
 * @brief Merge two Sequences into a new Sequence by interleaving their notes
 *
 * If one Sequence is shorter than the other, it is repeated to be the same length as
 * the longer Sequence. If either Sequence is empty, the other Sequence is returned.
 * Does not recurse into subsequences.
 *
 * @param seq_a The first sequence.
 * @param seq_b The second sequence.
 * @return Sequence The merged sequence.
 */
[[nodiscard]] inline auto merge(Sequence const &seq_a, Sequence const &seq_b)
    -> Sequence
{
    if (seq_a.cells.empty())
    {
        return seq_b;
    }
    if (seq_b.cells.empty())
    {
        return seq_a;
    }

    auto merged_seq = Sequence{};

    auto const max_size = std::max(seq_a.cells.size(), seq_b.cells.size());
    merged_seq.cells.reserve(max_size);

    for (auto i = 0u; i < max_size; ++i)
    {
        merged_seq.cells.push_back(seq_a.cells[i % seq_a.cells.size()]);
        merged_seq.cells.push_back(seq_b.cells[i % seq_b.cells.size()]);
    }

    return merged_seq;
}

/**
 * @brief Split a Sequence into two Sequences at the provided index.
 *
 * If index is out of bounds, the original Sequence is returned as the first element.
 *
 * @param seq The sequence to split.
 * @param index The index to split at, this element is included in the second Sequence.
 * @return std::pair<Sequence, Sequence> The split Sequences.
 *
 * @throws std::invalid_argument If index is less than zero.
 */
[[nodiscard]] inline auto split(Sequence const &seq, int index)
    -> std::pair<Sequence, Sequence>
{
    if (index < 0)
    {
        throw std::invalid_argument("index must be greater than or equal to zero");
    }

    index = std::min(index, static_cast<int>(seq.cells.size()));

    return {
        Sequence{std::vector(std::cbegin(seq.cells), std::cbegin(seq.cells) + index)},
        Sequence{std::vector(std::cbegin(seq.cells) + index, std::cend(seq.cells))}};
}

} // namespace sequence::modify
#endif // SEQUENCE_MODIFY_HPP