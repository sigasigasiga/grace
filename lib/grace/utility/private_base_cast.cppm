module;

#include <type_traits>

export module grace.utility:private_base_cast;

export namespace grace::utility {

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html#the-shadowing-mitigation-private-inheritance-problem
//
// Notes:
// 1. It is SFINAE-friendly like the builtin casts
// 2. It doesn't differentiate between xvalues and prvalues like the builtin casts
//    (e.g. `const_cast<int &&>(std::move(x))` works, but `const_cast<int &&>(3)` doesn't)
template<typename CvRefTo, typename CvFrom, typename CvTo = std::remove_reference_t<CvRefTo>>
[[nodiscard]] constexpr CvRefTo private_base_cast(CvFrom &from) noexcept
    requires std::is_reference_v<CvRefTo> &&
             std::is_base_of_v<CvTo, CvFrom> &&
             (!std::is_const_v<CvFrom> || std::is_const_v<CvTo>) &&
             (!std::is_volatile_v<CvFrom> || std::is_volatile_v<CvTo>) &&
             requires { (CvRefTo)from; } // check for ambigous base class
{
    return (CvRefTo)from;
}

} // namespace grace::utility
