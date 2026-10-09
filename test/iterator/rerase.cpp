#include <concepts>
#include <iterator>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

import grace.iterator;

using grace::iterator::rerase;

namespace {

// minimal container whose `erase` is `noexcept`, used to check `noexcept`
// propagation
struct noexcept_container
{
    using iterator = std::vector<int>::iterator;
    using const_iterator = std::vector<int>::const_iterator;

    std::vector<int> data;

    constexpr iterator erase(const_iterator it) noexcept { return data.erase(it); }
    constexpr iterator erase(const_iterator first, const_iterator last) noexcept { return data.erase(first, last); }
};

// container without `erase`
struct no_erase_container
{
    using iterator = int *;
    using const_iterator = int const *;
};

constexpr bool implies(bool a, bool b) { return !a || b; }

// whether the `std::reverse_iterator` operations `rerase` relies on are
// `noexcept`: copying, incrementing and unwrapping `std::reverse_iterator<It>`,
// and wrapping the `ErasedIt` returned by `erase`
template<typename It, typename ErasedIt>
constexpr bool nothrow_reverse_iterator_ops = requires (std::reverse_iterator<It> rit, ErasedIt it) {
    requires std::is_nothrow_copy_constructible_v<std::reverse_iterator<It>>;
    requires noexcept(++rit);
    requires noexcept(rit.base());
    requires noexcept(std::make_reverse_iterator(it));
};

template<typename Container, typename RevIt>
concept rerasable = requires (Container &c, RevIt rit) { rerase(c, rit); };

template<typename Container, typename RevIt>
concept range_rerasable = requires (Container &c, RevIt rbegin, RevIt rend) { rerase(c, rbegin, rend); };

consteval void test() {
    using vec = std::vector<int>;
    using rit = std::reverse_iterator<vec::iterator>;
    using crit = std::reverse_iterator<vec::const_iterator>;

    // single element, mutable iterator
    {
        vec v{1, 2, 3, 4, 5};
        auto it = rerase(v, v.rbegin() + 1);
        static_assert(std::same_as<decltype(it), rit>);

        if (v != vec{1, 2, 3, 5}) {
            throw "single element erase removed the wrong element";
        }
        if (it == v.rend() || *it != 3) {
            throw "single element erase returned the wrong iterator";
        }
    }

    // single element, `rbegin`
    {
        vec v{1, 2, 3};
        auto it = rerase(v, v.rbegin());

        if (v != vec{1, 2}) {
            throw "rbegin erase removed the wrong element";
        }
        if (it != v.rbegin()) {
            throw "rbegin erase must return the new rbegin";
        }
    }

    // single element, last element in reverse order
    {
        vec v{1, 2, 3};
        auto it = rerase(v, std::prev(v.rend()));

        if (v != vec{2, 3}) {
            throw "last reverse element erase removed the wrong element";
        }
        if (it != v.rend()) {
            throw "last reverse element erase must return rend";
        }
    }

    // single element, only element
    {
        vec v{1};
        auto it = rerase(v, v.rbegin());

        if (!v.empty()) {
            throw "only element erase failed";
        }
        if (it != v.rend()) {
            throw "only element erase must return rend";
        }
    }

    // single element, const iterator
    {
        vec v{1, 2, 3, 4, 5};
        auto it = rerase(v, v.crbegin() + 1);
        static_assert(std::same_as<decltype(it), rit>);

        if (v != vec{1, 2, 3, 5}) {
            throw "const single element erase removed the wrong element";
        }
        if (it == v.rend() || *it != 3) {
            throw "const single element erase returned the wrong iterator";
        }
    }

    // range, mutable iterators
    {
        vec v{1, 2, 3, 4, 5};
        auto it = rerase(v, v.rbegin() + 1, v.rbegin() + 3);
        static_assert(std::same_as<decltype(it), rit>);

        if (v != vec{1, 2, 5}) {
            throw "range erase removed the wrong elements";
        }
        if (it == v.rend() || *it != 2) {
            throw "range erase returned the wrong iterator";
        }
    }

    // range, empty
    {
        vec v{1, 2, 3};
        auto it = rerase(v, v.rbegin() + 1, v.rbegin() + 1);

        if (v != vec{1, 2, 3}) {
            throw "empty range erase must be a no-op";
        }
        if (it != v.rbegin() + 1) {
            throw "empty range erase must return the passed iterator";
        }
    }

    // range, whole container
    {
        vec v{1, 2, 3};
        auto it = rerase(v, v.rbegin(), v.rend());

        if (!v.empty()) {
            throw "whole range erase failed";
        }
        if (it != v.rend()) {
            throw "whole range erase must return rend";
        }
    }

    // range, prefix in reverse order
    {
        vec v{1, 2, 3, 4};
        auto it = rerase(v, v.rbegin(), v.rbegin() + 2);

        if (v != vec{1, 2}) {
            throw "reverse prefix range erase removed the wrong elements";
        }
        if (it != v.rbegin()) {
            throw "reverse prefix range erase must return the new rbegin";
        }
    }

    // range, const iterators
    {
        vec v{1, 2, 3, 4, 5};
        auto it = rerase(v, v.crbegin() + 1, v.crbegin() + 3);
        static_assert(std::same_as<decltype(it), rit>);

        if (v != vec{1, 2, 5}) {
            throw "const range erase removed the wrong elements";
        }
        if (it == v.rend() || *it != 2) {
            throw "const range erase returned the wrong iterator";
        }
    }

    // erasing while iterating in reverse
    {
        vec v{1, 2, 3, 4, 5, 6};
        for (auto it = v.rbegin(); it != v.rend();) {
            if (*it % 2 == 0) {
                it = rerase(v, it);
            } else {
                ++it;
            }
        }

        if (v != vec{1, 3, 5}) {
            throw "erasing while iterating in reverse failed";
        }
    }

    // SFINAE-friendliness
    {
        static_assert(rerasable<vec, rit>);
        static_assert(rerasable<vec, crit>);
        static_assert(range_rerasable<vec, rit>);
        static_assert(range_rerasable<vec, crit>);

        static_assert(!rerasable<vec const, rit>);
        static_assert(!rerasable<vec const, crit>);
        static_assert(!range_rerasable<vec const, rit>);
        static_assert(!range_rerasable<vec const, crit>);

        static_assert(!rerasable<vec, vec::iterator>);
        static_assert(!range_rerasable<vec, vec::iterator>);

        static_assert(!rerasable<no_erase_container, std::reverse_iterator<int *>>);
        static_assert(!range_rerasable<no_erase_container, std::reverse_iterator<int *>>);
    }

    // `noexcept` propagation
    {
        vec v;
        static_assert(!noexcept(rerase(v, v.rbegin())));
        static_assert(!noexcept(rerase(v, v.rbegin(), v.rend())));

        // whether `std::reverse_iterator` operations are `noexcept` depends on
        // the standard library implementation, so `rerase` with a `noexcept`
        // `erase` is only required to be `noexcept` if they are
        noexcept_container c{{1, 2, 3}};

        using it_t = noexcept_container::iterator;
        using cit_t = noexcept_container::const_iterator;

        static_assert(implies(
            nothrow_reverse_iterator_ops<it_t, it_t>,
            noexcept(rerase(c, c.data.rbegin()))
        ));
        static_assert(implies(
            nothrow_reverse_iterator_ops<it_t, it_t>,
            noexcept(rerase(c, c.data.rbegin(), c.data.rend()))
        ));
        static_assert(implies(
            nothrow_reverse_iterator_ops<cit_t, it_t>,
            noexcept(rerase(c, c.data.crbegin()))
        ));
        static_assert(implies(
            nothrow_reverse_iterator_ops<cit_t, it_t>,
            noexcept(rerase(c, c.data.crbegin(), c.data.crend()))
        ));

        auto it = rerase(c, c.data.rbegin());
        if (c.data != vec{1, 2} || it != c.data.rbegin()) {
            throw "custom container erase failed";
        }
    }
}

// containers where `iterator` and `const_iterator` are the same type
// (`std::set` is not `constexpr`, so this is a runtime test)
void test_same_iterator_types() {
    using set = std::set<int>;
    static_assert(std::same_as<set::iterator, set::const_iterator>);

    static_assert(rerasable<set, std::reverse_iterator<set::iterator>>);
    static_assert(range_rerasable<set, std::reverse_iterator<set::iterator>>);

    {
        set s{1, 2, 3, 4, 5};
        auto it = rerase(s, std::next(s.rbegin()));
        static_assert(std::same_as<decltype(it), std::reverse_iterator<set::iterator>>);

        if (s != set{1, 2, 3, 5}) {
            throw "set single element erase removed the wrong element";
        }
        if (it == s.rend() || *it != 3) {
            throw "set single element erase returned the wrong iterator";
        }
    }

    {
        set s{1, 2, 3, 4, 5};
        auto it = rerase(s, std::next(s.rbegin()), std::next(s.rbegin(), 3));

        if (s != set{1, 2, 5}) {
            throw "set range erase removed the wrong elements";
        }
        if (it == s.rend() || *it != 2) {
            throw "set range erase returned the wrong iterator";
        }
    }
}

} // anonymous namespace

int main() {
    test();
    test_same_iterator_types();
}
