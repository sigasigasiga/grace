module;

#include <functional>

export module grace.tuple:for_each_in_tuple;

import :index_sequence_for_tuple;

namespace grace::tuple {

namespace detail::for_each_in_tuple {

template<std::size_t ...Is, typename Fn, typename Tup>
constexpr auto impl(std::index_sequence<Is...>, Fn &&fn, Tup &&tup)
    noexcept(noexcept((..., static_cast<void>(std::invoke(fn, get<Is>(std::forward<Tup>(tup)))))))
    -> decltype((..., static_cast<void>(std::invoke(fn, get<Is>(std::forward<Tup>(tup))))))
{
    return (..., static_cast<void>(std::invoke(fn, get<Is>(std::forward<Tup>(tup)))));
}

} // namespace detail::for_each_in_tuple

export template<typename Tuple, typename Fn>
constexpr auto for_each_in_tuple(Fn &&fn, Tuple &&tuple)
    noexcept(noexcept(detail::for_each_in_tuple::impl(
        (index_sequence_for_tuple<Tuple>)(),
        std::forward<Fn>(fn),
        std::forward<Tuple>(tuple)
    )))
    -> decltype(detail::for_each_in_tuple::impl(
        (index_sequence_for_tuple<Tuple>)(),
        std::forward<Fn>(fn),
        std::forward<Tuple>(tuple)
    ))
{
    return detail::for_each_in_tuple::impl(
        (index_sequence_for_tuple<Tuple>)(),
        std::forward<Fn>(fn),
        std::forward<Tuple>(tuple)
    );
}

} // namespace grace::tuple
