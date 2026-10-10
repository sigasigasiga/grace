module;

#include <type_traits>

export module grace.utility:implicit_cast;

export namespace grace::utility {

// 1. I guess there's no way to implement it by writing it three times
// 2. If `To` is `void`, then it is a SFINAE-out case. I'm not sure if this is desired
// TODO: add constraint `!std::reference_converts_from_temporary_v<To, From>`?
template<typename To, typename From>
requires std::is_convertible_v<From, To>
[[nodiscard]] constexpr To implicit_cast(From &&from)
    noexcept(std::is_nothrow_convertible_v<From, To>)
{
    return from;
}

} // namespace grace::utility
