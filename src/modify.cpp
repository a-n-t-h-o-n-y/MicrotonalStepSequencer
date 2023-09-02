#include <sequence/modify.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <stdexcept>
#include <variant>
#include <vector>

#include <sequence/pattern.hpp>

namespace sequence::modify
{

auto randomize_intervals(Cell cell, int min, int max) -> Cell
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

auto randomize_velocity(Cell cell, float min, float max) -> Cell
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

auto randomize_delay(Cell cell, float min, float max) -> Cell
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

auto randomize_gate(Cell cell, float min, float max) -> Cell
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

auto shift_pitch(Cell cell, int amount) -> Cell
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

auto shift_velocity(Cell cell, float amount) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) {
                       note.velocity = std::clamp(note.velocity + amount, 0.f, 1.f);
                   },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = shift_velocity(c, amount);
                       }
                   },
               },
               cell);

    return cell;
}

auto shift_delay(Cell cell, float amount) -> Cell
{
    using namespace utility;

    std::visit(
        overload{
            [&](Note &note) { note.delay = std::clamp(note.delay + amount, 0.f, 1.f); },
            [](Rest) {},
            [&](Sequence &seq) {
                for (auto &c : seq.cells)
                {
                    c = shift_delay(c, amount);
                }
            },
        },
        cell);

    return cell;
}

auto shift_gate(Cell cell, float amount) -> Cell
{
    using namespace utility;

    std::visit(
        overload{
            [&](Note &note) { note.gate = std::clamp(note.gate + amount, 0.f, 1.f); },
            [](Rest) {},
            [&](Sequence &seq) {
                for (auto &c : seq.cells)
                {
                    c = shift_gate(c, amount);
                }
            },
        },
        cell);

    return cell;
}

auto set_pitch(Cell cell, int interval) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) { note.interval = interval; },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = set_pitch(c, interval);
                       }
                   },
               },
               cell);

    return cell;
}

auto set_octave(Cell cell, int octave, std::size_t tuning_length) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) {
                       int const tuning_length_i = static_cast<int>(tuning_length);
                       auto degree_in_current_octave =
                           (note.interval % tuning_length_i + tuning_length_i) %
                           tuning_length_i;

                       note.interval =
                           degree_in_current_octave + (octave * tuning_length_i);
                   },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = set_octave(c, octave, tuning_length);
                       }
                   },
               },
               cell);

    return cell;
}

auto set_velocity(Cell cell, float velocity) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) { note.velocity = std::clamp(velocity, 0.f, 1.f); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = set_velocity(c, velocity);
                       }
                   },
               },
               cell);

    return cell;
}

auto set_delay(Cell cell, float delay) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) { note.delay = std::clamp(delay, 0.f, 1.f); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = set_delay(c, delay);
                       }
                   },
               },
               cell);

    return cell;
}

auto set_gate(Cell cell, float gate) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [&](Note &note) { note.gate = std::clamp(gate, 0.f, 1.f); },
                   [](Rest) {},
                   [&](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = set_gate(c, gate);
                       }
                   },
               },
               cell);

    return cell;
}

auto rotate(Cell cell, int amount) -> Cell
{
    using namespace utility;

    amount *= -1;

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

auto swing(Cell cell, float amount, bool is_odd) -> Cell
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

auto quantize(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &note) {
                       note.delay = 0.f;
                       note.gate = 1.f;
                   },
                   [](Rest) {},
                   [](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           c = quantize(c);
                       }
                   },
               },
               cell);

    return cell;
}

auto mirror(Cell cell, int center_note) -> Cell
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

