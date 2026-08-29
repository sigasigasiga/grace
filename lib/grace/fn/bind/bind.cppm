module;

#include <functional>
#include <tuple>

export module grace.fn.bind:bind;

import :lift_invocable;

import grace.meta;
import grace.type_traits;
import grace.utility;

namespace bind_impl {

template<std::size_t I>
using p = grace::meta::overload_priority<I>;

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
    int Placeholder = std::is_placeholder_v<Elem>>
requires (Placeholder > 0)
constexpr auto get_arg(p<1>, FwdBoundTuple &&bound, Args &&...args)
    noexcept
    -> decltype(auto)
{
    constexpr std::size_t idx = Placeholder - 1;
    return std::forward<Args...[idx]>(args...[idx]);
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
        typename FwdSelf = grace::type_traits::copy_cvref_t<Self &&, binder>>
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

} // namespace bind_impl

export template<typename F, typename BoundTuple>
class std::is_bind_expression<bind_impl::binder<F, BoundTuple>> : std::true_type {};

export namespace grace::fn::bind {

// Like `std::bind` but:
// 1. Supports forwarding (i.e. `std::move(bind_result)()`)
// 2. Supports NTTP-stored callable (i.e. `bind<some_function>()`)
template<typename F, typename ...Args>
constexpr auto bind(F &&fn, Args &&...args)
    noexcept(noexcept(bind_impl::binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    )))
    -> decltype(bind_impl::binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    ))
{
    // `make_tuple` is not `constexpr`. duh
    return bind_impl::binder(
        std::forward<F>(fn),
        std::tuple<std::unwrap_ref_decay_t<Args>...>(std::forward<Args>(args)...)
    );
};

template<auto F, typename ...Args>
constexpr auto bind(Args &&...args)
    noexcept(noexcept((bind)(lift_invocable<F>{}, std::forward<Args>(args)...)))
    -> decltype((bind)(lift_invocable<F>{}, std::forward<Args>(args)...))
{
    return (bind)(lift_invocable<F>{}, std::forward<Args>(args)...);
}

} // namespace grace::fn::bind
