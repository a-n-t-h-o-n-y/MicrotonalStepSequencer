#include <sequence/modify.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

#include <sequence/pattern.hpp>
#include <sequence/random.hpp>

namespace
{

using namespace sequence;

template <typename NoteFn, typename SequenceFn>
[[nodiscard]]
auto visit_recursive(MusicElement const &element,
                     Pattern const &pattern,
                     NoteFn const &note_fn,
                     SequenceFn const &seq_fn) -> MusicElement
{
    static_assert(std::is_invocable_v<NoteFn, Note const &>,
                  "NoteFn must be invocable with a Note const&");
    static_assert(std::is_invocable_v<SequenceFn, Sequence const &>,
                  "SequenceFn must be invocable with a Sequence const&");

    return std::visit(
        utility::overload{
            [&](Note const &note) -> MusicElement { return note_fn(note); },
            [&](Sequence const &seq) -> MusicElement {
                auto new_seq = seq_fn(seq);
                auto view = PatternView{new_seq.cells, pattern};
                for (auto &c : view)
                {
                    for (auto &elem : c.elements)
                    {
                        elem = visit_recursive(elem, pattern, note_fn, seq_fn);
                    }
                }
                return new_seq;
            },
        },
        element);
}

template <typename NoteFn>
[[nodiscard]]
auto visit_recursive(MusicElement const &element,
                     Pattern const &pattern,
                     NoteFn const &note_fn) -> MusicElement
{
    return visit_recursive(element, pattern, note_fn, [](Sequence s) { return s; });
}

} // namespace

namespace sequence::modify
{

auto randomize_pitch(MusicElement element, Pattern const &pattern, int min, int max)
    -> MusicElement
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }

    auto &gen = sequence::random::engine();
    auto dis = std::uniform_int_distribution{min, max};

    return visit_recursive(element, pattern, [&](Note n) {
        n.pitch = dis(gen);
        return n;
    });
}

auto randomize_pitch(Cell cell, Pattern const &pattern, int min, int max) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = randomize_pitch(elem, pattern, min, max);
    }
    return cell;
}

auto randomize_velocity(MusicElement element,
                        Pattern const &pattern,
                        float min,
                        float max) -> MusicElement
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto &gen = sequence::random::engine();
    auto dis = std::uniform_real_distribution{min, max};

    return visit_recursive(element, pattern, [&](Note n) {
        n.velocity = dis(gen);
        return n;
    });
}

auto randomize_velocity(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = randomize_velocity(elem, pattern, min, max);
    }
    return cell;
}

auto randomize_delay(MusicElement element, Pattern const &pattern, float min, float max)
    -> MusicElement
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto &gen = sequence::random::engine();
    auto dis = std::uniform_real_distribution{min, max};

    return visit_recursive(element, pattern, [&](Note n) {
        n.delay = dis(gen);
        return n;
    });
}

auto randomize_delay(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = randomize_delay(elem, pattern, min, max);
    }
    return cell;
}

auto randomize_gate(MusicElement element, Pattern const &pattern, float min, float max)
    -> MusicElement
{
    if (min > max)
    {
        throw std::invalid_argument("min must be less than or equal to max");
    }
    else if (min < 0.f || min > 1.f || max < 0.f || max > 1.f)
    {
        throw std::invalid_argument("min and max must be in the range [0, 1]");
    }

    auto &gen = sequence::random::engine();
    auto dis = std::uniform_real_distribution{min, max};

    return visit_recursive(element, pattern, [&](Note n) {
        n.gate = dis(gen);
        return n;
    });
}

auto randomize_gate(Cell cell, Pattern const &pattern, float min, float max) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = randomize_gate(elem, pattern, min, max);
    }
    return cell;
}

auto shift_pitch(MusicElement element, Pattern const &pattern, int amount)
    -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        n.pitch += amount;
        return n;
    });
}

auto shift_pitch(Cell cell, Pattern const &pattern, int amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = shift_pitch(elem, pattern, amount);
    }
    return cell;
}

auto shift_velocity(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        n.velocity = std::clamp(n.velocity + amount, 0.f, 1.f);
        return n;
    });
}

auto shift_velocity(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = shift_velocity(elem, pattern, amount);
    }
    return cell;
}

auto shift_delay(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        n.delay = std::clamp(n.delay + amount, 0.f, 1.f);
        return n;
    });
}

auto shift_delay(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = shift_delay(elem, pattern, amount);
    }
    return cell;
}

auto shift_gate(MusicElement element, Pattern const &pattern, float amount)
    -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        n.gate = std::clamp(n.gate + amount, 0.f, 1.f);
        return n;
    });
}

auto shift_gate(Cell cell, Pattern const &pattern, float amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = shift_gate(elem, pattern, amount);
    }
    return cell;
}

auto set_pitch(MusicElement element, Pattern const &pattern, int pitch) -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        n.pitch = pitch;
        return n;
    });
}

auto set_pitch(Cell cell, Pattern const &pattern, int pitch) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = set_pitch(elem, pattern, pitch);
    }
    return cell;
}

auto set_octave(MusicElement element,
                Pattern const &pattern,
                int octave,
                std::size_t tuning_length) -> MusicElement
{
    if (tuning_length == 0)
    {
        throw std::invalid_argument("tuning_length must be greater than 0");
    }

    return visit_recursive(element, pattern, [&](Note n) {
        auto const tuning_length_i = static_cast<int>(tuning_length);
        auto degree_in_current_octave =
            (n.pitch % tuning_length_i + tuning_length_i) % tuning_length_i;

        n.pitch = degree_in_current_octave + (octave * tuning_length_i);
        return n;
    });
}

