import std;

import grace.fn.invoke;

#if 0

using grace::fn::invoke::fold;

void noexceptness_all_noexcept()
{
    int (*foo1)(int, double, float) noexcept;
    char *(*bar1)(int) noexcept;
    unsigned *(*baz1)(char *) noexcept;

    auto t = std::make_tuple(foo1, bar1, baz1);
    static_assert(noexcept(fold(t, 1, 2., 3.f)));
}

void noexceptness_middle_isnt()
{
    int (*foo2)(int, double, float) noexcept;
    char *(*bar2)(int);
    unsigned *(*baz2)(char *) noexcept;

    auto t = std::make_tuple(foo2, bar2, baz2);
    static_assert(not noexcept(fold(t, 1, 2., 3.f)));
}

void return_type_check()
{
    int (*foo3)(int, double, float);
    char *(*bar3)(int);
    unsigned *(*baz3)(char *);

    auto t = std::make_tuple(foo3, bar3, baz3);
    using ret_t = decltype(fold(t, 1, 2, 3));
    static_assert(std::same_as<ret_t, unsigned *>);
}
#endif

int main()
{
}
