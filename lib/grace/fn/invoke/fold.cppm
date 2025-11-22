module;

#include <functional>
#include <utility>

export module grace.fn.invoke:fold;

import grace.tuple;

// FIXME: remove `export` when fixed https://gcc.gnu.org/bugzilla/show_bug.cgi?id=122712
export namespace detail {

// https://stackoverflow.com/a/39996086/10961484
class adl_tag {};

template<std::size_t I, typename Tup, typename... Args>
constexpr auto fold_impl(adl_tag, std::index_sequence<I>, Tup &&tup, Args &&...args)
    noexcept(noexcept(std::invoke(get<I>(std::forward<Tup>(tup)), std::forward<Args>(args)...)))
    -> decltype(std::invoke(get<I>(std::forward<Tup>(tup)), std::forward<Args>(args)...))
{
    return std::invoke(get<I>(std::forward<Tup>(tup)), std::forward<Args>(args)...);
}

template<std::size_t I, std::size_t... Is, typename FnTuple, typename... Args>
constexpr auto fold_impl(
    adl_tag,
    std::index_sequence<I, Is...>,
    FnTuple &&tup,
    Args &&...args
)
    noexcept(noexcept(fold_impl(
        adl_tag{},
        std::index_sequence<Is...>{},
        std::forward<FnTuple>(tup),
        std::invoke(get<I>(std::forward<FnTuple>(tup)), std::forward<Args>(args)...)
    )))
    -> decltype(fold_impl(
        adl_tag{},
        std::index_sequence<Is...>{},
        std::forward<FnTuple>(tup),
        std::invoke(get<I>(std::forward<FnTuple>(tup)), std::forward<Args>(args)...)
    ))
{
    return fold_impl(
        adl_tag{},
        std::index_sequence<Is...>{},
        std::forward<FnTuple>(tup),
        std::invoke(get<I>(std::forward<FnTuple>(tup)), std::forward<Args>(args)...)
    );
}

} // namespace detail

export namespace grace::fn::invoke {

template<typename FnTuple, typename ...Args>
constexpr auto fold(FnTuple &&tup, Args &&...args)
    noexcept(noexcept(detail::fold_impl(
        detail::adl_tag{},
        tuple::index_sequence_for_tuple<FnTuple>(),
        std::forward<FnTuple>(tup),
        std::forward<Args>(args)...
    )))
    -> decltype(detail::fold_impl(
        detail::adl_tag{},
        tuple::index_sequence_for_tuple<FnTuple>(),
        std::forward<FnTuple>(tup),
        std::forward<Args>(args)...
    ))
{
    return detail::fold_impl(
        detail::adl_tag{},
        tuple::index_sequence_for_tuple<FnTuple>(),
        std::forward<FnTuple>(tup),
        std::forward<Args>(args)...
    );
}

} // namespace grace::fn::invoke
