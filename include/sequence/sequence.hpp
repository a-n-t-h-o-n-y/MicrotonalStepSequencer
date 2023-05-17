#ifndef SEQUENCE_SEQUENCE_HPP
#define SEQUENCE_SEQUENCE_HPP
#include <variant>
#include <vector>

namespace sequence
{

using Cell = std::variant<struct NoteOn, struct NoteOff, struct Rest, struct Sequence>;

struct NoteOn
{
    int note_offset; // 0 is scale's base note, 1 is scale's second note, etc.
    float velocity;  // 0.0 to 1.0
    float gate;      // 0.0 to 1.0
};

struct NoteOff
{
};

struct Rest
{
};

struct Sequence
{
    std::vector<Cell> cells;
};

} // namespace sequence
#endif // SEQUENCE_SEQUENCE_HPP