export module grace.ranges:make_subrange;

import std;

import grace.tuple;

export namespace grace::ranges {

template<tuple::pair_like P>
[[nodiscard]] constexpr auto make_subrange(P &&p)
    noexcept(noexcept(std::ranges::subrange(get<0>(std::forward<P>(p)), get<1>(std::forward<P>(p)))))
    -> decltype(std::ranges::subrange(get<0>(std::forward<P>(p)), get<1>(std::forward<P>(p))))
{
    return std::ranges::subrange(get<0>(std::forward<P>(p)), get<1>(std::forward<P>(p)));
}

} // namespace grace::ranges
