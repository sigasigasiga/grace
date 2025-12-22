module;

#include <type_traits>
#include <utility>

export module grace.utility:read_only_value;

import :storage_base;

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
    using storage_base<T>::storage_base;

    constexpr read_only_value(const read_only_value &) = default;
    constexpr read_only_value(read_only_value &&) = default;

    constexpr read_only_value &operator=(const read_only_value &) = delete;
    constexpr read_only_value &operator=(read_only_value &&) = delete;

public:
    [[nodiscard]] constexpr const T &get() const noexcept { return storage_base<T>::value(); }

    // Notes:
    // 1. I'm not sure if allowing `release` only for rvalues is a good idea, but IMO it looks nice:
    //    this way it'd be easier to notice that the value would be moved-from after the operation.
    //    However, it's not consistent with STL -- `unique_ptr::release` works for both `&` and `&&`
    // 2. `T &&` is not returned, as the underlying value may be modified using the reference
    [[nodiscard]] constexpr T release() && noexcept { return std::move(*this).value(); }
};

template<typename T>
read_only_value(T) -> read_only_value<T>;

} // namespace grace::utility

using grace::utility::read_only_value;

export template<typename T>
struct std::hash<read_only_value<T>>
{
    [[nodiscard]] constexpr static auto operator()(const read_only_value<T>& v)
        noexcept(noexcept(std::hash<T>{}(v.get())))
        -> decltype(std::hash<T>{}(v.get()))
    {
        return std::hash<T>{}(v.get());
    }
};
