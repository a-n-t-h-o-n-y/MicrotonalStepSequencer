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
 * @brief Randomize the note intervals in an existing Cell.
 *
 * This overwrites existings notes, it does not apply an offset to the existing
 * notes.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param min The minimum note value to use, inclusive.
 * @param max The maximum note value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max.
 */
[[nodiscard]] inline auto randomize_intervals(Cell cell, int min, int max) -> Cell
{
    using namespace utility;

    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_int_distribution{min, max};

    std::visit(overload{
                   [&](Note &note) { note.interval = dis(gen); },
                   [](Rest &) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = randomize_intervals(c, min, max);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Randomize the note velocities in a given Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param min The minimum velocity value to use, inclusive.
 * @param max The maximum velocity value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_velocity(Cell cell, float min, float max) -> Cell
{
    using namespace utility;

    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    std::visit(overload{
                   [&](Note &note) { note.velocity = dis(gen); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = randomize_velocity(c, min, max);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Randomize the note delays in a Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param min The minimum delay value to use, inclusive.
 * @param max The maximum delay value to use, inclusive.
 * @return Cell The randomized Cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_delay(Cell cell, float min, float max) -> Cell
{
    using namespace utility;

    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    std::visit(overload{
                   [&](Note &note) { note.delay = dis(gen); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = randomize_delay(c, min, max);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Randomize the note gates of a Cell.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to randomize.
 * @param min The minimum gate value to use, inclusive.
 * @param max The maximum gate value to use, inclusive.
 * @return Cell The randomized cell.
 *
 * @throws std::invalid_argument If min is greater than max, or if min or max are not in
 * the range [0, 1].
 */
[[nodiscard]] inline auto randomize_gate(Cell cell, float min, float max) -> Cell
{
    using namespace utility;

    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_real_distribution{min, max};

    std::visit(overload{
                   [&](Note &note) { note.gate = dis(gen); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = randomize_gate(c, min, max);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Shift note pitches by a constant amount.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to shift the notes of.
 * @param amount The amount to shift by, can be positive, negative or zero.
 * @return Cell The pitch shifted Cell.
 */
[[nodiscard]] inline auto shift_pitch(Cell cell, int amount) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) { note.interval += amount; },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = shift_pitch(c, amount);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Rotate the note order in an existing sequence so the `amount`th note is the
 * new first note. Negative amounts count from the end of the Sequence.
 *
 * If amount is greater than the size of the Sequence, it shifts by the remainder.
 *
 * No-op for Note and Rest.
 *
 * @param Cell The Cell to rotate.
 * @param amount The amount to rotate by, can be positive, negative or zero. Positive is
 * a right shift and negative a left shift, zero returns original Sequence.
 * @return Cell The rotated Cell.
 */
[[nodiscard]] inline auto rotate(Cell cell, int amount) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note) {},
                   [](Rest) {},
                   [&](Sequence &seq) {
                       if (seq.cells.empty())
                       {
                           return;
                       }
                       auto const size = static_cast<int>(seq.cells.size());
                       amount = (amount % size + size) % size;
                       std::rotate(seq.cells.begin(), seq.cells.begin() + amount,
                                   seq.cells.end());
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Set delay of even index notes to zero and odd index notes to `amount`.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to modify.
 * @param amount The amount to set the note delays to.
 * @return Cell The cell with the note delays set.
 *
 * @throws std::invalid_argument If amount is less than zero or greater than one.
 */
[[nodiscard]] inline auto swing(Cell cell, float amount, bool is_odd = false) -> Cell
{
    using namespace utility;

    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    std::visit(overload{
                   [&](Note &note) { note.delay = is_odd ? amount : 0.f; },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto i = 0u; i < seq.cells.size(); ++i)
                       {
                           auto &c = seq.cells[i];
                           c = swing(c, amount, (i % 2) == 1);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Quantize the notes in an existing sequence zero delay and full gate.
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to quantize.
 * @return Cell The quantized Cell.
 */
[[nodiscard]] inline auto quantize(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &note) {
                       note.delay = 0.f;
                       note.gate = 1.f;
                   },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = quantize(c);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Swap the notes in a Cell around a center note.

 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to mirror.
 * @param center_note The note to mirror around.
 * @return Cell The mirrored Cell.
 */
[[nodiscard]] inline auto mirror(Cell cell, int center_note) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) {
                       auto const diff = center_note - note.interval;
                       note.interval = center_note + diff;
                   },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = mirror(c, center_note);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Reverse the notes in a Cell
 *
 * If cell is a Sequence, this will recurse into child Sequences.
 *
 * @param cell The Cell to reverse.
 * @return Cell The reversed Cell.
 */
[[nodiscard]] inline auto reverse(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &) {},
                   [](Rest &) {},
                   [&](Sequence &seq) {
                       std::ranges::reverse(seq.cells);
                       for (auto &c : seq.cells)
                       {
                           c = reverse(c);
                       }
                   },
               },
               cell);
    return cell;
}

/**
 * @brief Create `count` copies of the provided Cell as a new Cell.
 *
 * The returned Cell is always a Sequence. This is also known as 'split'. This
 * does not recurse into child Sequences.
 *
 * @param cell The Cell to duplicate.
 * @param count The number of copies to make, if zero, returns an empty Sequence.
 * @return Cell The repeated sequence as a Cell.
 */
[[nodiscard]] inline auto repeat(Cell const &cell, std::size_t count) -> Cell
{
    auto result = Sequence{};
    result.cells.reserve(count);

    for (auto i = std::size_t{0}; i < count; ++i)
    {
        result.cells.push_back(cell);
    }

    return result;
}

/**
 * @brief Repeat each note in the sequence `amount` times in a row.
 *
 * The returned Cell is always a Sequence. This recurses into child Sequences.
 *
 * @param cell The Cell to repeat.
 * @param amount The number of times to repeat each Cell, if zero, returns an empty
 * Sequence.
 * @return Cell The stretched Sequence.
 */
[[nodiscard]] inline auto stretch(Cell const &cell, std::size_t amount) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [&](Note const &note) { return repeat(note, amount); },
                          [&](Rest const &rest) { return repeat(rest, amount); },
                          [&](Sequence const &seq) {
                              auto result = Sequence{};
                              result.cells.reserve(seq.cells.size());

                              for (auto const &c : seq.cells)
                              {
                                  result.cells.push_back(stretch(c, amount));
                              }

                              return Cell{result};
                          },
                      },
                      cell);
}

/**
 * @brief Compress a Cell by keeping every `amount` note and removing the remaining.
 *
 * This does not recurse into child Sequences. No-op for Notes and Rests.
 *
 * @param seq The Cell to compress.
 * @param amount The amount of compression, meaning the first note is kept, then
 * `amount` minus one notes are skipped, then the next note is kept, etc.
 * @return Cell The compressed Sequence.
 *
 * @throws std::invalid_argument If amount is less than one.
 */
[[nodiscard]] inline auto compress(Cell const &cell, std::size_t amount) -> Cell
{
    using namespace utility;

    if (amount < 1)
    {
        throw std::invalid_argument("count must be greater than or equal to one");
    }

    return std::visit(overload{
                          [&](Note const &note) { return Cell{note}; },
                          [&](Rest const &rest) { return Cell{rest}; },
                          [&](Sequence const &seq) {
                              auto result = Sequence{};
                              result.cells.reserve(seq.cells.size() / amount);

                              std::copy_if(std::cbegin(seq.cells), std::cend(seq.cells),
                                           std::back_inserter(result.cells),
                                           [amount, i = 0u](auto const &) mutable {
                                               return (i++ % amount) == 0;
                                           });

                              return Cell{result};
                          },
                      },
                      cell);
}

/**
 * @brief Get the first Note or Rest in a Cell.
 *
 * If the Cell is an empty Sequence, this will return a Rest. No-op for Notes
 * and Rests.
 *
 * @param cell The Cell to get the first Note or Rest from.
 * @return Cell The first Note or Rest.
 */
[[nodiscard]] inline auto first(Cell const &cell) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [&](Note const &note) -> Cell { return note; },
                          [&](Rest const &rest) -> Cell { return rest; },
                          [&](Sequence const &seq) -> Cell {
                              if (seq.cells.empty())
                              {
                                  return Rest{};
                              }
                              else
                              {
                                  return seq.cells.front();
                              }
                          },
                      },
                      cell);
}

/**
 * @brief Get the last Note or Rest in a Cell.
 *
 * If the Cell is an empty Sequence, this will return a Rest. No-op for Notes
 * and Rests.
 *
 * @param cell The Cell to get the last Note or Rest from.
 * @return Cell The last Note or Rest.
 */
[[nodiscard]] inline auto last(Cell const &cell) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [&](Note const &note) -> Cell { return note; },
                          [&](Rest const &rest) -> Cell { return rest; },
                          [&](Sequence const &seq) -> Cell {
                              if (seq.cells.empty())
                              {
                                  return Rest{};
                              }
                              else
                              {
                                  return seq.cells.back();
                              }
                          },
                      },
                      cell);
}

/**
 * @brief Shuffle the notes in a Cell into a random order.
 *
 * Each note remains in the Sequence, but in a (potentially) new order. Sub-Sequences
 * are recursed into but notes are only shuffled within their own Sequence.
 *
 * @param cell The Cell to shuffle.
 * @return Cell The shuffled Cell.
 */
[[nodiscard]] inline auto shuffle(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &) {},
                   [](Rest &) {},
                   [&](Sequence &seq) {
                       std::ranges::shuffle(seq.cells,
                                            std::mt19937{std::random_device{}()});
                       for (auto &c : seq.cells)
                       {
                           c = shuffle(c);
                       }
                   },
               },
               cell);

    return cell;
}

/**
 * @brief Concatenate two Cells into a new Cell.
 *
 * @param cell_a The first Cell.
 * @param cell_b The second Cell.
 * @return Cell The concatenated Cell as a Sequence.
 */
[[nodiscard]] inline auto concat(Cell const &cell_a, Cell const &cell_b) -> Cell
{
    using namespace utility;
    return std::visit(
        overload{
            [&](Note const &note_a, Note const &note_b) {
                return Sequence{{note_a, note_b}};
            },
            [&](Note const &note, Rest const &rest) {
                return Sequence{{note, rest}};
            },
            [&](Rest const &rest, Note const &note) {
                return Sequence{{rest, note}};
            },
            [&](Rest const &rest_a, Rest const &rest_b) {
                return Sequence{{rest_a, rest_b}};
            },
            [&](Note const &note, Sequence const &seq) {
                auto concat_seq = seq;
                concat_seq.cells.insert(std::cbegin(concat_seq.cells), note);
                return concat_seq;
            },
            [&](Rest const &rest, Sequence const &seq) {
                auto concat_seq = seq;
                concat_seq.cells.insert(std::cbegin(concat_seq.cells), rest);
                return concat_seq;
            },
            [&](Sequence const &seq, Note const &note) {
                auto concat_seq = seq;
                concat_seq.cells.push_back(note);
                return concat_seq;
            },
            [&](Sequence const &seq, Rest const &rest) {
                auto concat_seq = seq;
                concat_seq.cells.push_back(rest);
                return concat_seq;
            },
            [&](Sequence const &seq_a, Sequence const &seq_b) {
                auto concat_seq = seq_a;
                concat_seq.cells.insert(std::cend(concat_seq.cells),
                                        std::cbegin(seq_b.cells),
                                        std::cend(seq_b.cells));
                return concat_seq;
            },
        },
        cell_a, cell_b);
}

/**
 * @brief Merge two Cell into a new Cell by interleaving their notes
 *
 * If one Sequence is shorter than the other, it is repeated to be the same length as
 * the longer Sequence. If either Sequence is empty, the other Sequence is returned.
 * Does not recurse into subsequences.
 *
 * @param cell_a The first Cell.
 * @param cell_b The second Cell.
 * @return Cell The merged Cell as a Sequence.
 */
[[nodiscard]] inline auto merge(Cell const &cell_a, Cell const &cell_b) -> Cell
{
    using namespace utility;

    return std::visit(
        overload{
            [&](Note const &note_a, Note const &note_b) {
                return Sequence{{note_a, note_b}};
            },
            [&](Note const &note, Rest const &rest) {
                return Sequence{{note, rest}};
            },
            [&](Rest const &rest, Note const &note) {
                return Sequence{{rest, note}};
            },
            [&](Rest const &rest_a, Rest const &rest_b) {
                return Sequence{{rest_a, rest_b}};
            },
            [&](Note const &note, Sequence const &seq) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(note);
                    merged_seq.cells.push_back(c);
                }
                return merged_seq;
            },
            [&](Rest const &rest, Sequence const &seq) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(rest);
                    merged_seq.cells.push_back(c);
                }
                return merged_seq;
            },
            [&](Sequence const &seq, Note const &note) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(c);
                    merged_seq.cells.push_back(note);
                }
                return merged_seq;
            },
            [&](Sequence const &seq, Rest const &rest) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(c);
                    merged_seq.cells.push_back(rest);
                }
                return merged_seq;
            },
            [&](Sequence const &seq_a, Sequence const &seq_b) {
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
                merged_seq.cells.reserve(max_size * 2);

                for (auto i = 0u; i < max_size; ++i)
                {
                    merged_seq.cells.push_back(seq_a.cells[i % seq_a.cells.size()]);
                    merged_seq.cells.push_back(seq_b.cells[i % seq_b.cells.size()]);
                }

                return merged_seq;
            },
        },
        cell_a, cell_b);
}

