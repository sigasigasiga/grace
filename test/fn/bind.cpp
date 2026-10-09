#include <functional>
#include <utility>

import grace.fn.bind;

using namespace std::placeholders;

namespace {

struct classify
{
    constexpr int operator()(int &) const { return 1; }
    constexpr int operator()(int &&) const { return 2; }
    constexpr int operator()(int const &) const { return 3; }
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
constexpr int double_it_noexcept(int x) noexcept { return x * 2; }
constexpr int negate_it_noexcept(int x) noexcept { return -x; }

struct non_copyable_non_movable
{
    non_copyable_non_movable() = default;
    non_copyable_non_movable(non_copyable_non_movable const &) = delete;
    non_copyable_non_movable(non_copyable_non_movable &&) = delete;
};

struct throwing_copy
{
    throwing_copy() = default;
    constexpr throwing_copy(throwing_copy const &) noexcept(false) {}
};

struct throwing_copy_fn
{
    throwing_copy_fn() = default;
    constexpr throwing_copy_fn(throwing_copy_fn const &) noexcept(false) {}
    constexpr int operator()() const noexcept { return 0; }
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
        int const cx = 0;

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
        {
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

        {
            auto b = f::bind(std::plus{}, f::bind(double_it, 5), _1);
            if (b(10) != 20) {
                throw "nested bind expression support failed (inner bind with more arguments than outer bind)";
            }

            if (test_invocability(b, 10, 20)) {
                throw "nested bind expression support failed (extra arguments are not okay)";
            }
        }

        {
            auto b = f::bind(std::plus{}, f::bind(double_it, _1), 5);
            if (b(10) != 25) {
                throw "nested bind expression support failed (outer bind with more arguments than inner bind)";
            }

            if (test_invocability(b, 10, 20)) {
                throw "nested bind expression support failed (extra arguments are not okay)";
            }
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

    {
        auto b = f::bind(double_it, _5);

        if (b(0, 0, 0, 0, 21) != 42) {
            throw "bind expression with gaps in placeholders failed";
        }

        if (test_invocability(b, 0, 0, 0)) {
            throw "bind expression with gaps in placeholders should not be invocable with too few arguments";
        }

        if (test_invocability(b, 0, 0, 0, 0, 0, 0)) {
            throw "bind expression with gaps in placeholders should not be invocable with too many arguments";
        }
    }

    // `noexcept` propagation
    {
        // invocation
        {
            auto b = f::bind(double_it_noexcept, _1);
            static_assert(noexcept(b(1)));
            static_assert(noexcept(std::move(b)(1)));

            auto tb = f::bind(double_it, _1);
            static_assert(!noexcept(tb(1)));
            static_assert(!noexcept(std::move(tb)(1)));
        }

        // NTTP-stored callables
        {
            auto b = f::bind<double_it_noexcept>(_1);
            static_assert(noexcept(b(1)));

            auto tb = f::bind<double_it>(_1);
            static_assert(!noexcept(tb(1)));
        }

        // nested bind expressions
        {
            auto b = f::bind(double_it_noexcept, f::bind(negate_it_noexcept, _1));
            static_assert(noexcept(b(1)));

            auto tb_inner = f::bind(double_it_noexcept, f::bind(negate_it, _1));
            static_assert(!noexcept(tb_inner(1)));

            auto tb_outer = f::bind(double_it, f::bind(negate_it_noexcept, _1));
            static_assert(!noexcept(tb_outer(1)));
        }

        // bound arguments are passed by reference, so a throwing copy
        // constructor must not affect invocation
        {
            auto b = f::bind([](throwing_copy const &) noexcept {}, throwing_copy{});
            static_assert(noexcept(b()));
        }

        // `bind()` itself
        {
            throwing_copy tc;
            throwing_copy_fn tcf;

            static_assert(noexcept(f::bind(double_it_noexcept, 1, _1)));
            static_assert(!noexcept(f::bind(double_it_noexcept, tc)));
            static_assert(!noexcept(f::bind(tcf)));
            static_assert(noexcept(f::bind(std::ref(tcf))));
            static_assert(noexcept(f::bind(double_it_noexcept, std::ref(tc))));
        }
    }

    // bound arguments forwarding
    {
        {
            // bound arguments are forwarded according to the binder's own
            // value category
            auto b = f::bind(classify{}, 0);

            if (b() != 1) {
                throw "bound argument lvalue forwarding failed";
            }
            if (std::as_const(b)() != 3) {
                throw "bound argument const lvalue forwarding failed";
            }
            if (std::move(b)() != 2) {
                throw "bound argument rvalue forwarding failed";
            }
            if (std::move(std::as_const(b))() != 3) {
                throw "bound argument const rvalue forwarding failed";
            }
        }

        {
            // `std::reference_wrapper`s are unwrapped and always passed as
            // lvalue references
            int x = 0;
            auto b = f::bind(classify{}, std::ref(x));

            if (b() != 1) {
                throw "reference_wrapper bound argument lvalue forwarding failed";
            }
            if (std::as_const(b)() != 1) {
                throw "reference_wrapper bound argument const lvalue forwarding failed";
            }
            if (std::move(b)() != 1) {
                throw "reference_wrapper bound argument rvalue forwarding failed";
            }

            auto cb = f::bind(classify{}, std::cref(x));
            if (std::move(cb)() != 3) {
                throw "reference_wrapper (const) bound argument forwarding failed";
            }
        }

        {
            // nested bind expressions are invoked according to the outer
            // binder's value category
            auto b = f::bind([](int x) { return x; }, f::bind(multi_qual{}));

            if (b() != 1) {
                throw "nested bind expression lvalue forwarding failed";
            }
            if (std::as_const(b)() != 2) {
                throw "nested bind expression const lvalue forwarding failed";
            }
            if (std::move(b)() != 3) {
                throw "nested bind expression rvalue forwarding failed";
            }
            if (std::move(std::as_const(b))() != 4) {
                throw "nested bind expression const rvalue forwarding failed";
            }
        }
    }
}

} // anonymous namespace

int main() {
    test();
}
