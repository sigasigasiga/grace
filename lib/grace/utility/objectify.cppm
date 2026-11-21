export module grace.utility:objectify;

import std;

import :decay_copy;

export namespace grace::utility {

template<typename T>
[[nodiscard]] constexpr auto objectify(T &&v)
    noexcept(noexcept((decay_copy)(std::forward<T>(v))))
    -> decltype((decay_copy)(std::forward<T>(v)))
{
    return (decay_copy)(std::forward<T>(v));
}

template<typename T>
[[nodiscard]] constexpr auto objectify(T &v)
    noexcept(noexcept(std::ref(v)))
    -> decltype(std::ref(v))
{
    return std::ref(v);
}

} // namespace grace::utility
