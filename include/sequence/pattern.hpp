#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace sequence
{

/**
 * @brief A struct that contains information about a Command line pattern.
 *
 * This pattern describes which children in a Sequence an Action should be applied to.
 *
 * The pattern is defined by an offset and a list of intervals. The offset is the
 * starting index in the Sequence. The intervals are the steps between each index
 * that should be applied to. `intervals` has a valid range of [1, Sequence.size()] and
 * must be at least 1 in length.
 */
struct Pattern
{
    std::size_t offset;
    std::vector<std::size_t> intervals;
};

[[nodiscard]] inline auto operator==(Pattern const &lhs, Pattern const &rhs) -> bool
{
    return lhs.offset == rhs.offset && lhs.intervals == rhs.intervals;
}

[[nodiscard]] inline auto operator!=(Pattern const &lhs, Pattern const &rhs) -> bool
{
    return !(lhs == rhs);
}

/**
 * @brief Check if a command string contains a valid pattern.
 *
 * @param input The command string to check.
 * @return true If the command string contains a valid pattern.
 */
[[nodiscard]] auto contains_valid_pattern(std::string const &input) -> bool;

/**
 * @brief Parse a Pattern from a given string.
 *
 * @param input The string to parse.
 * @return Pattern The parsed pattern.
 * @exception std::invalid_argument Thrown when the input does not conform to the
 * pattern specification.
 */
[[nodiscard]] auto parse_pattern(std::string const &input) -> Pattern;

/**
 * @brief Strip all characters that are part of a Pattern prefix.
 *
 * @param x The string to strip.
 * @return std::string The stripped string without the Pattern prefix.
 */
[[nodiscard]] auto pop_pattern_chars(std::string const &x) -> std::string;

template <typename T>
class PatternView
{
  public:
    /**
     * @param vec Vector of elements to apply pattern on
     * @param pattern The pattern to use for iteration
     */
    PatternView(std::vector<T> &vec, Pattern const &pattern)
        : vec_{vec}, pattern_{pattern}, offset_index_{pattern.offset}
    {
        if (pattern_.intervals.empty() || vec_.empty())
        {
            throw std::invalid_argument("Pattern or vector should not be empty.");
        }
    }

    class Iterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        /**
         * @param pattern_view The PatternView object being iterated
         * @param index Initial index in vector based on pattern
         */
        Iterator(PatternView &pattern_view, std::size_t index)
            : pattern_view_{pattern_view},
              index_{std::min(index, pattern_view.vec_.size())}, interval_index_{0}
        {
        }

        [[nodiscard]] auto operator*() const -> T &
        {
            return pattern_view_.vec_[index_];
        }

        auto operator++() -> Iterator &
        {
            auto const interval = pattern_view_.pattern_.intervals[interval_index_];
            index_ += interval;
            index_ = std::min(index_, pattern_view_.vec_.size());

            ++interval_index_;
            if (interval_index_ >= pattern_view_.pattern_.intervals.size())
            {
                interval_index_ = 0;
            }

            return *this;
        }

        [[nodiscard]] auto operator!=(Iterator const &other) const -> bool
        {
            return index_ != other.index_;
        }

      private:
        PatternView &pattern_view_;
        std::size_t index_;
        std::size_t interval_index_;
    };

    [[nodiscard]] auto begin() -> Iterator
    {
        return Iterator(*this, offset_index_);
    }

    [[nodiscard]] auto end() -> Iterator
    {
        return Iterator(*this, vec_.size());
    }

  private:
    std::vector<T> &vec_;
    Pattern const &pattern_;
    std::size_t offset_index_;
};

template <typename T>
class ConstPatternView
{
  public:
    /**
     * @param vec Vector of elements to apply pattern on
     * @param pattern The pattern to use for iteration
     */
    ConstPatternView(std::vector<T> const &vec, Pattern const &pattern)
        : vec_(vec), pattern_(pattern), offset_index_(pattern.offset)
    {
        if (pattern_.intervals.empty() || vec_.empty())
        {
            throw std::invalid_argument("Pattern or vector should not be empty.");
        }
    }

    class ConstIterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T const *;
        using reference = T const &;

        /**
         * @param pattern_view The ConstPatternView object being iterated
         * @param index Initial index in vector based on pattern
         */
        ConstIterator(ConstPatternView const &pattern_view, std::size_t index)
            : pattern_view_(pattern_view), index_(index), interval_index_(0)
        {
        }

        [[nodiscard]] auto operator*() const -> reference
        {
            return pattern_view_.vec_[index_];
        }

        auto operator++() -> ConstIterator &
        {
            auto const interval = pattern_view_.pattern_.intervals[interval_index_];
            index_ += interval;
            index_ = std::min(index_, pattern_view_.vec_.size());

            ++interval_index_;
            if (interval_index_ >= pattern_view_.pattern_.intervals.size())
            {
                interval_index_ = 0;
            }

            return *this;
        }

        [[nodiscard]] auto operator!=(ConstIterator const &other) const -> bool
        {
            return index_ != other.index_;
        }

      private:
        ConstPatternView const &pattern_view_;
        std::size_t index_;
        std::size_t interval_index_;
    };

    [[nodiscard]] auto begin() const -> ConstIterator
    {
        return ConstIterator(*this, offset_index_);
    }

    [[nodiscard]] auto cbegin() const -> ConstIterator
    {
        return ConstIterator(*this, offset_index_);
    }

    [[nodiscard]] auto end() const -> ConstIterator
    {
        return ConstIterator(*this, vec_.size());
    }

    [[nodiscard]] auto cend() const -> ConstIterator
    {
        return ConstIterator(*this, vec_.size());
    }

  private:
    std::vector<T> const &vec_;
    Pattern const &pattern_;
    std::size_t offset_index_;
};

} // namespace sequence
