#pragma once

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

template <typename T>
[[nodiscard]]
auto holds(MusicElement const &element) -> bool
{
    return std::holds_alternative<T>(element);
}

template <typename T>
[[nodiscard]]
auto holds(Cell const &cell) -> bool
{
    return cell.elements.size() == 1 && holds<T>(cell.elements.front());
}

template <typename T>
[[nodiscard]]
auto get(MusicElement &element) -> T &
{
    return std::get<T>(element);
}

template <typename T>
[[nodiscard]]
auto get(MusicElement const &element) -> T const &
{
    return std::get<T>(element);
}

template <typename T>
[[nodiscard]]
auto get(Cell &cell) -> T &
{
    return get<T>(cell.elements.front());
}

template <typename T>
[[nodiscard]]
auto get(Cell const &cell) -> T const &
{
    return get<T>(cell.elements.front());
}

template <typename Fn>
concept NoteChecker = requires(Fn fn, Note const &note) {
    { fn(note) } -> std::same_as<void>;
};

template <NoteChecker Fn>
auto check_sequence(MusicElement const &element, Fn &&checker) -> void
{
    using namespace sequence::utility;

    std::visit(overload{
                   [&checker](Note const &note) { checker(note); },
                   [&checker](Sequence const &seq) {
                       for (auto const &cell : seq.cells)
                       {
                           for (auto const &nested : cell.elements)
                           {
                               check_sequence(nested, checker);
                           }
                       }
                   },
               },
               element);
}

template <NoteChecker Fn>
auto check_sequence(Cell const &cell, Fn &&checker) -> void
{
    for (auto const &element : cell.elements)
    {
        check_sequence(element, checker);
    }
}

template <typename Fn>
auto modify_notes(MusicElement &element, Fn &&modifier) -> void
{
    using namespace sequence::utility;

    std::visit(overload{
                   [&modifier](Note &note) { modifier(note); },
                   [&modifier](Sequence &seq) {
                       for (auto &cell : seq.cells)
                       {
                           modify_notes(cell, modifier);
                       }
                   },
               },
               element);
}

template <typename Fn>
auto modify_notes(Cell &cell, Fn &&modifier) -> void
{
    for (auto &element : cell.elements)
    {
        modify_notes(element, modifier);
    }
}

inline auto print_sequence(MusicElement const &element, int indent = 0) -> void
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
                   [&](Sequence const &seq) {
                       std::cout << std::string(indent * 2, ' ') << "Sequence(\n";
                       for (auto const &cell : seq.cells)
                       {
                           print_sequence(cell, indent + 1);
                       }
                       std::cout << std::string(indent * 2, ' ') << ")\n";
                   },
               },
               element);
}

inline auto print_sequence(Cell const &cell, int indent = 0) -> void
{
    std::cout << std::string(indent * 2, ' ') << "Cell(weight=" << cell.weight
              << ", elements=" << cell.elements.size() << ")\n";
    for (auto const &element : cell.elements)
    {
        print_sequence(element, indent + 1);
    }
}

inline auto print_midi_timeline(std::vector<midi::TimedMidiNote> const &timeline)
    -> void
{
    for (auto const &note : timeline)
    {
        std::cout << "TimedMidiNote(begin=" << note.begin << ", end=" << note.end
                  << ", note=" << static_cast<int>(note.note)
                  << ", velocity=" << static_cast<int>(note.velocity)
                  << ", pitch_bend=" << note.pitch_bend << ")\n";
    }
}

} // namespace sequence::test::helper
