#include <concepts>
#include <utility>

import grace.utility;

int main()
{
    using grace::utility::read_only_value;
    static_assert(std::constructible_from<read_only_value<int>, int>);
    static_assert(std::constructible_from<read_only_value<int>, read_only_value<int>>);
    static_assert(!std::assignable_from<read_only_value<int>, int>);
    static_assert(!std::assignable_from<read_only_value<int>, read_only_value<int>>);
    static_assert(std::equality_comparable_with<read_only_value<int>, int>);
    static_assert(!std::equality_comparable_with<read_only_value<int>, struct unrelated &>);

    read_only_value v{777};
    if (v.get() != 777) {
        return 1;
    }

    constexpr std::hash<read_only_value<int>> h;
    if (h(read_only_value{777}) != std::hash<int>{}(777)) {
        return 2;
    }

    if (v <= 776) {
        return 3;
    }

    if (v != 777) {
        return 4;
    }

    if (short(777) != v) {
        return 5;
    }

    if (v != read_only_value(777)) {
        return 6;
    }

    if (read_only_value<short>(777) != v) {
        return 7;
    }

    if (v < 0) {
        return 8;
    }

    if (short(0) > v) {
        return 9;
    }

    if (read_only_value(10) < read_only_value(1)) {
        return 10;
    }

    if (read_only_value<short>(10) < read_only_value<int>(2)) {
        return 11;
    }

    if (std::move(v).release() != 777) {
        return 100;
    }
}
