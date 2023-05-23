#ifndef SEQUENCE_TEST_HELPER_HPP
#define SEQUENCE_TEST_HELPER_HPP

#include <concepts>
#include <format>
#include <iostream>
#include <type_traits>
#include <variant>

#include <sequence/midi.hpp>
#include <sequence/sequence.hpp>
#include <sequence/utility.hpp>

namespace sequence::test::helper
{

/// Shorthand for std::holds_alternative
template <typename T, typename X>
[[nodiscard]] inline auto holds(X const &x) -> bool
{
    return std::holds_alternative<T>(x);
}

template <typename Fn>
concept NoteChecker = requires(Fn fn, Note const &note) {
    {
        fn(note)
    } -> std::same_as<void>;
};

template <NoteChecker Fn>
auto check_sequence(Sequence const &seq, Fn &&checker) -> void
{
    using namespace sequence::utility;
    visit_cells(seq, overload{
                         [&checker](Note const &note) { checker(note); },
                         [](Rest) {},
                         [&checker](Sequence const &seq) {
                             check_sequence(seq, std::forward<Fn>(checker));
                         },
                     });
}

template <typename Fn>
auto modify_notes(Sequence &seq, Fn &&modifier) -> void
{
    using namespace sequence::utility;
    visit_cells(seq, overload{
                         [&modifier](Note &note) { modifier(note); },
                         [](Rest) {},
                         [&modifier](Sequence &seq) {
                             modify_notes(seq, std::forward<Fn>(modifier));
                         },
                     });
}

/**
 * @brief Prints a Sequence to stdout.
 *
 * @param seq The Sequence to print.
 * @param indent The indentation level, used for recursion into sub-Sequences.
 */
inline auto print_sequence(Sequence const &seq, int indent = 0) -> void
{
    using namespace sequence::utility;
    visit_cells(seq, overload{
                         [&](Note const &note) {
                             std::cout << std::string(indent * 2, ' ');
                             std::cout << std::format(
                                 "Note(interval={}, velocity={}, delay={}, gate={})\n",
                                 note.interval, note.velocity, note.delay, note.gate);
                         },
                         [](Rest) { std::cout << "Rest\n"; },
                         [&](Sequence const &seq) {
                             std::cout << "Sequence(\n";
                             print_sequence(seq, indent + 1);
                             std::cout << ")\n";
                         },
                     });
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
                      std::cout
                          << std::format("NoteOn(note={}, velocity={})",
                                         (int)note_on.note, (int)note_on.velocity);
                  },
                  [&](midi::NoteOff const &note_off) {
                      std::cout << std::format("NoteOff(note={})", (int)note_off.note);
                  },
                  [&](midi::PitchBend const &pitch_bend) {
                      std::cout
                          << std::format("PitchBend(amount={})", pitch_bend.value);
                  },
              },
              event);
        std::cout << std::format(" | offset={}\n", offset);
    }
}

} // namespace sequence::test::helper
#endif // SEQUENCE_TEST_HELPER_HPP