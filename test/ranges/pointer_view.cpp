#include <memory>

#define GCC_SUX // TODO:

#ifndef GCC_SUX

#include <numeric>
#include <ranges>

import grace.ranges;

#endif // GCC_SUX

consteval void test() {
    constexpr auto sz = 10uz;
    auto p = std::make_unique<int[]>(sz);

#ifndef GCC_SUX
    auto v = grace::ranges::pointer_view(std::move(p), sz);
    std::ranges::iota(v, 0);

    if (!std::ranges::equal(v, std::views::iota(0, static_cast<int>(sz)))) {
        std::unreachable();
    }

    if (v.size() != sz) {
        std::unreachable();
    }
#else // GCC_SUX
    std::ignore = p;
#endif // GCC_SUX
}

int main() {
    test();
}
