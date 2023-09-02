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
 * Use in an ArgInfo, but always as the last parameter given in the Command.
 */
struct Pattern
{
    std::size_t offset;
    std::vector<std::size_t> intervals;
};

/**
 * @brief Parse a Pattern from a given string.
 *
 * @param input The string to parse.
 * @return Pattern The parsed pattern, or std::nullopt if the pattern input is empty.
 * @exception std::invalid_argument Thrown when the input does not conform to the
 * expected pattern.
 */
[[nodiscard]] auto parse_pattern(std::string const &input) -> std::optional<Pattern>;

/**
 * @brief Strip all characters that are not part of a Pattern prefix.
 *
 * @param x The string to strip.
 * @return std::string The stripped string.
 */
[[nodiscard]] auto strip_pattern_chars(std::string const &x) -> std::string;

/**
 * @brief Extract the pattern string from the given string.
 *
 * @param x The string to extract the pattern from.
 * @return std::string The extracted pattern.
 */
[[nodiscard]] auto extract_pattern_str(std::string const &x) -> std::string;

template <typename T>
class PatternView
{
  public:
    /**
     * @param vec Vector of elements to apply pattern on
     * @param pattern The pattern to use for iteration
     */
    PatternView(std::vector<T> &vec, Pattern const &pattern)
        : vec_(vec), pattern_(pattern), current_index_(pattern.offset)
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
            : pattern_view_(pattern_view), index_(index), interval_index_(0)
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
        return Iterator(*this, current_index_);
    }

    [[nodiscard]] auto end() -> Iterator
    {
        return Iterator(*this, vec_.size());
    }

  private:
    std::vector<T> &vec_;
    Pattern const &pattern_;
    std::size_t current_index_;
};

} // namespace sequence