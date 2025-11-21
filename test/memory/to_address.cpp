#include <memory>
#include <utility>

import grace.memory;

using namespace grace::memory;

void drill_down()
{
    struct foo
    {
        std::shared_ptr<int> member = std::make_shared<int>();
        auto operator->() const { return member; }
    };

    foo f;
    auto x = to_address(f);
    static_assert(std::same_as<decltype(x), int *>);
    if (x != f.member.get()) {
        throw drill_down;
    }
}

struct with_pointer_traits
{};

int g_dummy;

template<>
struct std::pointer_traits<with_pointer_traits>
{
    static int *to_address(with_pointer_traits) { return &g_dummy; }
};

void arrow_with_traits()
{
    struct foo
    {
        auto operator->() const { return with_pointer_traits{}; }
    };

    auto x = to_address(foo{});
    static_assert(std::same_as<decltype(x), int *>);
    if (x != &g_dummy) {
        throw arrow_with_traits;
    }
}

void ptr()
{
    int *x = to_address(&g_dummy);
    if (x != &g_dummy) {
        throw ptr;
    }
}

struct both
{
    float *operator->() const { return 0; }
};

template<>
struct std::pointer_traits<both>
{
    static int *to_address(both) { return &g_dummy; }
};

void test_both()
{
    auto x = to_address(both{});
    static_assert(std::same_as<decltype(x), int *>);
    if (x != &g_dummy) {
        throw test_both;
    }
}

void smart_arr()
{
    auto f = std::make_unique<int[]>(0);
    if (f.get() != to_address_arr(f)) {
        throw smart_arr;
    }

    if (&g_dummy != to_address_arr(&g_dummy)) {
        throw smart_arr;
    }
}

int main()
{
    drill_down();
    arrow_with_traits();
    ptr();
    test_both();
    smart_arr();
}
