module;

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

export module grace.utility:default_assign;

namespace default_assign {

template<typename T>
constexpr auto ufcs_swap(T &a, T &b, int)
    noexcept(noexcept(a.swap(b)))
    -> decltype(a.swap(b))
{
    return a.swap(b);
}

template<typename T>
constexpr auto ufcs_swap(T &a, T &b, ...)
    noexcept(noexcept(swap(a, b)))
    -> decltype(swap(a, b))
{
    return swap(a, b);
}

template<typename MutTo, typename FwdFrom>
constexpr MutTo &impl(MutTo &to, FwdFrom &&from)
    noexcept(
        std::is_nothrow_constructible_v<MutTo, FwdFrom &&> &&
        noexcept(ufcs_swap(to, to, 0))
    )
    requires
        std::same_as<MutTo, std::remove_cv_t<MutTo>> &&
        std::constructible_from<MutTo, FwdFrom &&> &&
        requires(MutTo &obj) {
            ufcs_swap(obj, obj, 0);
        }
{
    // TODO: Well, there's a case when `From` is a base class of `To`,
    //       and the address check will not work.
    //       Although I'm not sure if this is a valid case for assignment
    if constexpr (std::same_as<MutTo, std::remove_cvref_t<FwdFrom>>) {
        if (std::addressof(to) == std::addressof(from)) {
            return to;
        }
    }

    MutTo tmp(std::forward<FwdFrom>(from));
    ufcs_swap(to, tmp, 0); // FIXME: it MUST be `noexcept` because otherwise the object will be half-swappeda

    return to;
}

} // namespace default_assign

export namespace grace::utility {

// Generic way to implement assignment for any class that has
// 1. `To(FwdFrom &&)` constructor
// 2. `.swap` method or a `swap(To &, To &)` function available via ADL
//
// Self-assignment is optimized away
//
// Usage:
// ```
// my_class &operator=(T &&rhs) { return default_assign(*this, std::forward<T>(rhs)); }
// ```
template<typename To, typename FwdFrom>
constexpr auto default_assign(To &to, FwdFrom &&from)
    noexcept(noexcept(default_assign::impl(to, std::forward<FwdFrom>(from))))
    -> decltype(default_assign::impl(to, std::forward<FwdFrom>(from)))
{
    return default_assign::impl(to, std::forward<FwdFrom>(from));
}

} // namespace grace::utility
