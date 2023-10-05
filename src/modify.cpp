#include <sequence/modify.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

#include <sequence/pattern.hpp>

namespace
{

using namespace sequence;

template <typename NoteFn, typename RestFn, typename SequenceFn>
[[nodiscard]] auto visit_recursive(Cell const &cell, Pattern const &pattern,
                                   NoteFn const &note_fn, RestFn const &rest_fn,
                                   SequenceFn const &seq_fn) -> Cell
{
    static_assert(std::is_invocable_v<NoteFn, Note const &>,
                  "NoteFn must be invocable with a Note const&");
    static_assert(std::is_invocable_v<RestFn, Rest const &>,
                  "RestFn must be invocable with a Rest const&");
    static_assert(std::is_invocable_v<SequenceFn, Sequence const &>,
                  "SequenceFn must be invocable with a Sequence const&");

    return std::visit(utility::overload{
                          [&](Note const &note) -> Cell { return note_fn(note); },
                          [&](Rest const &rest) -> Cell { return rest_fn(rest); },
                          [&](Sequence const &seq) -> Cell {
                              auto new_seq = seq_fn(seq);
                              auto view = PatternView{new_seq.cells, pattern};
                              for (auto &c : view)
                              {
                                  c = visit_recursive(c, pattern, note_fn, rest_fn,
                                                      seq_fn);
                              }
                              return new_seq;
                          },
                      },
                      cell);
}

template <typename NoteFn, typename RestFn>
[[nodiscard]] auto visit_recursive(Cell const &cell, Pattern const &pattern,
                                   NoteFn const &note_fn, RestFn const &rest_fn) -> Cell
{
    return visit_recursive(cell, pattern, note_fn, rest_fn,
                           [](Sequence s) { return s; });
}

template <typename NoteFn>
[[nodiscard]] auto visit_recursive(Cell const &cell, Pattern const &pattern,
                                   NoteFn const &note_fn) -> Cell
{
    return visit_recursive(cell, pattern, note_fn, [](Rest r) { return r; });
}

} // namespace

namespace sequence::modify
{

auto randomize_intervals(Cell cell, Pattern const &pattern, int min, int max) -> Cell
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }

    auto gen = std::mt19937{std::random_device{}()};
    auto dis = std::uniform_int_distribution{min, max};

    return visit_recursive(cell, pattern, [&](Note n) {
        n.interval = dis(gen);
        return n;
    });
}

auto randomize_velocity(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
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

    return visit_recursive(cell, pattern, [&](Note n) {
        n.velocity = dis(gen);
        return n;
    });
}

auto randomize_delay(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
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

    return visit_recursive(cell, pattern, [&](Note n) {
        n.delay = dis(gen);
        return n;
    });
}

auto randomize_gate(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
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

    return visit_recursive(cell, pattern, [&](Note n) {
        n.gate = dis(gen);
        return n;
    });
}

auto shift_interval(Cell cell, Pattern const &pattern, int amount) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.interval += amount;
        return n;
    });
}

auto shift_velocity(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.velocity = std::clamp(n.velocity + amount, 0.f, 1.f);
        return n;
    });
}

auto shift_delay(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.delay = std::clamp(n.delay + amount, 0.f, 1.f);
        return n;
    });
}

auto shift_gate(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.gate = std::clamp(n.gate + amount, 0.f, 1.f);
        return n;
    });
}

auto set_interval(Cell cell, Pattern const &pattern, int interval) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.interval = interval;
        return n;
    });
}

auto set_octave(Cell cell, Pattern const &pattern, int octave,
                std::size_t tuning_length) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        auto const tuning_length_i = static_cast<int>(tuning_length);
        auto degree_in_current_octave =
            (n.interval % tuning_length_i + tuning_length_i) % tuning_length_i;

        n.interval = degree_in_current_octave + (octave * tuning_length_i);
        return n;
    });
}

auto set_velocity(Cell cell, Pattern const &pattern, float velocity) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.velocity = velocity;
        return n;
    });
}

auto set_delay(Cell cell, Pattern const &pattern, float delay) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.delay = delay;
        return n;
    });
}

auto set_gate(Cell cell, Pattern const &pattern, float gate) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        n.gate = gate;
        return n;
    });
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

auto quantize(Cell cell, Pattern const &pattern) -> Cell
{
    return visit_recursive(cell, pattern, [](Note n) {
        n.delay = 0.f;
        n.gate = 1.f;
        return n;
    });
}

