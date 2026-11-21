export module grace.utility:decay_copy;

import std;

export namespace grace::utility {

// https://en.cppreference.com/w/cpp/standard_library/decay-copy
// works with explicit constructors
template<typename T>
[[nodiscard]] constexpr auto decay_copy(T &&value)
    noexcept(noexcept(std::decay_t<T>(std::forward<T>(value))))
    -> decltype(std::decay_t<T>(std::forward<T>(value)))
{
    return std::decay_t<T>(std::forward<T>(value));
}

} // namespace grace::utility
