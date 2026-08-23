#include <memory>
#include <numeric>
#include <ranges>

import grace.ranges;

consteval void test() {
    {
        constexpr auto sz = 10uz;
        auto p = std::make_unique<int[]>(sz);

        auto v = grace::ranges::pointer_view(std::move(p), sz);
        std::ranges::iota(v, 0);

        if (!std::ranges::equal(v, std::views::iota(0, static_cast<int>(sz)))) {
            std::unreachable();
        }

        if (v.size() != sz) {
            std::unreachable();
        }
    }

    {
        constexpr auto sz = 10uz;
        auto p = std::make_unique<int[]>(sz);
        auto sent = p.get() + sz;

        auto v = grace::ranges::pointer_view(std::move(p), sent);
        std::ranges::iota(v, 0);

        if (!std::ranges::equal(v, std::views::iota(0, static_cast<int>(sz)))) {
            std::unreachable();
        }

        if (v.size() != sz) {
            std::unreachable();
        }
    }
}

int main() {
    test();
}