auto reverse(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &) {},
                   [](Rest &) {},
                   [](Sequence &seq) {
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

auto repeat(Cell const &cell, std::size_t count) -> Cell
{
    auto result = Sequence{};
    result.cells.reserve(count);

    for (auto i = std::size_t{0}; i < count; ++i)
    {
        result.cells.push_back(cell);
    }

    return result;
}

auto stretch(Cell const &cell, std::size_t amount) -> Cell
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

auto compress(Cell const &cell, std::size_t amount) -> Cell
{
    using namespace utility;

    if (amount < 1)
    {
        throw std::invalid_argument("count must be greater than or equal to one");
    }

    return std::visit(overload{
                          [](Note const &note) -> Cell { return note; },
                          [](Rest const &rest) -> Cell { return rest; },
                          [&](Sequence const &seq) -> Cell {
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

auto extract(Cell const &cell, std::size_t index) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [](Note const &note) -> Cell { return note; },
                          [](Rest const &rest) -> Cell { return rest; },
                          [&](Sequence const &seq) -> Cell {
                              if (index >= seq.cells.size())
                              {
                                  throw std::invalid_argument("index out of bounds");
                              }
                              return seq.cells[index];
                          },
                      },
                      cell);
}

auto first(Cell const &cell) -> Cell
{
    return extract(cell, 0u);
}

auto last(Cell const &cell) -> Cell
{
    // Can't easily implement with extract(...) because size is unknown at Cell level
    using namespace utility;

    return std::visit(overload{
                          [](Note const &note) -> Cell { return note; },
                          [](Rest const &rest) -> Cell { return rest; },
                          [](Sequence const &seq) -> Cell {
                              if (seq.cells.empty())
                              {
                                  throw std::invalid_argument("index out of bounds");
                              }
                              else
                              {
                                  return seq.cells.back();
                              }
                          },
                      },
                      cell);
}

auto shuffle(Cell cell) -> Cell
{
    using namespace utility;

    std::visit(overload{
                   [](Note &) {},
                   [](Rest &) {},
                   [](Sequence &seq) {
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

auto concat(Cell const &cell_a, Cell const &cell_b) -> Cell
{
    using namespace utility;
    return std::visit(
        overload{
            [](Note const &note_a, Note const &note_b) {
                return Sequence{{note_a, note_b}};
            },
            [](Note const &note, Rest const &rest) {
                return Sequence{{note, rest}};
            },
            [](Rest const &rest, Note const &note) {
                return Sequence{{rest, note}};
            },
            [](Rest const &rest_a, Rest const &rest_b) {
                return Sequence{{rest_a, rest_b}};
            },
            [](Note const &note, Sequence const &seq) {
                auto concat_seq = seq;
                concat_seq.cells.insert(std::cbegin(concat_seq.cells), note);
                return concat_seq;
            },
            [](Rest const &rest, Sequence const &seq) {
                auto concat_seq = seq;
                concat_seq.cells.insert(std::cbegin(concat_seq.cells), rest);
                return concat_seq;
            },
            [](Sequence const &seq, Note const &note) {
                auto concat_seq = seq;
                concat_seq.cells.push_back(note);
                return concat_seq;
            },
            [](Sequence const &seq, Rest const &rest) {
                auto concat_seq = seq;
                concat_seq.cells.push_back(rest);
                return concat_seq;
            },
            [](Sequence const &seq_a, Sequence const &seq_b) {
                auto concat_seq = seq_a;
                concat_seq.cells.insert(std::cend(concat_seq.cells),
                                        std::cbegin(seq_b.cells),
                                        std::cend(seq_b.cells));
                return concat_seq;
            },
        },
        cell_a, cell_b);
}

auto merge(Cell const &cell_a, Cell const &cell_b) -> Cell
{
    using namespace utility;

    return std::visit(
        overload{
            [](Note const &note_a, Note const &note_b) {
                return Sequence{{note_a, note_b}};
            },
            [](Note const &note, Rest const &rest) {
                return Sequence{{note, rest}};
            },
            [](Rest const &rest, Note const &note) {
                return Sequence{{rest, note}};
            },
            [](Rest const &rest_a, Rest const &rest_b) {
                return Sequence{{rest_a, rest_b}};
            },
            [](Note const &note, Sequence const &seq) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(note);
                    merged_seq.cells.push_back(c);
                }
                return merged_seq;
            },
            [](Rest const &rest, Sequence const &seq) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(rest);
                    merged_seq.cells.push_back(c);
                }
                return merged_seq;
            },
            [](Sequence const &seq, Note const &note) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(c);
                    merged_seq.cells.push_back(note);
                }
                return merged_seq;
            },
            [](Sequence const &seq, Rest const &rest) {
                auto merged_seq = Sequence{};
                merged_seq.cells.reserve(seq.cells.size() * 2);
                for (auto const &c : seq.cells)
                {
                    merged_seq.cells.push_back(c);
                    merged_seq.cells.push_back(rest);
                }
                return merged_seq;
            },
            [](Sequence const &seq_a, Sequence const &seq_b) {
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

auto divide(Cell const &cell, std::size_t index) -> Cell
{
    using namespace utility;

    return std::visit(
        overload{[](Note const &note) { return Sequence{{note}}; },
                 [](Rest const &rest) { return Sequence{{rest}}; },
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

auto note(int interval, float velocity, float delay, float gate) -> Cell
{
    return Note{interval, std::clamp(velocity, 0.f, 1.f), std::clamp(delay, 0.f, 1.f),
                std::clamp(gate, 0.f, 1.f)};
}

auto rest() -> Cell
{
    return Rest{};
}

auto sequence(std::vector<Cell> cells) -> Cell
{
    return Sequence{std::move(cells)};
}

auto flip(Cell const &cell, Note n) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [](Note const &) -> Cell { return Rest{}; },
                          [&](Rest const &) -> Cell { return n; }, // Returns a copy
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

auto humanize_velocity(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    using namespace utility;

    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    std::visit(overload{
                   [&](Note &note) {
                       auto const min = std::clamp(note.velocity - amount, 0.f, 1.f);
                       auto const max = std::clamp(note.velocity + amount, 0.f, 1.f);
                       auto dis = std::uniform_real_distribution{min, max};
                       note.velocity = dis(gen);
                   },
                   [](Rest &) {},
                   [&](Sequence &seq) {
                       // TODO maybe clean this up, can a temp patternview be used?
                       auto view = PatternView{seq.cells, pattern};
                       for (auto &c : view)
                       {
                           c = humanize_velocity(c, pattern, amount);
                       }
                   },
               },
               cell);

    return cell;
}

auto humanize_delay(Cell cell, sequence::Pattern const &pattern, float amount) -> Cell
{
    using namespace utility;

    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    std::visit(overload{
                   [&](Note &note) {
                       auto const min = std::clamp(note.delay - amount, 0.f, 1.f);
                       auto const max = std::clamp(note.delay + amount, 0.f, 1.f);
                       auto dis = std::uniform_real_distribution{min, max};
                       note.delay = dis(gen);
                   },
                   [](Rest const &) {},
                   [&](Sequence &seq) {
                       auto view = PatternView{seq.cells, pattern};
                       for (auto &c : view)
                       {
                           c = humanize_delay(c, pattern, amount);
                       }
                   },
               },
               cell);

    return cell;
}

auto humanize_gate(Cell const &cell, float amount) -> Cell
{
    using namespace utility;

    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    return std::visit(
        overload{
            [&](Note const &note) -> Cell {
                auto const min = std::clamp(note.gate - amount, 0.f, 1.f);
                auto const max = std::clamp(note.gate + amount, 0.f, 1.f);
                auto dis = std::uniform_real_distribution{min, max};
                return Note{note.interval, note.velocity, note.delay, dis(gen)};
            },
            [](Rest const &rest) -> Cell { return rest; },
            [&](Sequence const &seq) -> Cell {
                auto result = Sequence{};
                result.cells.reserve(seq.cells.size());
                std::transform(std::cbegin(seq.cells), std::cend(seq.cells),
                               std::back_inserter(result.cells),
                               [&](auto const &c) { return humanize_gate(c, amount); });
                return result;
            },
        },
        cell);
}

} // namespace sequence::modify