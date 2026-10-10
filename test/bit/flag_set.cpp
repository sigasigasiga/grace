#include <concepts>
#include <cstdint>
#include <functional>
#include <type_traits>

import grace.bit;

namespace {

using grace::bit::flag_set;

enum class flags : std::uint8_t {
    none = 0,
    a = 1 << 0,
    b = 1 << 1,
    c = 1 << 2,
};

enum unscoped_flags : unsigned {
    unscoped_a = 1 << 0,
    unscoped_b = 1 << 1,
};

template<typename T>
concept valid_flag_set = requires { typename flag_set<T>; };

} // namespace

int main()
{
    using set = flag_set<flags>;

    // only enums are accepted
    static_assert(valid_flag_set<flags>);
    static_assert(valid_flag_set<unscoped_flags>);
    static_assert(!valid_flag_set<int>);
    static_assert(!valid_flag_set<std::uint8_t>);

    static_assert(std::same_as<set::value_type, std::uint8_t>);
    static_assert(std::same_as<flag_set<unscoped_flags>::value_type, unsigned>);

    // trivial and cheap to pass by value
    static_assert(std::is_trivially_copyable_v<set>);
    static_assert(sizeof(set) == sizeof(std::uint8_t));

    // implicit construction from the enum, explicit from the underlying type
    static_assert(std::is_nothrow_constructible_v<set, flags>);
    static_assert(std::is_convertible_v<flags, set>);
    static_assert(std::is_nothrow_constructible_v<set, std::uint8_t>);
    static_assert(!std::is_convertible_v<std::uint8_t, set>);
    static_assert(!std::is_convertible_v<unscoped_flags, set>);

    // value-initialization yields an empty set
    static_assert(set{}.value() == 0);
    static_assert(set{} == flags::none);

    static_assert(set{flags::b}.value() == 0b010);
    static_assert(set{std::uint8_t{0b101}}.value() == 0b101);

    // explicit conversions to the underlying type and `bool` only
    static_assert(std::is_constructible_v<std::uint8_t, set>);
    static_assert(!std::is_convertible_v<set, std::uint8_t>);
    static_assert(std::is_constructible_v<bool, set>);
    static_assert(!std::is_convertible_v<set, bool>);
    static_assert(!std::is_constructible_v<int, set>);
    static_assert(!std::is_constructible_v<flags, set>);

    static_assert(static_cast<std::uint8_t>(set{flags::c}) == 0b100);
    static_assert(static_cast<bool>(set{flags::a}));
    static_assert(!static_cast<bool>(set{flags::none}));
    static_assert(set{flags::a} ? true : false);

    // bitwise operators
    static_assert((set{flags::a} | flags::b).value() == 0b011);
    static_assert((flags::a | set{flags::b}).value() == 0b011);
    static_assert(((set{flags::a} | flags::b) & flags::b) == flags::b);
    static_assert(((set{flags::a} | flags::b) & flags::c) == flags::none);
    static_assert((~set{flags::a}).value() == 0b1111'1110);
    static_assert(~~set{flags::a} == flags::a);
    static_assert(((set{flags::a} | flags::b) ^ flags::b) == flags::a);
    static_assert(((set{flags::a} | flags::b) ^ flags::c).value() == 0b111);
    static_assert((flags::a ^ set{flags::a}) == flags::none);
    static_assert((set{flags::a} ^ ~set{flags::a}).value() == 0b1111'1111);

    static_assert(noexcept(set{} | set{}));
    static_assert(noexcept(set{} & set{}));
    static_assert(noexcept(set{} ^ set{}));
    static_assert(noexcept(~set{}));
    static_assert(std::same_as<decltype(set{} | flags::a), set>);
    static_assert(std::same_as<decltype(set{} & flags::a), set>);
    static_assert(std::same_as<decltype(set{} ^ flags::a), set>);
    static_assert(std::same_as<decltype(~set{}), set>);

    // compound assignment
    static_assert([] {
        set s{};

        s |= flags::a;
        s |= flags::c;
        if (s.value() != 0b101) {
            return false;
        }

        s &= flags::c;
        if (s != flags::c) {
            return false;
        }

        s &= ~set{flags::c};
        if (s != flags::none) {
            return false;
        }

        // toggling twice restores the original value
        s ^= flags::b;
        if (s != flags::b) {
            return false;
        }

        s ^= set{flags::a} | flags::b;
        if (s != flags::a) {
            return false;
        }

        s ^= flags::a;
        return s == flags::none;
    }());

    static_assert(std::same_as<decltype(std::declval<set &>() |= flags::a), set &>);
    static_assert(std::same_as<decltype(std::declval<set &>() &= flags::a), set &>);
    static_assert(std::same_as<decltype(std::declval<set &>() ^= flags::a), set &>);
    static_assert(noexcept(std::declval<set &>() ^= flags::a));

    // equality
    static_assert(set{flags::a} == set{flags::a});
    static_assert(set{flags::a} != set{flags::b});
    static_assert(set{flags::a} == flags::a);
    static_assert(flags::a == set{flags::a});
    static_assert(set{std::uint8_t{0b011}} == (set{flags::a} | flags::b));

    // unscoped enums with a non-`uint8_t` underlying type
    static_assert((flag_set<unscoped_flags>{unscoped_a} | unscoped_b).value() == 0b11u);
    static_assert((~flag_set<unscoped_flags>{unscoped_a}).value() == ~1u);
    static_assert((~flag_set<unscoped_flags>{unscoped_a} ^ unscoped_b).value() == ~0b11u);

    // hash is the hash of the underlying value
    constexpr std::hash<set> h;
    static_assert(noexcept(h(set{})));
    if (h(set{flags::a} | flags::c) != std::hash<std::uint8_t>{}(0b101)) {
        return 1;
    }
}
