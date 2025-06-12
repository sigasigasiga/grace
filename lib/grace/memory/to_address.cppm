module;

#include <memory>

export module grace.memory:to_address;

import grace.meta;

namespace to_address {

template<std::uintmax_t N>
using p = grace::meta::overload_priority<N>;

class adl_tag
{};

template<typename Ptr>
constexpr auto impl(adl_tag, const Ptr &ptr, p<0>)
    noexcept(noexcept(impl(adl_tag{}, ptr.operator->(), p<2>{})))
    -> decltype(impl(adl_tag{}, ptr.operator->(), p<2>{}))
{
    return impl(adl_tag{}, ptr.operator->(), p<2>{});
}

template<typename Ptr>
constexpr auto impl(adl_tag, const Ptr &ptr, p<1>)
    noexcept(noexcept(std::pointer_traits<Ptr>::to_address(ptr)))
    -> decltype(std::pointer_traits<Ptr>::to_address(ptr))
{
    return std::pointer_traits<Ptr>::to_address(ptr);
}

template<typename T>
constexpr T *impl(adl_tag, T *ptr, p<2>) noexcept
{
    static_assert(!std::is_function_v<T>, "STL prohibits function pointers in `to_address`");
    return ptr;
}

} // namespace to_address

export namespace grace::memory {

// like `std::to_address` but is SFINAE-friendly
[[nodiscard]] constexpr auto to_address(const auto &ptr)
    noexcept(noexcept(to_address::impl(to_address::adl_tag{}, ptr, to_address::p<2>{})))
    -> decltype(to_address::impl(to_address::adl_tag{}, ptr, to_address::p<2>{}))
{
    return to_address::impl(to_address::adl_tag{}, ptr, to_address::p<2>{});
}

} // namespace grace::memory