auto set_octave(Cell cell,
                Pattern const &pattern,
                int octave,
                std::size_t tuning_length) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = set_octave(elem, pattern, octave, tuning_length);
    }
    return cell;
}

auto set_velocity(MusicElement element, Pattern const &pattern, float velocity)
    -> MusicElement
{
    velocity = std::clamp(velocity, 0.f, 1.f);
    return visit_recursive(element, pattern, [&](Note n) {
        n.velocity = velocity;
        return n;
    });
}

auto set_velocity(Cell cell, Pattern const &pattern, float velocity) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = set_velocity(elem, pattern, velocity);
    }
    return cell;
}

auto set_delay(MusicElement element, Pattern const &pattern, float delay)
    -> MusicElement
{
    delay = std::clamp(delay, 0.f, 1.f);
    return visit_recursive(element, pattern, [&](Note n) {
        n.delay = delay;
        return n;
    });
}

auto set_delay(Cell cell, Pattern const &pattern, float delay) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = set_delay(elem, pattern, delay);
    }
    return cell;
}

auto set_gate(MusicElement element, Pattern const &pattern, float gate) -> MusicElement
{
    gate = std::clamp(gate, 0.f, 1.f);
    return visit_recursive(element, pattern, [&](Note n) {
        n.gate = gate;
        return n;
    });
}

auto set_gate(Cell cell, Pattern const &pattern, float gate) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = set_gate(elem, pattern, gate);
    }
    return cell;
}

auto rotate(MusicElement element, int amount) -> MusicElement
{
    amount *= -1;

    std::visit(utility::overload{
                   [](Note &) {},
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
               element);

    return element;
}

auto rotate(Cell cell, int amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = rotate(elem, amount);
    }
    return cell;
}

auto mirror(MusicElement element, Pattern const &pattern, int center_note)
    -> MusicElement
{
    return visit_recursive(element, pattern, [&](Note n) {
        auto const diff = center_note - n.pitch;
        n.pitch = center_note + diff;
        return n;
    });
}

auto mirror(Cell cell, Pattern const &pattern, int center_note) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = mirror(elem, pattern, center_note);
    }
    return cell;
}

auto reverse(MusicElement element) -> MusicElement
{
    return visit_recursive(
        element, {0, {1}}, [](Note n) { return n; },
        [](Sequence seq) {
            std::ranges::reverse(seq.cells);
            return seq;
        });
}

auto reverse(Cell cell) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = reverse(elem);
    }
    return cell;
}

auto repeat(MusicElement element, std::size_t count) -> MusicElement
{
    if (count == 0)
    {
        throw std::invalid_argument{"Invalid count: " + std::to_string(count)};
    }
    auto seq = Sequence{};
    seq.cells.reserve(count);

    for (auto i = std::size_t{0}; i < count; ++i)
    {
        seq.cells.push_back({.elements = {element}, .weight = 1.f});
    }

    return seq;
}

auto repeat(Cell cell, std::size_t count) -> Cell
{
    if (count == 0)
    {
        throw std::invalid_argument{"Invalid count: " + std::to_string(count)};
    }
    auto seq = Sequence{};
    seq.cells.reserve(count);

    for (auto i = std::size_t{0}; i < count; ++i)
    {
        seq.cells.push_back(cell);
    }

    return {.elements = {seq}, .weight = cell.weight};
}

auto stretch(MusicElement element, Pattern const &pattern, std::size_t amount)
    -> MusicElement
{
    if (amount == 0)
    {
        throw std::invalid_argument{"Invalid amount: " + std::to_string(amount)};
    }

    return std::visit(
        utility::overload{
            [&](Note note) -> MusicElement { return repeat(note, amount); },
            [&](Sequence seq) -> MusicElement {
                auto view = PatternView{seq.cells, pattern};
                for (auto &c : view)
                {
                    c = stretch(c, pattern, amount);
                }
                return seq;
            },
        },
        element);
}

auto stretch(Cell cell, Pattern const &pattern, std::size_t amount) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = stretch(elem, pattern, amount);
    }
    return cell;
}

auto compress(MusicElement element, Pattern const &pattern) -> MusicElement
{
    return std::visit(utility::overload{
                          [&](Note const &note) -> MusicElement { return note; },
                          [&](Sequence const &seq) -> MusicElement {
                              auto new_seq = Sequence{};
                              auto view = ConstPatternView{seq.cells, pattern};
                              for (auto const &c : view)
                              {
                                  new_seq.cells.push_back(c);
                              }
                              return new_seq;
                          },
                      },
                      element);
}

auto compress(Cell cell, Pattern const &pattern) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = compress(elem, pattern);
    }
    return cell;
}

auto shuffle(MusicElement element) -> MusicElement
{
    return visit_recursive(
        element, {0, {1}}, [](Note n) { return n; },
        [](Sequence seq) {
            std::ranges::shuffle(seq.cells, sequence::random::engine());
            return seq;
        });
}

auto shuffle(Cell cell) -> Cell
{
    for (auto &elem : cell.elements)
    {
        elem = shuffle(elem);
    }
    return cell;
}

auto note(int pitch, float velocity, float delay, float gate) -> MusicElement
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
    return Note{pitch, velocity, delay, gate};
}

} // namespace sequence::modify