auto mirror(Cell cell, Pattern const &pattern, int center_note) -> Cell
{
    return visit_recursive(cell, pattern, [&](Note n) {
        auto const diff = center_note - n.interval;
        n.interval = center_note + diff;
        return n;
    });
}

auto reverse(Cell cell) -> Cell
{
    return visit_recursive(
        cell, {0, {1}}, [](Note n) { return n; }, [](Rest r) { return r; },
        [](Sequence seq) {
            std::ranges::reverse(seq.cells);
            return seq;
        });
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

auto stretch(Cell const &cell, Pattern const &pattern, std::size_t amount) -> Cell
{
    return visit_recursive(
        cell, pattern, [&](Note n) { return repeat(n, amount); },
        [&](Rest r) { return repeat(r, amount); });
}

auto compress(Cell const &cell, Pattern const &pattern) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [](Note const &note) -> Cell { return note; },
                          [](Rest const &rest) -> Cell { return rest; },
                          [&](Sequence const &seq) -> Cell {
                              auto result = Sequence{};
                              auto view = ConstPatternView{seq.cells, pattern};
                              for (auto const &c : view)
                              {
                                  result.cells.push_back(c);
                              }
                              return result;
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
    return visit_recursive(
        cell, {0, {1}}, [](Note n) { return n; }, [](Rest r) { return r; },
        [](Sequence seq) {
            std::ranges::shuffle(seq.cells, std::mt19937{std::random_device{}()});
            return seq;
        });
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
    if (velocity < 0.f || velocity > 1.f)
    {
        throw std::invalid_argument("velocity must be in the range [0.0, 1.0]");
    }
    else if (delay < 0.f || delay > 1.f)
    {
        throw std::invalid_argument("delay must be in the range [0.0, 1.0]");
    }
    else if (gate < 0.f || gate > 1.f)
    {
        throw std::invalid_argument("gate must be in the range [0.0, 1.0]");
    }
    return Note{interval, velocity, delay, gate};
}

auto rest() -> Cell
{
    return Rest{};
}

auto sequence(std::vector<Cell> cells) -> Cell
{
    return Sequence{std::move(cells)};
}

auto flip(Cell cell, Pattern const &pattern, Note n) -> Cell
{
    using namespace utility;

    return std::visit(overload{
                          [](Note &) -> Cell { return Rest{}; },
                          [&](Rest &) -> Cell { return n; }, // Returns a copy
                          [&](Sequence &seq) -> Cell {
                              auto view = PatternView{seq.cells, pattern};
                              for (auto &c : view)
                              {
                                  c = flip(c, pattern, n);
                              }
                              return seq;
                          },
                      },
                      cell);
}

auto humanize_velocity(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    return visit_recursive(cell, pattern, [&](Note n) {
        auto const min = std::clamp(n.velocity - amount, 0.f, 1.f);
        auto const max = std::clamp(n.velocity + amount, 0.f, 1.f);
        auto dis = std::uniform_real_distribution{min, max};
        n.velocity = dis(gen);
        return n;
    });
}

auto humanize_delay(Cell cell, sequence::Pattern const &pattern, float amount) -> Cell
{
    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    return visit_recursive(cell, pattern, [&](Note n) {
        auto const min = std::clamp(n.delay - amount, 0.f, 1.f);
        auto const max = std::clamp(n.delay + amount, 0.f, 1.f);
        auto dis = std::uniform_real_distribution{min, max};
        n.delay = dis(gen);
        return n;
    });
}

auto humanize_gate(Cell cell, sequence::Pattern const &pattern, float amount) -> Cell
{
    if (amount < 0.f || amount > 1.f)
    {
        throw std::invalid_argument("amount must be in the range [0.0, 1.0]");
    }

    auto gen = std::mt19937{std::random_device{}()};

    return visit_recursive(cell, pattern, [&](Note n) {
        auto const min = std::clamp(n.gate - amount, 0.f, 1.f);
        auto const max = std::clamp(n.gate + amount, 0.f, 1.f);
        auto dis = std::uniform_real_distribution{min, max};
        n.gate = dis(gen);
        return n;
    });
}

auto notes_fill(Cell cell, Pattern const &pattern, Note note) -> Cell
{
    return visit_recursive(
        cell, pattern, [&](Note const &) { return note; },
        [&](Rest const &) { return note; });
}

auto rests_fill(Cell cell, Pattern const &pattern) -> Cell
{
    return visit_recursive(cell, pattern, [](Note const &) { return Rest{}; },
                           [](Rest const &) { return Rest{}; });
}

} // namespace sequence::modify