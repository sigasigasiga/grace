export module grace.fn.op:cast;

import std;

export namespace grace::fn::op {

template<typename To>
class [[nodiscard]] static_value_cast
{
public:
    template<typename From>
    [[nodiscard]] static constexpr auto operator()(From &&from)
        noexcept(noexcept(static_cast<To>(std::forward<From>(from))))
        -> decltype(static_cast<To>(std::forward<From>(from)))
    {
        return static_cast<To>(std::forward<From>(from));
    }
};

// -------------------------------------------------------------------------------------------------

template<typename To>
class [[nodiscard]] dynamic_value_cast
{
public:
    template<typename From>
    [[nodiscard]] static constexpr auto operator()(From &&from)
        noexcept(noexcept(dynamic_cast<To>(std::forward<From>(from))))
        -> decltype(dynamic_cast<To>(std::forward<From>(from)))
    {
        return dynamic_cast<To>(std::forward<From>(from));
    }
};

// -------------------------------------------------------------------------------------------------

template<typename To>
class [[nodiscard]] const_value_cast
{
public:
    template<typename From>
    [[nodiscard]] static constexpr auto operator()(From &&from)
        noexcept(noexcept(const_cast<To>(std::forward<From>(from))))
        -> decltype(const_cast<To>(std::forward<From>(from)))
    {
        return const_cast<To>(std::forward<From>(from));
    }
};

// -------------------------------------------------------------------------------------------------

template<typename To>
class [[nodiscard]] reinterpret_value_cast
{
public:
    template<typename From>
    [[nodiscard]] static constexpr auto operator()(From &&from)
        noexcept(noexcept(reinterpret_cast<To>(std::forward<From>(from))))
        -> decltype(reinterpret_cast<To>(std::forward<From>(from)))
    {
        return reinterpret_cast<To>(std::forward<From>(from));
    }
};

// -------------------------------------------------------------------------------------------------

template<typename To>
class [[nodiscard]] c_style_cast
{
public:
    template<typename From>
    [[nodiscard]] static constexpr auto operator()(From &&from)
        noexcept(noexcept((To)std::forward<From>(from)))
        -> decltype((To)std::forward<From>(from))
    {
        return (To)std::forward<From>(from);
    }
};

} // namespace grace::fn::op
