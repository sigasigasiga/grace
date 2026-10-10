module;

#include <concepts>
#include <memory>

export module grace.memory:to_address_arr;

import :to_address;

import grace.meta;

namespace grace::memory {

namespace detail::to_address_arr {

template<typename Ptr>
constexpr auto impl(Ptr const &ptr, grace::meta::overload_priority<0>)
    requires requires(std::size_t i) {
        // overload for `unique_ptr<T[]>` and `shared_ptr<T[]>`.
        // it isn't perfect by any means but i guess that's something?
        { ptr[i] } -> std::same_as<typename std::pointer_traits<Ptr>::element_type &>;
        { ptr.get() } -> std::same_as<typename std::pointer_traits<Ptr>::element_type *>;
    }
{
    return ptr.get();
}

template<typename Ptr>
constexpr auto impl(Ptr const &ptr, grace::meta::overload_priority<1>)
    noexcept(noexcept(grace::memory::to_address(ptr)))
    -> decltype(grace::memory::to_address(ptr))
{
    return grace::memory::to_address(ptr);
}

} // namespace detail::to_address_arr

// same as `grace::memory::to_address` but supports `{unique,shared}_ptr<T[]>`
export [[nodiscard]] constexpr auto to_address_arr(auto const &ptr)
    noexcept(noexcept(detail::to_address_arr::impl(ptr, meta::overload_priority<1>{})))
    -> decltype(detail::to_address_arr::impl(ptr, meta::overload_priority<1>{}))
{
    return detail::to_address_arr::impl(ptr, meta::overload_priority<1>{});
}

} // namespace grace::memory
