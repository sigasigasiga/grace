#include <concepts>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

import grace.ranges;

using grace::ranges::get_lowest_base;

namespace {

// minimal range adaptor whose `base` is `noexcept(Nothrow)`
template<typename R, bool Nothrow = true>
struct wrapper
{
    R r;

    constexpr auto begin() const { return std::ranges::begin(r); }
    constexpr auto end() const { return std::ranges::end(r); }

    constexpr R base() const noexcept(Nothrow) { return r; }
};

// range adaptor whose `base` can only be called on rvalues
template<typename R>
struct rvalue_base_wrapper
{
    R r;

    constexpr auto begin() const { return std::ranges::begin(r); }
    constexpr auto end() const { return std::ranges::end(r); }

    constexpr R base() && noexcept { return std::move(r); }
};

// not a range, but has `base`
struct not_a_range
{
    constexpr std::span<int> base() const noexcept { return {}; }
};

static_assert(std::ranges::range<wrapper<std::span<int>>>);
static_assert(std::ranges::range<rvalue_base_wrapper<std::span<int>>>);
static_assert(!std::ranges::range<not_a_range>);

template<typename T>
concept has_lowest_base = requires (T &&r) { get_lowest_base(std::forward<T>(r)); };

consteval void test() {
    using vec = std::vector<int>;
    using span = std::span<int>;

    int arr[]{1, 2, 3, 4, 5};

    // range without `base`, lvalue: returns a reference to the range itself
    {
        vec v{1, 2, 3};
        static_assert(std::same_as<decltype(get_lowest_base(v)), vec &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::as_const(v))), vec const &>);
        static_assert(std::same_as<decltype(get_lowest_base(arr)), int (&)[5]>);

