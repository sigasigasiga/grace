#include <compare>
#include <concepts>
#include <utility>

import grace.utility;

template<typename L, typename R>
concept equality_comparable_with_ = requires (L const &l, R const &r) { l == r; r == l; l != r; r != l; };

template<typename L, typename R>
concept ordered_with = requires (L const &l, R const &r) { l <=> r; r <=> l; l < r; r < l; };

int main()
{
    using grace::utility::read_only_value;
    static_assert(std::constructible_from<read_only_value<int>, int>);
    static_assert(std::constructible_from<read_only_value<int>, read_only_value<int>>);
    static_assert(!std::assignable_from<read_only_value<int>, int>);
    static_assert(!std::assignable_from<read_only_value<int>, read_only_value<int>>);

    struct no_cmp {};
    struct explicit_bool { constexpr explicit operator bool() const noexcept { return true; } };
    struct weird_eq { constexpr explicit_bool operator==(weird_eq const &) const noexcept { return {}; } };

    static_assert(std::totally_ordered<read_only_value<int>>);
    static_assert(std::three_way_comparable<read_only_value<int>>);
    static_assert(equality_comparable_with_<read_only_value<int>, read_only_value<long>>);
    static_assert(ordered_with<read_only_value<int>, read_only_value<long>>);
    static_assert(!std::equality_comparable<read_only_value<no_cmp>>);
    static_assert(!std::three_way_comparable<read_only_value<no_cmp>>);
    static_assert(read_only_value{weird_eq{}} == read_only_value{weird_eq{}});
    static_assert(!(read_only_value{weird_eq{}} != read_only_value{weird_eq{}}));
    static_assert(read_only_value{1} < read_only_value{2L});
    static_assert((read_only_value{1} <=> read_only_value{1}) == 0);
    static_assert(read_only_value{1} != read_only_value{2});

    static_assert(read_only_value{1} == 1);
    static_assert(1 == read_only_value{1});
    static_assert(read_only_value{1} != 2L);
    static_assert(2L != read_only_value{1});
    static_assert(read_only_value{1} < 2);
    static_assert(0 < read_only_value{1});
    static_assert((0 <=> read_only_value{1}) < 0);
    static_assert(read_only_value{weird_eq{}} == weird_eq{});
    static_assert((weird_eq{} != read_only_value{weird_eq{}}) == false);
    static_assert(read_only_value<read_only_value<int>>{read_only_value{1}} == read_only_value{1});
    static_assert(equality_comparable_with_<read_only_value<int>, long>);
    static_assert(ordered_with<read_only_value<int>, long>);
    static_assert(!equality_comparable_with_<read_only_value<int>, no_cmp>);
    static_assert(!ordered_with<read_only_value<int>, no_cmp>);

    read_only_value v{777};
    if (v.get() != 777) {
        return 1;
    }

    if (std::move(v).release() != 777) {
        return 2;
    }

    constexpr std::hash<read_only_value<int>> h;
    if (h(read_only_value{777}) != std::hash<int>{}(777)) {
        return 3;
    }
}