/**
 * @brief Divide a Sequence into two Sequences at the provided index.
 *
 * If index is out of bounds, the second Sequence will be empty.
 *
 * @param seq The sequence to divide.
 * @param index The index to split at, this element is included in the second Sequence.
 * @return Cell The split Cell as a new Sequence.
 */
[[nodiscard]] inline auto divide(Cell const &cell, std::size_t index) -> Cell
{
    using namespace utility;

    return std::visit(
        overload{[&](Note const &note) { return Sequence{{note}}; },
                 [&](Rest const &rest) { return Sequence{{rest}}; },
                 [&](Sequence const &seq) {
                     index = std::min(index, seq.cells.size());
                     auto const begin = std::cbegin(seq.cells);
                     auto const mid = std::cbegin(seq.cells) + (std::ptrdiff_t)index;
                     auto const end = std::cend(seq.cells);
                     return Sequence{
                         std::vector<Cell>{Sequence{std::vector(begin, mid)},
                                           Sequence{std::vector(mid, end)}},
                     };
                 }},
        cell);
}

[[nodiscard]] inline auto note(int interval, float velocity, float delay, float gate)
    -> Cell
{
    return Note{interval, velocity, delay, gate};
}

[[nodiscard]] inline auto rest() -> Cell
{
    return Rest{};
}

// TODO SequenceBuilders
[[nodiscard]] inline auto sequence(std::vector<Cell> cells) -> Cell
{
    return Sequence{std::move(cells)};
}

[[nodiscard]] inline auto flip(Cell const &cell, Note n = Note{0, 1.f, 0.f, 1.f})
    -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [](Note const &) -> Cell { return Rest{}; },
                          [=](Rest const &) -> Cell { return n; },
                          [](Sequence const &seq) -> Cell {
                              auto result = Sequence{};
                              result.cells.reserve(seq.cells.size());
                              std::transform(std::cbegin(seq.cells),
                                             std::cend(seq.cells),
                                             std::back_inserter(result.cells),
                                             [](auto const &c) { return flip(c); });
                              return result;
                          },
                      },
                      cell);
}

} // namespace sequence::modify
#endif // SEQUENCE_MODIFY_HPP