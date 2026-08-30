module;

#include <functional>
#include <utility>

export module grace.fn.bind:operators;

import grace.compat;
import grace.fn.op;

import :bind;

namespace grace::fn::bind {

template<typename T>
[[nodiscard]] constexpr auto equal_to(T &&value)
    noexcept(noexcept((bind)(std::equal_to(), std::forward<T>(value), std::placeholders::_1)))
    -> decltype((bind)(std::equal_to(), std::forward<T>(value), std::placeholders::_1))
{
    return (bind)(std::equal_to(), std::forward<T>(value), std::placeholders::_1);
}

// -------------------------------------------------------------------------------------------------

template<typename T>
[[nodiscard]] constexpr auto not_equal_to(T &&value)
    noexcept(noexcept((bind)(std::not_equal_to(), std::forward<T>(value), std::placeholders::_1)))
    -> decltype((bind)(std::not_equal_to(), std::forward<T>(value), std::placeholders::_1))
{
    return (bind)(std::not_equal_to(), std::forward<T>(value), std::placeholders::_1);
}

// -------------------------------------------------------------------------------------------------

template<typename Container>
[[nodiscard]] constexpr auto index_in(Container &&container)
    noexcept(noexcept((bind)(op::subscript(), std::forward<Container>(container), std::placeholders::_1)))
    -> decltype((bind)(op::subscript(), std::forward<Container>(container), std::placeholders::_1))
{
    return (bind)(op::subscript(), std::forward<Container>(container), std::placeholders::_1);
}

// -------------------------------------------------------------------------------------------------

template<typename Idx>
[[nodiscard]] constexpr auto index_with(Idx &&idx)
    noexcept(noexcept((bind)(op::subscript(), std::placeholders::_1, std::forward<Idx>(idx))))
    -> decltype((bind)(op::subscript(), std::placeholders::_1, std::forward<Idx>(idx)))
{
    return (bind)(op::subscript(), std::placeholders::_1, std::forward<Idx>(idx));
}

} // namespace grace::fn::bind
