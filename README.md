# Microtonal Step Sequencer

A small C++20 library for building, transforming, timing, and translating nested step
sequences with microtonal tunings.

## Build

This project currently exposes the library target through CMake.

```cmake
add_subdirectory(path/to/MicrotonalStepSequencer)
target_link_libraries(your_target PRIVATE sequence::sequencer)
```

## Core Types

- `sequence::Note`: pitch, velocity, delay, and gate for one note event.
- `sequence::Rest`: an empty step.
- `sequence::MusicElement`: the variant payload stored inside a `Cell`; it holds a
  `Note`, `Rest`, or nested `Sequence`.
- `sequence::Cell`: the weighted wrapper around a `MusicElement`.
- `sequence::Sequence`: a nested collection of `Cell`s.
- `sequence::Tuning`: microtonal scale intervals and octave size.

`Cell` is the unit most APIs operate on. A `Sequence` is recursive because each child is
itself a `Cell`, which means a child can be another `Sequence`.

## Example

```cpp
#include <cstdint>
#include <sequence/midi.hpp>
#include <sequence/modify.hpp>
#include <sequence/pattern.hpp>
#include <sequence/timing.hpp>
#include <sequence/time_signature.hpp>
#include <sequence/tuning.hpp>

int main()
{
    using namespace sequence;

    auto seq = Cell{Sequence{{
        {Note{0, 0.8f, 0.0f, 1.0f}},
        {Note{2, 0.8f, 0.0f, 1.0f}},
        {Note{4, 0.8f, 0.0f, 1.0f}},
        {Note{5, 0.8f, 0.0f, 1.0f}},
    }}};
    auto pattern = Pattern{0, {2}};
    auto cell = modify::shift_pitch(Cell{seq}, pattern, 7);

    auto tuning = Tuning{
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
        1200.0f,
        "12-EDO",
    };

    auto timeline = midi::translate_to_midi_timeline(
        cell,
        TimeSignature{4, 4},
        std::uint32_t{48000},
        120.0f,
        tuning,
        440.0f,
        48.0f
    );

    return timeline.empty() ? 1 : 0;
}
```

## Pattern Semantics

Most `sequence::modify` operations accept a `Pattern` to decide which child cells are
changed at each `Sequence` level.

- Pattern matching happens independently per `Sequence`.
- Recursion only continues through child `Sequence` cells selected by the current
  level's `Pattern`.
- A nested `Sequence` skipped by the parent pattern is left unchanged.

For example, with `Pattern{0, {2}}`, a transform visits child indices `0`, `2`, `4`,
and so on at each visited `Sequence` level:

```cpp
using namespace sequence;

auto cell = Cell{Sequence{{
    {Note{0, 0.8f, 0.0f, 1.0f}},
    {Sequence{{{Note{1, 0.8f, 0.0f, 1.0f}}}}},
    {Sequence{{{Note{2, 0.8f, 0.0f, 1.0f}}}}},
}}};

auto shifted = modify::shift_pitch(cell, Pattern{0, {2}}, 12);

// Child 0 is modified.
// Child 1 is skipped entirely, including its nested note.
// Child 2 is visited, and the nested note inside it is modified.
```

## Main Entry Points

- `sequence::modify`: transform existing material by pattern.
- `sequence::from_scala`: load a tuning from a Scala `.scl` file.
- `sequence::samples_count`: derive total duration in samples from a cell and time signature.
- `sequence::midi::translate_to_midi_timeline`: convert a timed cell into MIDI events.

Tests in [`test/`](/Users/anthony/Documents/code/MicrotonalStepSequencer/test) show more
complete usage.
