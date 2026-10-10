module;

#include <concepts>
#include <type_traits>
#include <utility>

export module grace.fn.op:construct;

export namespace grace::fn::op {

template<typename T, bool UseRoundBrackets = true>
class [[nodiscard]] construct
{
public:
    // cannot `return T(args...)`, as it invokes a C-style cast
    // instead of calling a constructor if there's only one arg
    template<typename... Args>
    [[nodiscard]] static constexpr T operator()(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
        requires std::constructible_from<T, Args &&...>
    {
        T ret(std::forward<Args>(args)...);
        return ret;
    }
};

// FIXME: that should be a different class rather than a specialization imo
template<typename T>
class [[nodiscard]] construct<T, false>
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
