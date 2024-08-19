#include <siga/util/functional.hpp>

namespace {

constexpr void test_storage_assignment()
{
    // it:
    // 1. compiles
    // 2. without warnings
    siga::util::storage_base<int> i;
    i = 3;
    i = 3.0f;
    auto copy = i;
    i = copy;

    siga::util::storage_base<double> d;
    i = d;
    i = std::move(d);
}

template<typename T>
constexpr volatile T &as_volatile(T &value)
{
    return value;
}

bool test_cvref()
{
    class tester
    {
    public:
        constexpr int operator()() & { return 1; }
        constexpr int operator()() const & { return 2; }
        constexpr int operator()() volatile & { return 3; }
        constexpr int operator()() const volatile & { return 4; }
        constexpr int operator()() && { return 5; }
        constexpr int operator()() const && { return 6; }
        constexpr int operator()() volatile && { return 7; }
        constexpr int operator()() const volatile && { return 8; }
    };

    constexpr siga::util::stored_func_invoker inv{tester{}};
    auto copy = inv;
    copy = inv;
    static_assert(std::same_as<const decltype(copy), decltype(inv)>);

    siga::util::stored_func_invoker<std::function<int()>> fn_inv;
    fn_inv = inv;
    fn_inv = [] {
        return 3;
    };

    // clang-format off
    return
        inv() == 1 &&
        std::as_const(inv)() == 2 &&
        as_volatile(inv)() == 3 &&
        std::as_const(as_volatile(inv))() == 4 &&
        std::move(inv)() == 5 &&
        std::move(std::as_const(inv))() == 6 &&
        std::move(as_volatile(inv))() == 7 &&
        std::move(std::as_const(as_volatile(inv)))() == 8
    ;
    // clang-format on
};

} // anonymous namespace

int main()
{
    std::ignore = test_storage_assignment;
    return test_cvref();
}
