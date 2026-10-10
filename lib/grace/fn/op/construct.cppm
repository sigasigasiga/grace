module;

#include <concepts>
#include <type_traits>
#include <utility>

export module grace.fn.op:construct;

export namespace grace::fn::op {

template<typename T>
requires std::is_object_v<T>
class [[nodiscard]] construct
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
        // Guard against a one-argument case where `T(arg)` could invoke a C-style cast
        // https://cplusplus.github.io/LWG/issue3528
        requires std::constructible_from<T, Args &&...>
    {
        return T(std::forward<Args>(args)...);
    }
};

template<typename T>
requires std::is_object_v<T>
class [[nodiscard]] brace_construct
{
public:
    template<typename... Args>
    [[nodiscard]] static constexpr auto operator()(Args &&...args)
        noexcept(noexcept(T{std::forward<Args>(args)...}))
        -> decltype(T{std::forward<Args>(args)...})
    {
        return T{std::forward<Args>(args)...};
    }
};

} // namespace grace::fn::op
