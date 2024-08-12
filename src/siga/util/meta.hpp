#pragma once

namespace siga::util {

// idea from Ed Catmur. usage example: `conceptify<std::is_enum>`
template<typename T, template<typename...> typename Trait>
concept conceptify = Trait<T>::value;

// -------------------------------------------------------------------------------------------------

template<typename T, template<typename...> typename Trait, template<typename...> typename... Rest>
class combine_traits
{
public:
    using type = combine_traits<typename Trait<T>::type, Rest...>::type;
};

template<typename T, template<typename...> typename Trait>
class combine_traits<T, Trait>
{
public:
    using type = Trait<T>::type;
};

template<typename T, template<typename...> typename... Traits>
using combine_traits_t = combine_traits<T, Traits...>::type;

#if 0
static_assert(
    std::same_as<
        combine_traits_t<const int &, std::remove_reference, std::remove_cv>,
        int
    >
);
#endif

// -------------------------------------------------------------------------------------------------

template<typename... Ts>
class [[nodiscard]] type_list
{};

// -------------------------------------------------------------------------------------------------

template<std::size_t N>
struct [[nodiscard]] string_literal
{
public:
    static_assert(N > 0);
    static constexpr std::size_t size = N - 1;

public:
    constexpr string_literal() = default;

    constexpr string_literal(const char (&str)[N])
    {
        if(str[N - 1] == '\0') {
            std::ranges::copy(str, std::ranges::begin(data));
        } else {
            throw std::invalid_argument{"the string must be null-terminated"};
        }
    }

public:
    [[nodiscard]] constexpr std::string_view as_view() const noexcept { return data; }
    [[nodiscard]] constexpr operator std::string_view() const noexcept { return as_view(); }

public:
    char data[N] = {};
};

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_const
{
public:
    // TODO: should we remove `const` from `To` if `From` doesn't have it?
    using type = To;
};

template<typename From, typename To>
class copy_const<const From, To>
{
public:
    using type = const To;
};

template<typename From, typename To>
using copy_const_t = copy_const<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_volatile
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_volatile<volatile From, To>
{
public:
    using type = volatile To;
};

template<typename From, typename To>
using copy_volatile_t = copy_volatile<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_cv
{
public:
    using type = copy_const_t<From, copy_volatile_t<From, To>>;
};

template<typename From, typename To>
using copy_cv_t = copy_cv<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_lvalue_ref
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_lvalue_ref<From &, To>
{
public:
    using type = To &;
};

template<typename From, typename To>
using copy_lvalue_ref_t = copy_lvalue_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_rvalue_ref
{
public:
    using type = To;
};

// TODO: this class won't copy the rvalue reference if `To` is an lvalue reference type.
//       whether it is okay or not is a philosophical question
template<typename From, typename To>
class copy_rvalue_ref<From &&, To>
{
public:
    using type = To &&;
};

template<typename From, typename To>
using copy_rvalue_ref_t = copy_rvalue_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_ref
{
public:
    using type = copy_lvalue_ref_t<From, copy_rvalue_ref_t<From, To>>;
};

template<typename From, typename To>
using copy_ref_t = copy_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_cv_ref
{
private:
    using from_unref_t = std::remove_reference_t<From>;
    using to_unref_t = std::remove_reference_t<To>;
    using to_cv_t = copy_cv_t<from_unref_t, to_unref_t>;
    using to_cv_ref_t = copy_ref_t<From, to_cv_t>;

public:
    using type = to_cv_ref_t;
};

template<typename From, typename To>
using copy_cv_ref_t = copy_cv_ref<From, To>::type;

} // namespace siga::util
