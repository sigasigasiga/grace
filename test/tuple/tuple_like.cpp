#include <ranges>
#include <string>
#include <tuple>

import grace.tuple;

struct member_get
{
public:
    template<std::size_t I>
    int get()
    {
        return I;
    }
};

template<>
struct std::tuple_size<member_get> : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, member_get> : std::type_identity<int>
{};

// -------------------------------------------------------------------------------------------------

struct mismatching_types
{
public:
    template<std::size_t I>
    const char *get()
    {
        return "";
    }
};

template<>
struct std::tuple_size<mismatching_types> : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, mismatching_types> : std::type_identity<std::string>
{};

// -------------------------------------------------------------------------------------------------

struct explicit_constructor
{
    explicit explicit_constructor(int) {}
};

struct mismatching_and_explicit
{
    template<std::size_t I>
    int get()
    {
        return I;
    }
};

template<>
struct std::tuple_size<mismatching_and_explicit> : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, mismatching_and_explicit> : std::type_identity<explicit_constructor>
{};

// -------------------------------------------------------------------------------------------------

struct adl_get_is_a_friend
{
    template<std::size_t I>
    friend int get(adl_get_is_a_friend)
    {
        return I;
    }
};

template<>
struct std::tuple_size<adl_get_is_a_friend> : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, adl_get_is_a_friend> : std::type_identity<int>
{};

// -------------------------------------------------------------------------------------------------

struct some_with_member_and_some_with_adl
{
    template<std::size_t I>
    requires(I == 0)
    int get()
    {
        return I;
    }

    template<std::size_t I>
    requires(I == 1)
    friend int get(some_with_member_and_some_with_adl)
    {
        return I;
    }
};

template<>
struct std::tuple_size<some_with_member_and_some_with_adl> : std::integral_constant<std::size_t, 2>
{};

template<std::size_t I>
struct std::tuple_element<I, some_with_member_and_some_with_adl> : std::type_identity<int>
{};

// -------------------------------------------------------------------------------------------------

struct some_with_member_and_some_with_adl_2
{
    // a function that'd never be actually used because `tuple_size == 2`
    template<std::size_t I>
    requires(I > 100)
    int get()
    {
        return I;
    }

    template<std::size_t I>
    friend int get(some_with_member_and_some_with_adl_2)
    {
        return I;
    }
};

template<>
struct std::tuple_size<some_with_member_and_some_with_adl_2>
    : std::integral_constant<std::size_t, 2>
{};

template<std::size_t I>
struct std::tuple_element<I, some_with_member_and_some_with_adl_2> : std::type_identity<int>
{};

// -------------------------------------------------------------------------------------------------

struct type_is_a_ref_but_get_returns_a_value
{
public:
    template<std::size_t I>
    int get()
    {
        return I;
    }
};

template<>
struct std::tuple_size<type_is_a_ref_but_get_returns_a_value>
    : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, type_is_a_ref_but_get_returns_a_value> : std::type_identity<int &>
{};

// -------------------------------------------------------------------------------------------------

struct convertible_to_rvalue_int
{
    operator int &&()
    {
        static int x;
        return std::move(x);
    }
};

struct mismatching_types_convertible_to_rvalue
{
    template<std::size_t I>
    convertible_to_rvalue_int get()
    {
        return {};
    }
};

template<>
struct std::tuple_size<mismatching_types_convertible_to_rvalue>
    : std::integral_constant<std::size_t, 1>
{};

template<>
struct std::tuple_element<0, mismatching_types_convertible_to_rvalue> : std::type_identity<int>
{};

// -------------------------------------------------------------------------------------------------

int main()
{
    using namespace grace::tuple;

    // `get` returns a value
    static_assert(tuple_like<std::ranges::subrange<int *, void *>>);
    static_assert(tuple_like<std::ranges::subrange<int *, void *> &>);
    static_assert(tuple_like<std::ranges::subrange<int *, void *> &&>);

    // `get` returns a reference
    static_assert(tuple_like<std::tuple<>>);
    static_assert(tuple_like<std::tuple<int &>>);
    static_assert(tuple_like<std::tuple<int &&>>);
    static_assert(tuple_like<std::tuple<int &> &>);
    static_assert(tuple_like<std::tuple<int &&> &>);

    // `get` is a member function
    static_assert(tuple_like<member_get>);

    // `get` returns type that is convertible to `tuple_element_t`
    static_assert(tuple_like<mismatching_types>);

    // `get` is a friend function defined in the class
    static_assert(tuple_like<adl_get_is_a_friend>);

    // self-explanatory
    static_assert(not tuple_like<some_with_member_and_some_with_adl>);

    // FIXME? it works but probably shouldn't because structured bindings don't work with it
    // though there's probably no way it can be fixed
    static_assert(tuple_like<some_with_member_and_some_with_adl_2>);

    // `get` returns a type that is only _explicitly_ convertible to `tuple_element_t`
    static_assert(not tuple_like<mismatching_and_explicit>);

    // self-explanatory
    static_assert(not tuple_like<type_is_a_ref_but_get_returns_a_value>);

    // `tuple_element` = `int`; `get` returns a type that converts to rvalue `int`
    static_assert(tuple_like<mismatching_types_convertible_to_rvalue>);
}
