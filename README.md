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
- `sequence::MusicElement`: the variant payload stored inside a `Cell`; it holds a
  `Note` or nested `Sequence`.
- `sequence::Cell`: a weighted time span containing zero or more simultaneous
  `MusicElement`s.
- `sequence::Sequence`: a nested collection of `Cell`s.
- `sequence::Tuning`: microtonal scale intervals and octave size.

`Cell` is the unit most APIs operate on. A `Sequence` is recursive because each child is
itself a `Cell`, and each `Cell` may contain multiple simultaneous notes or nested
sequences. An empty `Cell.elements` vector represents silence for that span.

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

    auto cell = Cell{
        .elements =
            {Sequence{{
                {.elements = {Note{0, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
                {.elements = {Note{2, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
                {.elements = {Note{4, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
                {.elements = {Note{5, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
            }}},
        .weight = 1.0f,
    };
    auto pattern = Pattern{0, {2}};
    auto shifted = modify::shift_pitch(cell, pattern, 7);

    auto tuning = Tuning{
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100},
        1200.0f,
        "12-EDO",
    };

    auto total_samples = samples_count(TimeSignature{4, 4}, std::uint32_t{48000}, 120.0f);

    auto timeline = midi::flatten_to_midi(
        shifted.elements,
        0,
        total_samples,
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

auto nested_a = Sequence{{
    {.elements = {Note{1, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
}};

auto nested_b = Sequence{{
    {.elements = {Note{2, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
}};

auto cell = Cell{
    .elements =
        {Sequence{{
            {.elements = {Note{0, 0.8f, 0.0f, 1.0f}}, .weight = 1.0f},
            {.elements = {nested_a}, .weight = 1.0f},
            {.elements = {nested_b}, .weight = 1.0f},
        }}},
    .weight = 1.0f,
};

auto shifted = modify::shift_pitch(cell, Pattern{0, {2}}, 12);

// Child 0 is modified.
// Child 1 is skipped entirely, including its nested note.
// Child 2 is visited, and the nested note inside it is modified.
```

## Main Entry Points

- `sequence::modify`: transform existing material by pattern.
- `sequence::from_scala`: load a tuning from a Scala `.scl` file.
- `sequence::samples_count`: derive total duration in samples from a time signature, sample rate, and BPM.
- `sequence::midi::flatten_to_midi`: convert simultaneous recursive music elements into timed MIDI notes over a sample span.

Tests in [`test/`](/Users/anthony/Documents/code/MicrotonalStepSequencer/test) show more
complete usage.
