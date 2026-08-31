#include <functional>
#include <utility>

import grace.fn.bind;

using namespace std::placeholders;

namespace {

struct classify
{
    constexpr int operator()(int &) const { return 1; }
    constexpr int operator()(int &&) const { return 2; }
    constexpr int operator()(const int &) const { return 3; }
};

struct multi_qual
{
    constexpr int operator()() & { return 1; }
    constexpr int operator()() const & { return 2; }
    constexpr int operator()() && { return 3; }
    constexpr int operator()() const && { return 4; }
};

constexpr int double_it(int x) { return x * 2; }
constexpr int negate_it(int x) { return -x; }

struct non_copyable_non_movable
{
    non_copyable_non_movable() = default;
    non_copyable_non_movable(const non_copyable_non_movable &) = delete;
    non_copyable_non_movable(non_copyable_non_movable &&) = delete;
};

template<typename F, typename ...Args>
requires requires (F &&f, Args &&...args) { std::invoke(std::forward<F>(f), std::forward<Args>(args)...); }
constexpr std::true_type test_invocability(F &&f, Args &&...args) { return {}; }

template<typename F, typename ...Args>
constexpr std::false_type test_invocability(F &&f, Args &&...args) { return {}; }

// SFINAE-friendliness
// `bind()` itself must be SFINAE-friendly: if the callable cannot be
// stored, `bind(...)` must simply not be a viable expression rather than
// a hard error.
template<typename T>
concept is_bindable = requires (T &&t) { grace::fn::bind::bind(std::forward<T>(t)); };

consteval void test() {
    namespace f = grace::fn::bind;

    // extra args + SFINAE-friendliness
    {
        auto b = f::bind(double_it, _1);

        if (!test_invocability(b, 21)) {
            throw "bind expression should be invocable with 1 argument";
        }

        if (test_invocability(b, 21, 42)) {
            throw "bind expression should not be invocable with 2 arguments";
        }

        static_assert(!is_bindable<non_copyable_non_movable &>);
        static_assert(is_bindable<std::reference_wrapper<non_copyable_non_movable>>);

    }

    // forwarding semantics
    {
        auto b = f::bind(classify{}, _1);

        int x = 0;
        const int cx = 0;

        if (b(x) != 1) {
            throw "lvalue argument forwarding failed";
        }
        if (b(std::move(x)) != 2) {
            throw "rvalue argument forwarding failed";
        }
        if (b(cx) != 3) {
            throw "const lvalue argument forwarding failed";
        }
    }

    {
        // the bind result itself must forward according to its own value
        // category (i.e. `std::move(bind(...))()` must be well-formed and
        // pick the rvalue-qualified overload of the stored callable).
        auto b = f::bind(multi_qual{});

        if (b() != 1) {
            throw "binder lvalue forwarding failed";
        }
        if (std::as_const(b)() != 2) {
            throw "binder const lvalue forwarding failed";
        }
        if (std::move(b)() != 3) {
            throw "binder rvalue forwarding failed";
        }
        if (std::move(std::as_const(b))() != 4) {
            throw "binder const rvalue forwarding failed";
        }
    }

    // NTTP-stored callables
    {
        if (f::bind<double_it>(_1)(21) != 42) {
            throw "NTTP-stored callable failed";
        }

        // fully bound, no placeholders
        if (f::bind<double_it>(5)() != 10) {
            throw "NTTP-stored callable with bound argument failed";
        }
    }

    // nested bind expression support
    {
        // bind(bind(f, _1), bind(g, _2)):
        //   invoking with (x, y) computes `bind(g, _2)(x, y)` == g(y) first,
        //   then feeds that single result into `bind(f, _1)`, i.e. f(g(y)).
        auto b = f::bind(f::bind(double_it, _1), f::bind(negate_it, _2));

        if (b(100, 5) != -10) {
            throw "nested bind expression support failed";
        }

        if (test_invocability(b, 100, 5, 2, 3, 4)) {
            throw "nested bind expression support failed (extra arguments are not okay)";
        }

        if (test_invocability(b, 100)) {
            throw "nested bind expression support failed (not enough arguments is not okay)";
        }
    }

    // nested std::bind support
    {
        auto b = f::bind(double_it, std::bind(negate_it, _1));

        if (b(5) != -10) {
            throw "nested std::bind support failed";
        }

        if (b(5, 2, 3, 4) != -10) {
            throw "nested std::bind support failed (extra arguments are okay)";
        }
    }

    {
        auto b = f::bind([](auto, auto) {}, _1, _2);

        if (test_invocability(b, 1)) {
            throw "bind expression should not be invocable with 1 argument";
        }

        if (!test_invocability(b, 1, 2)) {
            throw "bind expression should be invocable with 2 arguments";
        }

        if (test_invocability(b, 1, 2, 3)) {
            throw "bind expression should not be invocable with 3 arguments";
        }
    }
}

} // anonymous namespace

int main() {
    test();
}
