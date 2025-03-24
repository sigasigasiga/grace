#pragma once

#include <utility>

namespace sgia::fn::bind {

// make a class from a function pointer that doesn't take any space and
// correctly adds it to the overload set
template<auto V>
struct lift_function;

template<typename Ret, typename... Args, bool Noexcept, Ret (*Fn)(Args...) noexcept(Noexcept)>
struct lift_function<Fn>
{
    static constexpr Ret operator()(Args &&...args) noexcept(Noexcept)
    {
        return Fn(std::forward<Args>(args)...);
    }
};

} // namespace sgia::fn::bind
