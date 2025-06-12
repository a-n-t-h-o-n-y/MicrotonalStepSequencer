#ifndef SEQUENCE_TEST_HELPER_HPP
#define SEQUENCE_TEST_HELPER_HPP

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>

#include <sequence/midi.hpp>
#include <sequence/sequence.hpp>
#include <sequence/utility.hpp>

namespace sequence::test::helper
{

/// Shorthand for std::holds_alternative
template <typename T, typename X>
[[nodiscard]] auto holds(X const &x) -> bool
{
    return std::holds_alternative<T>(x.element);
}

template <typename T, typename X>
[[nodiscard]] auto get(X &x) -> T &
{
    return std::get<T>(x.element);
}

template <typename T, typename X>
[[nodiscard]] auto get(X const &x) -> T const &
{
    return std::get<T>(x.element);
}

template <typename Fn>
concept NoteChecker = requires(Fn fn, Note const &note) {
    { fn(note) } -> std::same_as<void>;
};

template <NoteChecker Fn>
auto check_sequence(Cell const &cell, Fn &&checker) -> void
{
    using namespace sequence::utility;

    std::visit(overload{
                   [&checker](Note const &note) { checker(note); },
                   [](Rest) {},
                   [&checker](Sequence const &seq) {
                       for (auto const &c : seq.cells)
                       {
                           check_sequence(c, checker);
                       }
                   },
               },
               cell.element);
}

template <typename Fn>
auto modify_notes(Cell &cell, Fn &&modifier) -> void
{
    using namespace sequence::utility;

    std::visit(overload{
                   [&modifier](Note &note) { modifier(note); },
                   [](Rest) {},
                   [&modifier](Sequence &seq) {
                       for (auto &c : seq.cells)
                       {
                           modify_notes(c, modifier);
                       }
                   },
               },
               cell.element);
}

/**
 * @brief Prints a Sequence to stdout.
 *
 * @param seq The Sequence to print.
 * @param indent The indentation level, used for recursion into sub-Sequences.
 */
inline auto print_sequence(Cell const &cell, int indent = 0) -> void
{
    using namespace sequence::utility;

    std::visit(overload{
                   [&](Note const &note) {
                       std::cout << std::string(indent * 2, ' ');
                       std::cout << "Note(pitch=" << note.pitch
                                 << ", velocity=" << note.velocity
                                 << ", delay=" << note.delay << ", gate=" << note.gate
                                 << ")\n";
                   },
                   [](Rest) { std::cout << "Rest\n"; },
                   [&](Sequence const &seq) {
                       std::cout << "Sequence(\n";
                       for (auto const &c : seq.cells)
                       {
                           print_sequence(c, indent + 1);
                       }
                       std::cout << ")\n";
                   },
               },
               cell.element);
}

inline auto print_midi_event_timeline(midi::EventTimeline const &timeline) -> void
{
    using namespace sequence::utility;
    for (auto const &[event, offset] : timeline)
    {
        using namespace utility;
        std::cout << "Event: ";
        visit(overload{
                  [&](midi::NoteOn const &note_on) {
                      std::cout << "NoteOn(note=" << (int)note_on.note
                                << ", velocity=" << (int)note_on.velocity << ")";
                  },
                  [&](midi::NoteOff const &note_off) {
                      std::cout << "NoteOff(note=" << (int)note_off.note << ")";
                  },
                  [&](midi::PitchBend const &pitch_bend) {
                      std::cout << "PitchBend(amount=" << (int)pitch_bend.value << ")";
                  },
              },
              event);
        std::cout << " | offset=" << offset << "\n";
    }
}

} // namespace sequence::test::helper
#endif // SEQUENCE_TEST_HELPER_HPP