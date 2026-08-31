module;

#include <algorithm>
#include <functional>
#include <limits>
#include <tuple>

export module grace.fn.bind:bind;

import :lift_invocable;

import grace.meta;
import grace.type_traits;
import grace.utility;

template<std::size_t I>
using p = grace::meta::overload_priority<I>;

template<int V>
using int_constant = std::integral_constant<int, V>;

namespace grace::fn::bind {

export template<typename BindExpr>
struct bind_expression_argument_count : int_constant<std::numeric_limits<int>::max()> {};

export template<typename BindExpr>
inline constexpr auto bind_expression_argument_count_v = bind_expression_argument_count<BindExpr>::value;

template<
    std::size_t I,
    typename FwdBoundTuple,
    typename ...Args>
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
    int Idx = Placeholder - 1>
requires (0 <= Idx && Idx < sizeof...(Args))
constexpr auto get_arg(p<1>, FwdBoundTuple &&bound, Args &&...args)
    noexcept
    -> decltype(auto)
{
    return std::forward<Args...[Idx]>(args...[Idx]);
}

template<
    std::size_t I,
    typename FwdBoundTuple,
    typename ...Args,
    typename BoundTuple = std::remove_cvref_t<FwdBoundTuple>,
    typename Elem = std::tuple_element_t<I, BoundTuple>>
requires std::is_bind_expression_v<Elem>
constexpr auto get_arg(p<1>, FwdBoundTuple &&bound, Args &&...args)
    noexcept(noexcept(std::invoke(get<I>(std::forward<FwdBoundTuple>(bound)), std::forward<Args>(args)...)))
    -> decltype(std::invoke(get<I>(std::forward<FwdBoundTuple>(bound)), std::forward<Args>(args)...))
{
    return std::invoke(get<I>(std::forward<FwdBoundTuple>(bound)), std::forward<Args>(args)...);
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

template<typename F, typename BoundTuple>
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
        : m_fn(std::forward<FwdF>(fn))
        , m_bound(std::forward<FwdBoundTuple>(bound))
    {
    }

public:
    template<
        typename Self,
        typename ...Args,
        typename FwdSelf = grace::type_traits::copy_cvref_t<Self &&, binder>,
        int ArgCount = bind_expression_argument_count_v<binder>>
    requires
        (ArgCount == std::numeric_limits<int>::max()) ||
        (sizeof...(Args) == ArgCount)
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

template<typename F, typename BoundTuple>
binder(F, BoundTuple) -> binder<F, BoundTuple>;

template<typename ...BoundArgs>
constexpr auto count_args() {
    return std::ranges::max(
        {0, (std::is_bind_expression_v<BoundArgs> ? bind_expression_argument_count_v<BoundArgs> : std::is_placeholder_v<BoundArgs>)...}
    );
}

template<typename F, typename ...BoundArgs>
struct bind_expression_argument_count<binder<F, std::tuple<BoundArgs...>>> : int_constant<count_args<BoundArgs...>()>
{};

// Like `std::bind` but:
// 1. Supports forwarding (i.e. `std::move(bind_result)()`)
// 2. Supports NTTP-stored callable (i.e. `bind<some_function>()`)
// 3. Cannot be invoked with more arguments than needed (i.e. `bind(f, _1)(1, 2)` is an error)
export template<typename F, typename ...Args>
constexpr auto bind(F &&fn, Args &&...args)
    noexcept(noexcept(binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    )))
    -> decltype(binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    ))
{
    // `make_tuple` is not SFINAE-friendly. duh
    return binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    );
};

export template<auto F, typename ...Args>
constexpr auto bind(Args &&...args)
    noexcept(noexcept((bind)(lift_invocable<F>{}, std::forward<Args>(args)...)))
    -> decltype((bind)(lift_invocable<F>{}, std::forward<Args>(args)...))
{
    return (bind)(lift_invocable<F>{}, std::forward<Args>(args)...);
}

} // namespace grace::fn::bind

template<typename F, typename BoundTuple>
struct std::is_bind_expression<grace::fn::bind::binder<F, BoundTuple>> : std::true_type {};
