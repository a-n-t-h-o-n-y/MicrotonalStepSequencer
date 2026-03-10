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
- `sequence::Sequence`: a nested list of `Cell`s.
- `sequence::Cell`: wraps a `Note`, `Rest`, or nested `Sequence` plus a relative weight.
- `sequence::Tuning`: microtonal scale intervals and octave size.

## Example

```cpp
#include <cstdint>
#include <sequence/generate.hpp>
#include <sequence/midi.hpp>
#include <sequence/modify.hpp>
#include <sequence/pattern.hpp>
#include <sequence/time_signature.hpp>
#include <sequence/tuning.hpp>

int main()
{
    using namespace sequence;

    auto seq = generate::interval(16, 2, 0, Note{0, 0.8f, 0.0f, 0.9f});
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

## Main Entry Points

- `sequence::generate`: create empty, full, interval, or random patterns.
- `sequence::modify`: transform existing material by pattern.
- `sequence::from_scala`: load a tuning from a Scala `.scl` file.
- `sequence::create_cell` / `sequence::samples_count`: derive timing from a time signature.
- `sequence::midi::translate_to_midi_timeline`: convert a timed cell into MIDI events.

Tests in [`test/`](/Users/anthony/Documents/code/MicrotonalStepSequencer/test) show more
complete usage.
