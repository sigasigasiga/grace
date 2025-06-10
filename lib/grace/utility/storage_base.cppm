module;

#include <concepts>
#include <type_traits>
#include <utility>

export module grace.utility:storage_base;

import :private_base_cast;
import grace.type_traits;

export namespace grace::utility {

template<typename T>
class [[nodiscard]] storage_base
{
private:
    static_assert(!std::same_as<T, std::in_place_t>);

    template<typename U>
    friend class storage_base;

public:
    // NOTE: all these constructors are heavily inspired by the constructors of `std::optional`.
    // Each constructor is annotated according to the `optional`'s constructor on the cppreference:
    // https://en.cppreference.com/w/cpp/utility/optional/optional

    // (1)
    constexpr storage_base() = default;

    // (2)
    constexpr storage_base(const storage_base &) = default;

    // (3)
    constexpr storage_base(storage_base &&) = default;

    // (4)
    template<typename U>
    requires
        std::is_constructible_v<T, const U &> &&
        (!std::is_constructible_v<T, storage_base<U>&>) &&
        (!std::is_constructible_v<T, const storage_base<U>&>) &&
        (!std::is_constructible_v<T, storage_base<U>&&>) &&
        (!std::is_constructible_v<T, const storage_base<U>&&>) &&
        (!std::is_convertible_v<storage_base<U>&, T>) &&
        (!std::is_convertible_v<const storage_base<U>&, T>) &&
        (!std::is_convertible_v<storage_base<U>&&, T>) &&
        (!std::is_convertible_v<const storage_base<U>&&, T>)
    explicit(!std::is_convertible_v<T, const U &>)
    constexpr
    storage_base(const storage_base<U> &value)
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        : value_(value.get())
    {
    }

    // (5)
    template<typename U>
    requires
        std::is_constructible_v<T, const U &> &&
        (!std::is_constructible_v<T, storage_base<U>&>) &&
        (!std::is_constructible_v<T, const storage_base<U>&>) &&
        (!std::is_constructible_v<T, storage_base<U>&&>) &&
        (!std::is_constructible_v<T, const storage_base<U>&&>) &&
        (!std::is_convertible_v<storage_base<U>&, T>) &&
        (!std::is_convertible_v<const storage_base<U>&, T>) &&
        (!std::is_convertible_v<storage_base<U>&&, T>) &&
        (!std::is_convertible_v<const storage_base<U>&&, T>)
    explicit(!std::is_convertible_v<T, U &&>)
    constexpr
    storage_base(storage_base<U> &&value)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(value).get())
    {
    }

    // (6)
    template<typename... Args>
    requires std::is_constructible_v<T, Args...>
    constexpr explicit storage_base(std::in_place_t, Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>) // strengthened
        : value_(std::forward<Args>(args)...)
    {
    }

    // (7)
    template<typename U, typename... Args>
    requires std::is_constructible_v<T, std::initializer_list<U> &, Args...>
    constexpr explicit storage_base(
        std::in_place_t,
        std::initializer_list<U> ilist,
        Args &&...args
    )
        noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>) // strengthened
        : value_(ilist, std::forward<Args>(args)...)
    {
    }

    // (8)
    template<typename U = T>
    requires
        std::is_constructible_v<T, U &&> &&
        (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>) &&
        (!std::same_as<std::remove_cvref_t<U>, storage_base>)
    explicit(!std::is_convertible_v<U &&, T>)
    constexpr
    storage_base(U &&value)
        noexcept(std::is_nothrow_constructible_v<T, U>) // strengthened
        : value_(std::forward<U>(value))
    {
    }

public:
    // https://en.cppreference.com/w/cpp/utility/optional/operator%3D

    // (1)
    // `operator=(std::nullopt)`, don't need to implement it here

    // (2)
    constexpr storage_base &operator=(const storage_base &) = default;

    // (3)
    constexpr storage_base &operator=(storage_base &&) = default;

    // (4)
    template<typename U = T>
    requires
        (!std::same_as<std::remove_cvref_t<U>, storage_base<T>>) &&
        std::is_assignable_v<T &, U>
        // cppref states that there are two more requirements:
        // 1. `std::is_constructible_v<T, U>`,
        //    but it is only needed for the `optional`
        // 2. `(!std::is_scalar_v<T> || !std::same_as<std::decay_t<U>, T>)`
        //    TODO: I don't get what it is for
    constexpr storage_base &operator=(U &&value)
        noexcept(std::is_nothrow_assignable_v<T, U>) // strengthened
    {
        this->value_ = std::forward<U>(value);
        return *this;
    }

    // (5)
    template<typename U>
    requires
        (!std::is_convertible_v<storage_base<U> &, T>) &&
        (!std::is_convertible_v<const storage_base<U> &, T>) &&
        (!std::is_convertible_v<storage_base<U> &&, T>) &&
        (!std::is_convertible_v<const storage_base<U> &&, T>) &&
        (!std::is_assignable_v<T &, storage_base<U> &>) &&
        (!std::is_assignable_v<T &, const storage_base<U> &>) &&
        (!std::is_assignable_v<T &, storage_base<U> &&>) &&
        (!std::is_assignable_v<T &, const storage_base<U> &&>) &&
        std::is_assignable_v<T &, const U &>
    constexpr storage_base &operator=(const storage_base<U> &rhs)
        noexcept(std::is_nothrow_assignable_v<T, const U &>) // strengthened
    {
        this->value_ = rhs.value();
        return *this;
    }

    // (6)
    template<typename U>
    requires
        (!std::is_convertible_v<storage_base<U> &, T>) &&
        (!std::is_convertible_v<const storage_base<U> &, T>) &&
        (!std::is_convertible_v<storage_base<U> &&, T>) &&
        (!std::is_convertible_v<const storage_base<U> &&, T>) &&
        (!std::is_assignable_v<T &, storage_base<U> &>) &&
        (!std::is_assignable_v<T &, const storage_base<U> &>) &&
        (!std::is_assignable_v<T &, storage_base<U> &&>) &&
        (!std::is_assignable_v<T &, const storage_base<U> &&>) &&
        std::is_assignable_v<T &, U &&>
    constexpr storage_base &operator=(storage_base<U> &&rhs)
        noexcept(std::is_nothrow_assignable_v<T, U &&>) // strengthened
    {
        this->value_ = std::move(rhs).value();
        return *this;
    }

protected:
    template<typename Self, typename USelf = type_traits::copy_cvref_t<Self &&, storage_base>>
    [[nodiscard]] constexpr auto &&value(this Self &&self) noexcept
    {
        return (private_base_cast<USelf>)(self).value_;
    }

private:
    [[no_unique_address]] T value_;
};

// As in `optional`, we have a single deducing guide here
// https://en.cppreference.com/w/cpp/utility/optional/deduction_guides
template<typename T>
storage_base(T) -> storage_base<T>;

} // namespace grace::utility
