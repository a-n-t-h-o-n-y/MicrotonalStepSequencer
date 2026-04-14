#pragma once

#include <cstdint>
#include <limits>
#include <mutex>
#include <optional>
#include <random>

namespace sequence::random
{

namespace detail
{

inline auto seed_mutex() -> std::mutex &
{
    static auto mutex = std::mutex{};
    return mutex;
}

inline auto configured_seed() -> std::optional<std::uint32_t> &
{
    static auto seed = std::optional<std::uint32_t>{};
    return seed;
}

inline auto seed_version() -> std::uint64_t &
{
    static auto version = std::uint64_t{0};
    return version;
}

inline thread_local auto local_seed_version = std::numeric_limits<std::uint64_t>::max();
inline thread_local auto rng = std::mt19937{std::random_device{}()};

inline auto sync_rng() -> void
{
    auto const lock = std::lock_guard{seed_mutex()};
    if (local_seed_version == seed_version())
    {
        return;
    }

    if (auto const seed = configured_seed(); seed.has_value())
    {
        rng.seed(*seed);
    }
    else
    {
        rng.seed(std::random_device{}());
    }

    local_seed_version = seed_version();
}

} // namespace detail

inline auto set_seed(std::uint32_t seed) -> void
{
    auto const lock = std::lock_guard{detail::seed_mutex()};
    detail::configured_seed() = seed;
    ++detail::seed_version();
}

inline auto clear_seed() -> void
{
    auto const lock = std::lock_guard{detail::seed_mutex()};
    detail::configured_seed().reset();
    ++detail::seed_version();
}

[[nodiscard]] inline auto get_seed() -> std::optional<std::uint32_t>
{
    auto const lock = std::lock_guard{detail::seed_mutex()};
    return detail::configured_seed();
}

[[nodiscard]] inline auto engine() -> std::mt19937 &
{
    detail::sync_rng();
    return detail::rng;
}

} // namespace sequence::random
