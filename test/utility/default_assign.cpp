#include <concepts>
#include <utility>

import grace.utility;

namespace {

struct swap_member {
public:
    static inline int int_ctor_calls = 0;
    static inline int copy_ctor_calls = 0;
    static inline int move_ctor_calls = 0;
    static inline int dtor_calls = 0;
    static inline int swap_calls = 0;

    static void reset_counters() {
        int_ctor_calls = 0;
        copy_ctor_calls = 0;
        move_ctor_calls = 0;
        dtor_calls = 0;
        swap_calls = 0;
    }

public:
    int value = 0;

public:
    swap_member() = default;
    explicit swap_member(int v) : value(v) { ++int_ctor_calls; }
    swap_member(const swap_member &rhs) : value(rhs.value) { ++copy_ctor_calls; }
    swap_member(swap_member &&rhs) : value(rhs.value) { ++move_ctor_calls; }
    ~swap_member() { ++dtor_calls; }

    swap_member &operator=(auto &&rhs) {
        return grace::utility::default_assign(*this, std::forward<decltype(rhs)>(rhs));
    }

    void swap(swap_member &other) noexcept {
        ++swap_calls;
        std::swap(value, other.value);
    }
};

struct adl_swap {
    int value = 0;

    adl_swap() = default;
    explicit adl_swap(int v) : value(v) {}
    adl_swap(const adl_swap &) = default;
    adl_swap(adl_swap &&) = default;

    adl_swap &operator=(auto &&rhs) {
        return grace::utility::default_assign(*this, std::forward<decltype(rhs)>(rhs));
    }


    friend void swap(adl_swap &a, adl_swap &b) noexcept {
        std::swap(a.value, b.value);
    }
};

// A type that is not swappable at all: `default_assign` must be unavailable
// for it via SFINAE (the `requires` clause / trailing return type).
struct not_swappable {
    int value = 0;
    explicit not_swappable(int v) : value(v) {}
};

template<typename To, typename FwdFrom>
concept is_default_assignable = requires(To &to, FwdFrom &&from) {
    { grace::utility::default_assign(to, std::forward<FwdFrom>(from)) } -> std::same_as<To &>;
};

// A type that is swappable, but whose `.swap` is not `noexcept`.
// `default_assign` requires `noexcept(swap)`, so this must be SFINAE'd out.
struct throwing_swap {
    int value = 0;
    explicit throwing_swap(int v) : value(v) {}

    void swap(throwing_swap &other) // deliberately not noexcept
    {
        std::swap(value, other.value);
    }
};

// A type usable in a constexpr context: literal type, constexpr constructor
// and constexpr, noexcept `.swap`.
struct constexpr_swap {
    int value = 0;

    constexpr constexpr_swap() = default;
    constexpr explicit constexpr_swap(int v) : value(v) {}
    constexpr constexpr_swap(const constexpr_swap &) = default;
    constexpr constexpr_swap(constexpr_swap &&) = default;

    constexpr void swap(constexpr_swap &other) noexcept
    {
        std::swap(value, other.value);
    }

    constexpr constexpr_swap &operator=(auto &&rhs)
    {
        return grace::utility::default_assign(*this, std::forward<decltype(rhs)>(rhs));
    }
};

} // namespace

int main()
{
    // member `.swap` path
    {
        swap_member::reset_counters();

        swap_member a{1};
        swap_member b{2};

        a = b;

        if (a.value != 2) {
            throw "Unexpected value after copy assignment via member swap";
        }

        if (swap_member::swap_calls != 1) {
            throw "Unexpected number of swap calls after copy assignment via member swap";
        }

        if (swap_member::copy_ctor_calls != 1) {
            throw "Unexpected number of copy constructor calls after copy assignment via member swap";
        }
    }

    // self-assignment is optimized away (no swap performed)
    {
        swap_member::reset_counters();

        swap_member a{42};

        a = a;

        if (a.value != 42) {
            throw "Unexpected value after self-assignment";
        }

        if (swap_member::swap_calls != 0) {
            throw "Unexpected number of swap calls after self-assignment";
        }
    }

    // `To` != `FwdFrom`
    {
        swap_member::reset_counters();
        swap_member a;
        a = 42;

        if (a.value != 42) {
            throw "Unexpected value after assignment from int";
        }

        if (swap_member::int_ctor_calls != 1) {
            throw "Unexpected number of int constructor calls after assignment from int";
        }

        if (swap_member::swap_calls != 1) {
            throw "Unexpected number of swap calls after assignment from int";
        }
    }

    // move-assignment also goes through the swap path
    {
        swap_member::reset_counters();

        swap_member a{1};
        swap_member b{2};

        a = std::move(b);

        if (a.value != 2) {
            throw "Unexpected value after move assignment via member swap";
        }

        if (swap_member::swap_calls != 1) {
            throw "Unexpected number of swap calls after move assignment via member swap";
        }
    }

    // ADL `swap` path
    {
        adl_swap a{1};
        adl_swap b{2};

        a = b;

        if (a.value != 2) {
            throw "Unexpected value after copy assignment via ADL swap";
        }
    }

    // sanity check: the detection idiom reports true for the working cases
    static_assert(is_default_assignable<swap_member, const swap_member &>);
    static_assert(is_default_assignable<adl_swap, const adl_swap &>);

    // constrained away when the type isn't swappable
    static_assert(!is_default_assignable<not_swappable, not_swappable &&>);

    // constrained away when there's no constructor from `FwdFrom`
    static_assert(!is_default_assignable<swap_member, std::nullptr_t>);

    // constrained away when `.swap` is not `noexcept`
    static_assert(!is_default_assignable<throwing_swap, const throwing_swap &>);

    // works in a constexpr context
    {
        constexpr constexpr_swap result = [] {
            constexpr_swap a{1};
            constexpr_swap b{2};
            a = b;
            return a;
        }();

        static_assert(result.value == 2);
    }
}