        if (&get_lowest_base(v) != &v) {
            throw "lvalue range without base must be returned by reference";
        }
        if (&get_lowest_base(arr) != &arr) {
            throw "lvalue array must be returned by reference";
        }
    }

    // range without `base`, rvalue: returns the range by value
    {
        vec v{1, 2, 3};
        static_assert(std::same_as<decltype(get_lowest_base(std::move(v))), vec>);
        static_assert(std::same_as<decltype(get_lowest_base(span{arr})), span>);

        if (get_lowest_base(std::move(v)) != vec{1, 2, 3}) {
            throw "rvalue range without base returned the wrong range";
        }
        if (get_lowest_base(span{arr}).data() != arr) {
            throw "rvalue span returned the wrong range";
        }
    }

    // single level of wrapping: `ref_view::base` returns a reference to the referred range
    {
        vec v{1, 2, 3};
        auto rv = std::views::all(v);
        static_assert(std::same_as<decltype(rv), std::ranges::ref_view<vec>>);
        static_assert(std::same_as<decltype(get_lowest_base(rv)), vec &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::as_const(rv))), vec &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::move(rv))), vec &>);

        if (&get_lowest_base(rv) != &v || &get_lowest_base(std::move(rv)) != &v) {
            throw "ref_view unwrapped to the wrong range";
        }
    }

    // multiple levels of wrapping
    {
        vec v{1, 2, 3, 4};
        auto nested = v | std::views::take(3) | std::views::drop(1) | std::views::reverse;
        static_assert(std::same_as<decltype(get_lowest_base(nested)), vec &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::move(nested))), vec &>);

        if (&get_lowest_base(nested) != &v) {
            throw "nested view unwrapped to the wrong range";
        }

        using wrapped = wrapper<wrapper<decltype(nested)>>;
        wrapped w{wrapper<decltype(nested)>{nested}};
        static_assert(std::same_as<decltype(get_lowest_base(w)), vec &>);

        if (&get_lowest_base(w) != &v) {
            throw "nested custom wrapper unwrapped to the wrong range";
        }
    }

    // `base` returning a reference, lvalue: the reference is preserved
    {
        auto ov = std::views::all(vec{1, 2, 3});
        static_assert(std::same_as<decltype(ov), std::ranges::owning_view<vec>>);
        static_assert(std::same_as<decltype(get_lowest_base(ov)), vec &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::as_const(ov))), vec const &>);

        if (&get_lowest_base(ov) != &ov.base()) {
            throw "lvalue owning_view must return a reference to its base";
        }
    }

    // `base` returning a reference, rvalue: the base is returned by value
    {
        auto ov = std::views::all(vec{1, 2, 3});
        static_assert(std::same_as<decltype(get_lowest_base(std::move(ov))), vec>);

        if (get_lowest_base(std::move(ov)) != vec{1, 2, 3}) {
            throw "rvalue owning_view unwrapped to the wrong range";
        }
    }

    // move-only views: lvalue `base` is unavailable, so unwrapping stops early
    {
        auto rev = vec{1, 2, 3} | std::views::reverse;
        using rev_t = decltype(rev);
        static_assert(std::same_as<rev_t, std::ranges::reverse_view<std::ranges::owning_view<vec>>>);

        // `reverse_view::base() const &` requires a copyable underlying view
        static_assert(std::same_as<decltype(get_lowest_base(rev)), rev_t &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::move(rev))), vec>);

        if (&get_lowest_base(rev) != &rev) {
            throw "lvalue reverse_view over owning_view must not be unwrapped";
        }
        if (get_lowest_base(std::move(rev)) != vec{1, 2, 3}) {
            throw "rvalue reverse_view over owning_view unwrapped to the wrong range";
        }
    }

    // value category is forwarded to `base`
    {
        using w = rvalue_base_wrapper<span>;
        w r{span{arr}};

        // `base` is not callable on lvalues, so the wrapper itself is the lowest base
        static_assert(std::same_as<decltype(get_lowest_base(r)), w &>);
        static_assert(std::same_as<decltype(get_lowest_base(std::move(r))), span>);

        if (&get_lowest_base(r) != &r) {
            throw "lvalue with rvalue-only base must not be unwrapped";
        }
        if (get_lowest_base(std::move(r)).data() != arr) {
            throw "rvalue with rvalue-only base unwrapped to the wrong range";
        }
    }

    // SFINAE-friendliness
    {
        static_assert(has_lowest_base<vec>);
        static_assert(has_lowest_base<vec &>);
        static_assert(has_lowest_base<vec const &>);
        static_assert(has_lowest_base<int (&)[5]>);
        static_assert(has_lowest_base<span>);
        static_assert(has_lowest_base<std::ranges::ref_view<vec>>);
        static_assert(has_lowest_base<std::ranges::owning_view<vec>>);
        static_assert(has_lowest_base<wrapper<span>>);
        static_assert(has_lowest_base<std::ranges::iota_view<int, int>>);

        static_assert(!has_lowest_base<int>);
        static_assert(!has_lowest_base<int *>);
        static_assert(!has_lowest_base<vec::iterator>);
        static_assert(!has_lowest_base<not_a_range>);
        static_assert(!has_lowest_base<not_a_range &>);
    }

    // `noexcept` propagation
    {
        vec v{1, 2, 3};
        static_assert(noexcept(get_lowest_base(v)));
        static_assert(noexcept(get_lowest_base(std::move(v))));

        wrapper<span, true> nothrow_w{span{arr}};
        wrapper<span, false> throw_w{span{arr}};
        static_assert(noexcept(get_lowest_base(nothrow_w)));
        static_assert(!noexcept(get_lowest_base(throw_w)));

        // a throwing `base` at any level makes the whole call potentially throwing
        wrapper<wrapper<span, false>, true> outer_nothrow{throw_w};
        wrapper<wrapper<span, true>, false> outer_throw{nothrow_w};
        static_assert(!noexcept(get_lowest_base(outer_nothrow)));
        static_assert(!noexcept(get_lowest_base(outer_throw)));

        wrapper<wrapper<span, true>, true> all_nothrow{nothrow_w};
        static_assert(noexcept(get_lowest_base(all_nothrow)));

        if (get_lowest_base(outer_nothrow).data() != arr
            || get_lowest_base(outer_throw).data() != arr
            || get_lowest_base(all_nothrow).data() != arr) {
            throw "nested wrapper unwrapped to the wrong range";
        }
    }
}

} // anonymous namespace

int main() {
    test();
}
