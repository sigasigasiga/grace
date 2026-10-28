module;

#include <algorithm>
#include <functional>
#include <tuple>

export module grace.fn.bind:bind;

import grace.meta;
import grace.type_traits;
import grace.utility;

template<std::size_t I>
using p = grace::meta::overload_priority<I>;

template<std::size_t V>
using size_constant = std::integral_constant<std::size_t, V>;

namespace grace::fn::bind {

export template<typename BindExpr>
struct bind_expression_argument_count;

export template<typename BindExpr>
inline constexpr auto bind_expression_argument_count_v = bind_expression_argument_count<BindExpr>::value;

template<
    std::size_t I,
    typename FwdBoundTuple,
    typename ...Args
>
constexpr auto get_arg(p<0>, FwdBoundTuple &&bound, Args &&..._)
    noexcept(noexcept(get<I>(std::forward<FwdBoundTuple>(bound))))
    -> decltype(get<I>(std::forward<FwdBoundTuple>(bound)))
{
    return get<I>(std::forward<FwdBoundTuple>(bound));
}

template<
    std::size_t I,
    typename FwdBoundTuple,
    typename ...Args,
    typename BoundTuple = std::remove_cvref_t<FwdBoundTuple>,
    typename Elem = std::tuple_element_t<I, BoundTuple>,
    int Placeholder = std::is_placeholder_v<Elem>,
    int Idx = Placeholder - 1
>
constexpr auto get_arg(p<1>, FwdBoundTuple &&bound, Args &&...args)
    noexcept
    -> decltype(std::forward<Args...[Idx]>(args...[Idx]))
{
    return std::forward<Args...[Idx]>(args...[Idx]);
}

template<std::size_t ...Is, typename FwdF, typename ...FwdArgs>
constexpr auto invoke_with_some_args(
    std::index_sequence<Is...>,
    FwdF &&fn,
    FwdArgs &&...args
)
    noexcept(noexcept(std::invoke(
        std::forward<FwdF>(fn),
        std::forward<FwdArgs...[Is]>(args...[Is])...
    )))
    -> decltype(std::invoke(
        std::forward<FwdF>(fn),
        std::forward<FwdArgs...[Is]>(args...[Is])...
    ))
{
    return std::invoke(
        std::forward<FwdF>(fn),
        std::forward<FwdArgs...[Is]>(args...[Is])...
    );
}

template<
    std::size_t I,
    typename FwdBoundTuple,
    typename ...Args,
    typename BoundTuple = std::remove_cvref_t<FwdBoundTuple>,
    typename Elem = std::tuple_element_t<I, BoundTuple>,
    std::size_t ArgCount = bind_expression_argument_count<Elem>::value
>
constexpr auto get_arg(p<1>, FwdBoundTuple &&bound, Args &&...args)
    noexcept(noexcept(invoke_with_some_args(
        std::make_index_sequence<ArgCount>{},
        get<I>(std::forward<FwdBoundTuple>(bound)),
        std::forward<Args>(args)...
    )))
    -> decltype(invoke_with_some_args(
        std::make_index_sequence<ArgCount>{},
        get<I>(std::forward<FwdBoundTuple>(bound)),
        std::forward<Args>(args)...
    ))
{
    return invoke_with_some_args(
        std::make_index_sequence<ArgCount>{},
        get<I>(std::forward<FwdBoundTuple>(bound)),
        std::forward<Args>(args)...
    );
}

template<std::size_t ...Is, typename F, typename BoundTuple, typename ...Args>
constexpr auto invoke_helper(
    std::index_sequence<Is...>,
    F &&fn,
    BoundTuple &&bound,
    Args &&...args
)
    noexcept(noexcept(std::invoke(
        std::forward<F>(fn),
        (get_arg<Is>)(
            p<1>{},
            std::forward<BoundTuple>(bound),
            std::forward<Args>(args)...
        )...
    )))
    -> decltype(std::invoke(
        std::forward<F>(fn),
        (get_arg<Is>)(
            p<1>{},
            std::forward<BoundTuple>(bound),
            std::forward<Args>(args)...
        )...
    ))
{
    return std::invoke(
        std::forward<F>(fn),
        (get_arg<Is>)(
            p<1>{},
            std::forward<BoundTuple>(bound),
            std::forward<Args>(args)...
        )...
    );
}

template<std::size_t ArgCount, typename F, typename BoundTuple>
class binder
{
public:
    template<typename FwdF, typename FwdBoundTuple>
    requires
        std::constructible_from<F, FwdF &&> &&
        std::constructible_from<BoundTuple, FwdBoundTuple &&>
    constexpr binder(
        FwdF &&fn,
        FwdBoundTuple &&bound
    )
        noexcept(
            std::is_nothrow_constructible_v<F, FwdF &&> &&
            std::is_nothrow_constructible_v<BoundTuple, FwdBoundTuple &&>
        )
        : m_fn(std::forward<FwdF>(fn))
        , m_bound(std::forward<FwdBoundTuple>(bound))
    {
    }

public:
    template<
        typename Self,
        typename ...Args,
        typename FwdSelf = grace::type_traits::copy_cvref_t<Self &&, binder>
    >
    requires (sizeof...(Args) == ArgCount)
    constexpr auto operator()(this Self &&self, Args &&...args)
        noexcept(noexcept((invoke_helper)(
            std::make_index_sequence<std::tuple_size_v<BoundTuple>>{},
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            grace::utility::private_base_cast<FwdSelf>(self).m_bound,
            std::forward<Args>(args)...
        )))
        -> decltype((invoke_helper)(
            std::make_index_sequence<std::tuple_size_v<BoundTuple>>{},
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            grace::utility::private_base_cast<FwdSelf>(self).m_bound,
            std::forward<Args>(args)...
        ))
    {
        return (invoke_helper)(
            std::make_index_sequence<std::tuple_size_v<BoundTuple>>{},
            grace::utility::private_base_cast<FwdSelf>(self).m_fn,
            grace::utility::private_base_cast<FwdSelf>(self).m_bound,
            std::forward<Args>(args)...
        );
    }

private:
    [[no_unique_address]] F m_fn;
    BoundTuple m_bound;
};

template<
    std::size_t ArgCount,
    typename FwdF,
    typename FwdBoundTuple,
    typename F = std::decay_t<FwdF>, // must `decay` because of the function references
    typename BoundTuple = std::remove_cvref_t<FwdBoundTuple>
>
constexpr auto make_binder(FwdF &&fn, FwdBoundTuple &&tup)
    noexcept(noexcept(binder<ArgCount, F, BoundTuple>(std::forward<FwdF>(fn), std::forward<FwdBoundTuple>(tup))))
    -> decltype(binder<ArgCount, F, BoundTuple>(std::forward<FwdF>(fn), std::forward<FwdBoundTuple>(tup)))
{
    return binder<ArgCount, F, BoundTuple>(std::forward<FwdF>(fn), std::forward<FwdBoundTuple>(tup));
}

template<
    typename BoundArg,
    std::size_t ArgCount = bind_expression_argument_count<BoundArg>::value
>
constexpr std::size_t count_arg(p<1>) {
    return ArgCount;
}

template<
    typename BoundArg,
    int Placeholder = std::is_placeholder_v<BoundArg>
>
constexpr std::size_t count_arg(p<0>) {
    static_assert(Placeholder >= 0, "BoundArg is not a placeholder");
    return static_cast<std::size_t>(Placeholder);
}

template<typename ...FwdBoundArgs>
constexpr auto count_args() {
    return std::ranges::max({0uz, (count_arg<std::remove_cvref_t<FwdBoundArgs>>(p<1>{}))...});
}

template<std::size_t ArgCount, typename ...Rest>
struct bind_expression_argument_count<binder<ArgCount, Rest...>> : size_constant<ArgCount>
{};

// Like `std::bind` but:
// 1. Supports forwarding (i.e. `std::move(bind_result)()` is different from `bind_result()`)
// 2. Must be invoked with exactly `ArgCount` arguments (i.e. `bind<2>(f, _1)(1)` and `bind<2>(f, _1)(1, 2, 3)` are errors).
//    The explicit count must be at least what the placeholders and nested bind expressions need
// 3. Supports `std::placeholders`
// 4. Does not define and does not use `std::is_bind_expression`.
//    It uses `bind_expression_argument_count` instead
export template<
    std::size_t ArgCount,
    typename F,
    typename ...Args
>
requires (ArgCount >= (count_args<Args...>()))
[[nodiscard]] constexpr auto bind(F &&fn, Args &&...args)
    noexcept(noexcept((make_binder<ArgCount>)(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    )))
    -> decltype((make_binder<ArgCount>)(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    ))
{
    // `make_tuple` is not SFINAE-friendly. duh
    return (make_binder<ArgCount>)(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    );
}

export template<
    typename F,
    typename ...Args,
    std::size_t ArgCount = (count_args<Args ...>)()
>
[[nodiscard]] constexpr auto bind(F &&fn, Args &&...args)
    noexcept(noexcept((bind<ArgCount>)(std::forward<F>(fn), std::forward<Args>(args)...)))
    -> decltype((bind<ArgCount>)(std::forward<F>(fn), std::forward<Args>(args)...))
{
    return (bind<ArgCount>)(std::forward<F>(fn), std::forward<Args>(args)...);
}

} // namespace grace::fn::bind
