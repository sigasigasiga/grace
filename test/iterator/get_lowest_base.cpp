#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

import grace.iterator;

using grace::iterator::get_lowest_base;

namespace {

// minimal iterator adaptor whose `base` is `noexcept(Nothrow)`
template<typename It, bool Nothrow = true>
struct wrapper
{
    using difference_type = std::iter_difference_t<It>;
    using value_type = std::iter_value_t<It>;

    It it;

    constexpr decltype(auto) operator*() const { return *it; }
    constexpr wrapper &operator++() { ++it; return *this; }
    constexpr wrapper operator++(int) { auto tmp = *this; ++it; return tmp; }

    constexpr It base() const noexcept(Nothrow) { return it; }
};

// iterator adaptor whose `base` can only be called on rvalues
template<typename It>
struct rvalue_base_wrapper
{
    using difference_type = std::iter_difference_t<It>;
    using value_type = std::iter_value_t<It>;

    It it;

    constexpr decltype(auto) operator*() const { return *it; }
    constexpr rvalue_base_wrapper &operator++() { ++it; return *this; }
    constexpr rvalue_base_wrapper operator++(int) { auto tmp = *this; ++it; return tmp; }

    constexpr It base() && noexcept { return std::move(it); }
};

// not an iterator, but has `base`
struct not_an_iterator
{
    constexpr int *base() const noexcept { return nullptr; }
};

static_assert(std::input_or_output_iterator<wrapper<int *>>);
static_assert(std::input_or_output_iterator<rvalue_base_wrapper<int *>>);
static_assert(!std::input_or_output_iterator<not_an_iterator>);

template<typename T>
concept has_lowest_base = requires (T &&it) { get_lowest_base(std::forward<T>(it)); };

consteval void test() {
    using rit = std::reverse_iterator<int *>;
    using mit = std::move_iterator<int *>;

    int arr[]{1, 2, 3, 4, 5};

    // iterator without `base`, lvalue: returns a reference to the iterator itself
    {
        int *p = arr + 1;
        static_assert(std::same_as<decltype(get_lowest_base(p)), int *&>);
        static_assert(std::same_as<decltype(get_lowest_base(std::as_const(p))), int *const &>);

        if (&get_lowest_base(p) != &p) {
            throw "lvalue iterator without base must be returned by reference";
        }
    }

    // iterator without `base`, rvalue: returns the iterator by value
    {
        int *p = arr + 1;
        static_assert(std::same_as<decltype(get_lowest_base(std::move(p))), int *>);
        static_assert(std::same_as<decltype(get_lowest_base(arr + 1)), int *>);

        if (get_lowest_base(std::move(p)) != arr + 1) {
            throw "rvalue iterator without base returned the wrong iterator";
        }
    }

    // single level of wrapping
    {
        rit r{arr + 3};
        static_assert(std::same_as<decltype(get_lowest_base(r)), int *>);
        static_assert(std::same_as<decltype(get_lowest_base(rit{arr + 3})), int *>);

        if (get_lowest_base(r) != arr + 3) {
            throw "reverse_iterator unwrapped to the wrong iterator";
        }
    }

    // multiple levels of wrapping
    {
        using rrit = std::reverse_iterator<rit>;
        using mrrit = std::move_iterator<rrit>;
        using wmrrit = wrapper<mrrit>;

        wmrrit w{mrrit{rrit{rit{arr + 2}}}};
        static_assert(std::same_as<decltype(get_lowest_base(w)), int *>);

        if (get_lowest_base(w) != arr + 2) {
            throw "nested iterator unwrapped to the wrong iterator";
        }
    }

    // `base` returning a reference, lvalue: the reference is preserved
    {
        mit m{arr + 4};
        static_assert(std::same_as<decltype(get_lowest_base(m)), int *const &>);

        if (&get_lowest_base(m) != &m.base()) {
            throw "lvalue move_iterator must return a reference to its base";
        }
    }

    // `base` returning a reference, rvalue: the base is returned by value
    {
        mit m{arr + 4};
        static_assert(std::same_as<decltype(get_lowest_base(std::move(m))), int *>);

        if (get_lowest_base(std::move(m)) != arr + 4) {
            throw "rvalue move_iterator unwrapped to the wrong iterator";
        }
    }

    // value category is forwarded to `base`
    {
        using w = rvalue_base_wrapper<int *>;
        w it{arr};

        // `base` is not callable on lvalues, so the wrapper itself is the lowest base
        static_assert(std::same_as<decltype(get_lowest_base(it)), w &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::move(it))), int *>);

        if (&get_lowest_base(it) != &it) {
            throw "lvalue with rvalue-only base must not be unwrapped";
        }
        if (get_lowest_base(std::move(it)) != arr) {
            throw "rvalue with rvalue-only base unwrapped to the wrong iterator";
        }
    }

    // SFINAE-friendliness
    {
        static_assert(has_lowest_base<int *>);
        static_assert(has_lowest_base<int *&>);
        static_assert(has_lowest_base<int *const &>);
        static_assert(has_lowest_base<rit>);
        static_assert(has_lowest_base<rit &>);
        static_assert(has_lowest_base<mit>);
        static_assert(has_lowest_base<wrapper<int *>>);
        static_assert(has_lowest_base<std::vector<int>::iterator>);

        static_assert(!has_lowest_base<int>);
        static_assert(!has_lowest_base<std::vector<int>>);
        static_assert(!has_lowest_base<not_an_iterator>);
        static_assert(!has_lowest_base<not_an_iterator &>);
    }

    // `noexcept` propagation
    {
        int *p = arr;
        static_assert(noexcept(get_lowest_base(p)));
        static_assert(noexcept(get_lowest_base(std::move(p))));

        wrapper<int *, true> nothrow_w{arr};
        wrapper<int *, false> throw_w{arr};
        static_assert(noexcept(get_lowest_base(nothrow_w)));
        static_assert(!noexcept(get_lowest_base(throw_w)));

        // a throwing `base` at any level makes the whole call potentially throwing
        wrapper<wrapper<int *, false>, true> outer_nothrow{throw_w};
        wrapper<wrapper<int *, true>, false> outer_throw{nothrow_w};
        static_assert(!noexcept(get_lowest_base(outer_nothrow)));
        static_assert(!noexcept(get_lowest_base(outer_throw)));

        wrapper<wrapper<int *, true>, true> all_nothrow{nothrow_w};
        static_assert(noexcept(get_lowest_base(all_nothrow)));

        if (get_lowest_base(outer_nothrow) != arr
            || get_lowest_base(outer_throw) != arr
            || get_lowest_base(all_nothrow) != arr) {
            throw "nested wrapper unwrapped to the wrong iterator";
        }
    }
}

} // anonymous namespace

int main() {
    test();
}
