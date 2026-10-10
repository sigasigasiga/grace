module;

#include <compare>
#include <functional>
#include <type_traits>
#include <utility>

export module grace.utility:read_only_value;

import :storage_base;
import :private_base_cast;
import grace.type_traits;

export namespace grace::utility {

// In Rust mutability is not part of a type but a part of a variable.
// That's an attempt to do something similar to that.
//
// Why `read_only_value` may be better than `const` in some scenarios:
// 1. It is move-constructible
// 2. Like in Rust, you can make the value mutable by moving it to a mutable variable
//
//    Rust: `let x = "".to_string(); let mut y = x;`
//    C++: `auto x = read_only_value(std::string()); auto y = std::move(x).release();`
//
//    In both examples `x` is moved-from and `y` is a new mutable variable
template<typename T>
class read_only_value : private storage_base<T>
{
public:
    static_assert(std::is_object_v<T>);
    static_assert(!std::is_array_v<T>);
    static_assert(!std::is_const_v<T>);

public:
    using value_type = T;

public:
    using storage_base<T>::storage_base;

    constexpr read_only_value(read_only_value const &) = default;
    constexpr read_only_value(read_only_value &&) = default;

    constexpr read_only_value &operator=(read_only_value const &) = delete;
    constexpr read_only_value &operator=(read_only_value &&) = delete;

public:
    [[nodiscard]] constexpr T const &get() const noexcept { return storage_base<T>::value(); }

    // Notes:
    // 1. I'm not sure if allowing `release` only for rvalues is a good idea, but IMO it looks nice:
    //    this way it'd be easier to notice that the value would be moved-from after the operation.
    //    However, it's not consistent with STL -- `unique_ptr::release` works for both `&` and `&&`
    // 2. `T &&` is not returned, as the underlying value may be modified using the reference
    [[nodiscard]] constexpr T release() && noexcept { return std::move(*this).value(); }
};

} // namespace grace::utility

namespace grace::utility::detail {

template<typename T>
constexpr bool is_read_only_value_v = false;

template<typename T>
constexpr bool is_read_only_value_v<read_only_value<T>> = true;

// Only one level is unwrapped, so `read_only_value<read_only_value<T>>` is compared as `read_only_value<T>`
template<typename T>
[[nodiscard]] constexpr T const &unwrap_read_only_value(T const &v) noexcept
{
    return v;
}

template<typename T>
[[nodiscard]] constexpr T const &unwrap_read_only_value(read_only_value<T> const &v) noexcept
{
    return v.get();
}

} // namespace grace::utility::detail

export namespace grace::utility {

// Covers `read_only_value<T> @ read_only_value<U>`, `read_only_value<T> @ U` and `U @ read_only_value<T>`.
// The remaining operators are provided by the rewritten candidates.
//
// `bool` is returned, as the rewritten `!=` requires `==` to return `bool`
template<typename L, typename R>
requires
    (detail::is_read_only_value_v<L> || detail::is_read_only_value_v<R>) &&
    requires (L const &l, R const &r) {
        static_cast<bool>(detail::unwrap_read_only_value(l) == detail::unwrap_read_only_value(r));
    }
[[nodiscard]] constexpr bool operator==(L const &lhs, R const &rhs)
    noexcept(noexcept(
        static_cast<bool>(detail::unwrap_read_only_value(lhs) == detail::unwrap_read_only_value(rhs))
    ))
{
    return static_cast<bool>(detail::unwrap_read_only_value(lhs) == detail::unwrap_read_only_value(rhs));
}

template<typename L, typename R>
requires (detail::is_read_only_value_v<L> || detail::is_read_only_value_v<R>)
[[nodiscard]] constexpr auto operator<=>(L const &lhs, R const &rhs)
    noexcept(noexcept(detail::unwrap_read_only_value(lhs) <=> detail::unwrap_read_only_value(rhs)))
    -> decltype(detail::unwrap_read_only_value(lhs) <=> detail::unwrap_read_only_value(rhs))
{
    return detail::unwrap_read_only_value(lhs) <=> detail::unwrap_read_only_value(rhs);
}

template<typename T>
read_only_value(T) -> read_only_value<T>;

} // namespace grace::utility

export template<typename T>
struct std::hash<grace::utility::read_only_value<T>> : private std::hash<T>
{
public:
    using std::hash<T>::hash;

public:
    template<
        typename Self,
        typename FwdBase = grace::type_traits::copy_cvref_t<Self &&, std::hash<T>>
    >
    [[nodiscard]] constexpr auto operator()(
        this Self &&self,
        grace::utility::read_only_value<T> const & v
    )
        noexcept(noexcept(grace::utility::private_base_cast<FwdBase>(self)(v.get())))
        -> decltype(grace::utility::private_base_cast<FwdBase>(self)(v.get()))
    {
        return grace::utility::private_base_cast<FwdBase>(self)(v.get());
    }
};
